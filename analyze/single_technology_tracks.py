#!/usr/bin/python

import sys
import json
import time
from typing import Dict
from collections import defaultdict
import argparse

from util.record_extractor import RecordExtractor
from util.common import find_value, time_str_from_seconds
import util.track


def print_warn(message, end='\n'):
    sys.stderr.write('\x1b[1;33m' + message.strip() + '\x1b[0m' + end)

# filter functions return True if record should be skipped
def filter_tracks(cat, record):
    if cat != 62:
        return True

    #return find_value("040.Track Number", record) != 4592

    return False


max_age_for_update = 12


class TrackAges:
    def __init__(self, psr_age, ssr_age, mds_age, mlt_age, ads_age):
        self._psr_age = psr_age
        self._ssr_age = ssr_age
        self._mds_age = mds_age
        self._mlt_age = mlt_age
        self._ads_age = ads_age

    def is_single_technology(self):

        is_single_tech = sum((False if self._psr_age is None else self._psr_age <= max_age_for_update,
                              False if self._ssr_age is None else self._ssr_age <= max_age_for_update,
                              False if self._mds_age is None else self._mds_age <= max_age_for_update,
                              False if self._mlt_age is None else self._mlt_age <= max_age_for_update,
                              False if self._ads_age is None else self._ads_age <= max_age_for_update)) == 1

        print('tech PSR {} SSR {} MDS {} MLT {} ADS {} single {}'.format(
            self._psr_age, self._ssr_age, self._mds_age, self._mlt_age, self._ads_age, is_single_tech))

        return is_single_tech

    def get_single_technology(self):

        #global max_age_for_update

        if self._psr_age <= max_age_for_update:
            return "PSR"
        if self._ssr_age <= max_age_for_update:
            return "SSR"
        if self._mds_age <= max_age_for_update:
            return "MDS"
        if self._mlt_age <= max_age_for_update:
            return "MLT"
        if self._ads_age <= max_age_for_update:
            return "ADS"

        return None


class TrackStatistic:
    def __init__(self, utn, track_num):
        self._utn = utn
        self._track_num = track_num
        self._num_records = 0

        self._first_tod = None
        self._last_tod = None

        self._mode_a_codes = {}
        self._target_addresses = {}
        self._target_identifications = {}

        self._track_ages = defaultdict(set)  # tod -> TrackAges
        self._num_multi_technology_updates = 0
        self._num_single_technology_updates = 0
        self._num_single_technology_counts = {}

        self._single_technology_ratio = 0

        self._single_tech_track = False

    @property
    def single_tech_track(self):
        return self._single_tech_track

    def process_record(self, cat, record):

        self._num_records += 1

        tod = find_value("070.Time Of Track Information", record)
        assert tod is not None

        if self._first_tod is None:
            self._first_tod = tod
            self._last_tod = tod

        if tod > self._last_tod:
            self._last_tod = tod
        elif tod < self._last_tod:
            print('warn: utn {} track_num {} record {} time jump from {} to {} diff {}'.format(
                self._utn, self._track_num, self._num_records,
                time_str_from_seconds(self._last_tod), time_str_from_seconds(tod),
                time_str_from_seconds(self._last_tod-tod)))

        track_num = find_value("040.Track Number", record)
        assert track_num is not None

        assert track_num == self._track_num

        # process mode 3/a code
        mode_a_code = find_value("060.Mode-3/A reply", record)  # already oct

        if mode_a_code not in self._mode_a_codes:
            self._mode_a_codes[mode_a_code] = 1
        else:
            self._mode_a_codes[mode_a_code] += 1

        # process mode s address
        target_addr = find_value("380.ADR.Target Address", record)

        if target_addr is not None:
            target_addr = hex(target_addr)

        if target_addr not in self._target_addresses:
            self._target_addresses[target_addr] = 1
        else:
            self._target_addresses[target_addr] += 1

        # process target identification
        target_id = find_value("380.ID.Target Identification", record)

        if target_id not in self._target_identifications:
            self._target_identifications[target_id] = 1
        else:
            self._target_identifications[target_id] += 1

        # process ages
        self._track_ages[tod].add(TrackAges(
            find_value("290.PSR.Age", record),
            find_value("290.SSR.Age", record),
            find_value("290.MDS.Age", record),
            find_value("290.MLT.Age", record),
            find_value("290.ADS.Age", record)))

    def finalize(self):

        for tod, ages in self._track_ages.items():
            for age in ages:
                if age.is_single_technology():
                    single_tech = age.get_single_technology()

                    self._num_single_technology_updates += 1

                    if single_tech not in self._num_single_technology_counts:
                        self._num_single_technology_counts[single_tech] = 1
                    else:
                        self._num_single_technology_counts[single_tech] += 1
                else:
                    self._num_multi_technology_updates += 1

        self._single_technology_ratio = self._num_single_technology_updates/self._num_records

        self._single_tech_track = self._single_technology_ratio > 0.9

    def print(self):
        if self._single_tech_track:
            print_warn('single tech utn {} track_num {} begin {} end {} duration {} records {}'.format(
                self._utn, self._track_num, time_str_from_seconds(self._first_tod),
                time_str_from_seconds(self._last_tod), time_str_from_seconds(self._last_tod - self._first_tod),
                self._num_records))
            print('\tmode 3/a codes: {}'.format(self._mode_a_codes))
            print('\ttarget addresses: {}'.format(self._target_addresses))
            print('\ttarget identifications: {}'.format(self._target_identifications))
            print_warn('ratio: {0:.2f}% counts {1}'.format(
                100*self._single_technology_ratio, self._num_single_technology_counts))
            print('\n')
        else:
            print('multi tech utn {0} track_num {1} records {2} target addresses {3} ratio {4:.2f}%'.format(
                self._utn, self._track_num, self._num_records, self._target_addresses, 100*self._single_technology_ratio))


global_first_tod = None
global_last_tod = None


class TrackStatisticsCalculator:
    def __init__(self):
        self.__num_records = 0

        self._rec_num_cnt = 0

        self._utn_cnt = 0

        self._tn2utn_map = {}  # type: Dict[int,int]  # track_num -> utn
        self._active_targets = {}  # type: Dict[int,TrackStatistic]  # utn -> statistic
        self._finalized_targets = {}  # type: Dict[int,TrackStatistic]  # utn -> statistic

        self._num_tracks = 0
        self._num_multi_tech_tracks = 0
        self._num_single_tech_tracks = 0

    @property
    def num_records(self):
        return self.__num_records

    def process_record(self, cat, record):

        self.__num_records += 1

        tod = find_value("070.Time Of Track Information", record)
        assert tod is not None

        global global_first_tod
        global global_last_tod

        if global_first_tod is None:
            global_first_tod = tod
            global_last_tod = tod

        if tod > global_last_tod:
            global_last_tod = tod

        track_num = find_value("040.Track Number", record)
        assert track_num is not None

        track_begin = find_value("080.TSB", record)
        assert track_begin is not None

        track_end = find_value("080.TSE", record)
        assert track_end is not None

        if track_num not in self._tn2utn_map:  # does not exist yet
            current_utn = self._utn_cnt  # create new utn
            self._utn_cnt += 1

            self._tn2utn_map[track_num] = current_utn  # put in mapping

            self._active_targets[current_utn] = TrackStatistic(current_utn, track_num)

        utn = self._tn2utn_map[track_num]
        assert utn in self._active_targets

        self._active_targets[utn].process_record(cat, record)

        if track_end == 1:
            self._finalized_targets[utn] = self._active_targets[utn]
            del self._active_targets[utn]
            del self._tn2utn_map[track_num]

    def finalize(self):
        for utn, statistic in self._active_targets.items():
            self._finalized_targets[utn] = statistic

        self._active_targets = {}

        self._num_tracks = len(self._finalized_targets)

        for utn, statistic in sorted(self._finalized_targets.items()):
            statistic.finalize()

            if statistic.single_tech_track:
                self._num_single_tech_tracks += 1
            else:
                self._num_multi_tech_tracks += 1

    def print(self):
        print('num records {}'.format(self.__num_records))

        print('recording begin {} end {} duration {}'.format(
            time_str_from_seconds(global_first_tod), time_str_from_seconds(global_last_tod),
            time_str_from_seconds(global_last_tod-global_first_tod)))

        for utn, statistic in sorted(self._finalized_targets.items()):
            statistic.print()

        print('num tracks {0} single tech {1} ({2:.2f}%)'.format(
            self._num_tracks, self._num_single_tech_tracks, 100*self._num_single_tech_tracks/self._num_tracks))

def main(argv):

    parser = argparse.ArgumentParser(description='ASTERIX CAT062 tracks supported only by one technology analysis')
    parser.add_argument('--framing', help='Framing True or False', required=True)

    args = parser.parse_args()

    assert args.framing is not None
    framing = args.framing

    print('framing {}'.format(framing))

    num_blocks = 0

    statistics_calc = TrackStatisticsCalculator()  # type: TrackStatisticsCalculator

    record_extractor = RecordExtractor(
        framing, statistics_calc.process_record, filter_tracks)  # type: RecordExtractor

    start_time = time.time()

    for line in sys.stdin:
        #print(line)

        json_data = json.loads(line)

        record_extractor.find_records (json_data)

        num_blocks += 1

        end_time = time.time()
        print('blocks {0} time {1:.2f}s unfiltered records {2} filtered {3} rate {4} rec/s'.format(
            num_blocks, end_time-start_time, statistics_calc.num_records, record_extractor.num_filtered,
            int(record_extractor.num_records/(end_time-start_time))))

    statistics_calc.finalize()
    statistics_calc.print()


if __name__ == "__main__":
    main(sys.argv[1:])