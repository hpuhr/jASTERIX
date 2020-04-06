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

    return False

global_first_tod = None
global_last_tod = None

short_track_duration = None  # 30, 60, None disables check

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

        self._dubious_track = False
        self._dubious_track_reasons = ""

    @property
    def dubious_track(self):
        return self._dubious_track

    def process_record(self, cat, record):

        self._num_records += 1

        # process time
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

        # process track number
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

    def finalize(self):

        if len(self._mode_a_codes) > 3:
            self._dubious_track = True
            self._dubious_track_reasons += "too many mode 3/a codes\n"

        if (len(self._target_addresses) == 2 and None not in self._target_addresses) \
                or len(self._target_addresses) > 2:
            self._dubious_track = True
            self._dubious_track_reasons += "too many target_addresses\n"

        if (len(self._target_identifications) == 2 and None not in self._target_identifications) \
                or len(self._target_identifications) > 2:
            self._dubious_track = True
            self._dubious_track_reasons += "too many target ids\n"

        if short_track_duration is not None:

            global global_first_tod
            global global_last_tod

            if self._last_tod - self._first_tod < short_track_duration \
                    and self._last_tod > global_first_tod+2*short_track_duration \
                    and self._first_tod < global_last_tod-2*short_track_duration:
                self._dubious_track = True
                self._dubious_track_reasons += "short track\n"

    def print(self):

        if self._dubious_track:
            print_warn('dubious utn {} track_num {} begin {} end {} duration {} records {}'.format(
                self._utn, self._track_num, time_str_from_seconds(self._first_tod),
                time_str_from_seconds(self._last_tod), time_str_from_seconds(self._last_tod-self._first_tod),
                self._num_records))
            print('\tmode 3/a codes: {}'.format(self._mode_a_codes))
            print('\ttarget addresses: {}'.format(self._target_addresses))
            print('\ttarget identifications: {}'.format(self._target_identifications))
            print_warn('reasons: {}'.format(self._dubious_track_reasons))
            print('\n')
        # else:
        #     print('not dubious utn {} track_num {} begin {} end {} duration {} records {}'.format(
        #         self._utn, self._track_num, time_str_from_seconds(self._first_tod),
        #         time_str_from_seconds(self._last_tod), time_str_from_seconds(self._last_tod-self._first_tod),
        #         self._num_records))

class TrackStatisticsCalculator:
    def __init__(self):
        self.__num_records = 0

        self._rec_num_cnt = 0

        self._utn_cnt = 0

        self._tn2utn_map = {}  # type: Dict[int,int]  # track_num -> utn
        self._active_targets = {}  # type: Dict[int,TrackStatistic]  # utn -> statistic
        self._finalized_targets = {}  # type: Dict[int,TrackStatistic]  # utn -> statistic

        self._track_cnt = 0
        self._track_dubious_cnt = 0

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

        self._track_cnt = len(self._finalized_targets)

        for utn, statistic in sorted(self._finalized_targets.items()):
            statistic.finalize()

            if statistic.dubious_track:
                self._track_dubious_cnt += 1

    def print(self):
        print('num records {}'.format(self.__num_records))
        print('recording begin {} end {} duration {}'.format(
            time_str_from_seconds(global_first_tod), time_str_from_seconds(global_last_tod),
            time_str_from_seconds(global_last_tod-global_first_tod)))

        for utn, statistic in sorted(self._finalized_targets.items()):
            statistic.print()

        print('num tracks {0} dubious {1} ({2:.2f}%)'.format(
            self._track_cnt, self._track_dubious_cnt, 100*self._track_dubious_cnt/self._track_cnt))

def main(argv):

    parser = argparse.ArgumentParser(description='ASTERIX CAT062 dubious tracks')
    parser.add_argument('--framing', help='Framing True or False', required=True)
    parser.add_argument('--short_track_duration', help='Short track duration in seconds, disabled when not set ',
                        required=False)

    args = parser.parse_args()

    assert args.framing is not None
    framing = args.framing

    global short_track_duration
    if args.short_track_duration is not None:
        short_track_duration = float(args.short_track_duration)

    print('framing {}'.format(framing))
    print('short track duration {}'.format(short_track_duration))

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