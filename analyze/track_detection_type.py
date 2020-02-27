#!/usr/bin/python

import sys
import json
import time
from typing import Dict
import argparse

from util.record_extractor import RecordExtractor
from util.common import find_value

detection_type_strings = {
    0: "no detection",
    1: "psr only",
    2: "ssr only",
    3: "combined",
    4: "mode s all-call",
    5: "mode s role-call",
    6: "mode s all-call combined",
    7: "mode s role-call combined"
}

class TrackStatisticsCalculator:
    def __init__(self, mysql_wrapper):
        self.__num_records = 0

        self._rec_num_cnt = 0
        self._diff_det_cnt_sum = 0
        self._diff_det_cnt = {}  # type: Dict[(int,int),int]  # (calc_det, db_det) -> cnt

        self._det_cnt = {}  # type: Dict[int,int]  # det type -> cnt

        self._mysql_wrapper = mysql_wrapper  # can be None
        if self._mysql_wrapper is not None:
            self._mysql_wrapper.prepare_read('rec_num, tod, track_num, detection_type', 'sd_track')

    @property
    def num_records(self):
        return self.__num_records

    @property
    def diff_det_cnt_sum(self):
        return self._diff_det_cnt_sum

    def get_detection_type(self, record):

        if find_value("080.CST", record) == 1:
            return 0  # no detection

        psr_updated = find_value("080.PSR", record) == 0
        ssr_updated = find_value("080.SSR", record) == 0
        mds_updated = find_value("080.MDS", record) == 0
        ads_updated = find_value("080.ADS", record) == 0

        if not mds_updated:
            if psr_updated and not ssr_updated:

                if find_value("290.MLT.Age", record) is not None:
                    # age not 63.75
                    mlat_age = find_value("290.MLT.Age", record)

                    if mlat_age <= 12.0:
                        return 3

                return 1  # single psr, no mode-s

            if not psr_updated and ssr_updated:
                return 2  # single ssr, no mode-s

            if psr_updated and ssr_updated:
                return 3  # cmb, no mode-s

        else:
            if not psr_updated:
                return 5  # ssr, mode-s
            else:
                return 7  # cmb, mode-s

        if find_value("380.ADR.Target Address", record) is not None:
            return 5

        if find_value("060.Mode-3/A reply", record) is not None \
                or find_value("136.Measured Flight Level", record) is not None:
            return 2

        return 0  # unknown

    def process_record(self, cat, record):

        self.__num_records += 1

        calc_detection_type = self.get_detection_type(record)

        if calc_detection_type not in self._det_cnt:
            self._det_cnt[calc_detection_type] = 0

        self._det_cnt[calc_detection_type] += 1

        tod = find_value("070.Time Of Track Information", record)
        assert tod is not None

        track_num = find_value("040.Track Number", record)
        assert track_num is not None

        if self._mysql_wrapper is not None:
            db_rec_num, db_tod, db_track_num, db_detection_type = self._mysql_wrapper.fetch_one()
            db_tod /= 128.0

            if tod != db_tod or track_num != db_track_num:
                print('cnt {} json tod {} db {} tn {} db {}'.format(
                    self.__num_records, tod, db_tod, track_num, db_track_num))

            if calc_detection_type != db_detection_type:

                self._diff_det_cnt_sum += 1

                key = (calc_detection_type, db_detection_type)

                print('\tdifference det {} verif {} ages PSR {} SSR {} MDS {} MLT {} ADS {} MA {} MF {}'.format(
                    calc_detection_type, db_detection_type,
                    find_value("290.PSR.Age", record),
                    find_value("290.SSR.Age", record),
                    find_value("290.MDS.Age", record),
                    find_value("290.MLT.Age", record),
                    find_value("290.ADS.Age", record),
                    find_value("295.MDA.Age", record),
                    find_value("295.MFL.Age", record)
                ))

                #print('rec_num {} det {} ({})  v7_det {} ({}) json \'{}\''.format(
                #    db_rec_num,
                #    calc_detection_type, detection_type_strings[calc_detection_type],
                #    db_detection_type, detection_type_strings[db_detection_type],
                #    json.dumps(record, indent=4)))

                if key not in self._diff_det_cnt:
                    print('rec_num {} det {} ({}) v7_det {} ({}) json \'{}\''.format(
                        db_rec_num,
                        calc_detection_type, detection_type_strings[calc_detection_type],
                        db_detection_type, detection_type_strings[db_detection_type],
                        json.dumps(record, indent=4)))
                    self._diff_det_cnt[key] = 1
                else:
                    #print('rec_num {} det {} v7_det {}'.format(
                    #    db_rec_num, calc_detection_type, db_detection_type))
                    self._diff_det_cnt[key] += 1

            assert tod == db_tod
            assert track_num == db_track_num
        else:
            pass  # nothing implemented yet

    def print(self):
        print('num cat062 records {}'.format(self.__num_records))

        for det_type, cnt in self._det_cnt.items():
            print('detection type {} ({}) count {}'.format(det_type, detection_type_strings[det_type], cnt))

        print('rec num cnt {}'.format(self._rec_num_cnt))

        print('diff det cnt sum {}'.format(self._diff_det_cnt_sum))

        for key, cnt in self._diff_det_cnt.items():
            calc_det_type, db_det_type = key
            print('diff det {} {}, {} ({}) cnt {}'.format(
                calc_det_type, detection_type_strings[calc_det_type],
                db_det_type, detection_type_strings[db_det_type], cnt))


# filter functions return True if record should be skipped
def filter_psr_tracks(cat, record):
    if cat != 62:
        return True


def main(argv):

    parser = argparse.ArgumentParser(description='ASTERIX data item analysis')
    parser.add_argument('--framing', help='Framing True or False', required=True)
    parser.add_argument('--mysql_info', help='MySQL server info as CSV: host;user;passwd;database', required=False)

    args = parser.parse_args()

    assert args.framing is not None
    framing = args.framing

    print('framing {}'.format(framing))
    print('mysql_info {}'.format(args.mysql_info))

    mysql_wrapper = None

    if args.mysql_info is not None:
        # e.g. localhost;sassc;sassc;PR_v8_24102019"
        assert len(args.mysql_info.split(';')) == 4
        host, user, passwd, database = args.mysql_info.split(';')

        from util.mysql_wrapper import MySQLWrapper

        mysql_wrapper = MySQLWrapper(host=host, user=user, passwd=passwd, database=database)

    num_blocks = 0

    statistics_calc = TrackStatisticsCalculator(mysql_wrapper)  # type: TrackStatisticsCalculator

    record_extractor = RecordExtractor(
        framing, statistics_calc.process_record, filter_psr_tracks)  # type: RecordExtractor

    start_time = time.time()

    for line in sys.stdin:
        #print(line)

        json_data = json.loads(line)

        record_extractor.find_records (json_data)

        num_blocks += 1

        end_time = time.time()
        print('blocks {0} time {1:.2f}s unfiltered records {2} filtered {3} rate {4} rec/s diff det {5}'.format(
            num_blocks, end_time-start_time, statistics_calc.num_records, record_extractor.num_filtered,
            int(record_extractor.num_records/(end_time-start_time)), statistics_calc.diff_det_cnt_sum))

    statistics_calc.print()


if __name__ == "__main__":
    main(sys.argv[1:])