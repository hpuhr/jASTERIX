#!/usr/bin/python

import sys
import json
import time
from typing import Dict
import argparse

import mysql.connector

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
    def __init__(self):
        self.__num_records = 0
        self.__num_psr_records = 0

        self._rec_num_cnt = 0
        self._diff_det_cnt_sum = 0
        self._diff_det_cnt = {}  # type: Dict[(int,int),int]  # (sdl_det, db_det) -> cnt

        self._mydb = mysql.connector.connect(
            host="localhost",
            user="sassc",
            passwd="sassc",
            database="PR_v8_24102019"
        )

        self._mycursor = self._mydb.cursor()

        #print ('sd_track columns')
        #self._mycursor.execute("SHOW COLUMNS FROM sd_track")
        #for x in self._mycursor:
        #    print(x)

        self._mycursor.execute("SELECT count(*) FROM sd_track")
        self.__num_db_records = self._mycursor.fetchone()[0]

        #self._mycursor.execute("SELECT count(*) FROM sd_track WHERE detection_type=1 AND multiple_sources=\"N\"")
        #print ('tmp {}'.format(self._mycursor.fetchone()[0]))

        #self.print_distinct("report_type", "sd_track")
        self.print_distinct("detection_type", "sd_track")
        #self.print_distinct("multiple_sources", "sd_track")

        self._mycursor.execute("SELECT rec_num, tod, track_num, detection_type FROM sd_track") #  WHERE track_num=4227 LIMIT 0,10000

    @property
    def num_records(self):
        return self.__num_records

    @property
    def num_psr_records(self):
        return self.__num_psr_records

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

        sdl_detection_type = self.get_detection_type(record)

        if sdl_detection_type == 1:
            self.__num_psr_records += 1

        tod = find_value("070.Time Of Track Information", record)
        assert tod is not None

        track_num = find_value("040.Track Number", record)
        assert track_num is not None

        db_rec_num, db_tod, db_track_num, db_detection_type = self._mycursor.fetchone()
        db_tod /= 128.0

        if tod != db_tod or track_num != db_track_num:
            print('cnt {} json tod {} db {} tn {} db {}'.format(
                self.__num_records, tod, db_tod, track_num, db_track_num))

        if sdl_detection_type != db_detection_type:

            self._diff_det_cnt_sum += 1

            key = (sdl_detection_type, db_detection_type)

            print('\tdifference det {} verif {} ages PSR {} SSR {} MDS {} MLT {} ADS {} MA {} MF {}'.format(
                sdl_detection_type, db_detection_type,
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
            #    sdl_detection_type, detection_type_strings[sdl_detection_type],
            #    db_detection_type, detection_type_strings[db_detection_type],
            #    json.dumps(record, indent=4)))

            if key not in self._diff_det_cnt:
                print('rec_num {} det {} ({}) v7_det {} ({}) json \'{}\''.format(
                    db_rec_num,
                    sdl_detection_type, detection_type_strings[sdl_detection_type],
                    db_detection_type, detection_type_strings[db_detection_type],
                    json.dumps(record, indent=4)))
                self._diff_det_cnt[key] = 1
            else:
                #print('rec_num {} sdl_det {} v7_det {}'.format(
                #    db_rec_num, sdl_detection_type, db_detection_type))
                self._diff_det_cnt[key] += 1

        assert tod == db_tod
        assert track_num == db_track_num

    def print(self):
        print('num cat062 records {}'.format(self.__num_records))
        print('num psr track update records {}'.format(self.__num_psr_records))

        print('rec num cnt {}'.format(self._rec_num_cnt))

        print('diff det cnt sum {}'.format(self._diff_det_cnt_sum))

        for key, cnt in self._diff_det_cnt.items():
            sdl_det_type, db_det_type = key
            print('diff det {} {}, {} ({}) cnt {}'.format(
                sdl_det_type, detection_type_strings[sdl_det_type],
                db_det_type, detection_type_strings[db_det_type], cnt))

    def print_distinct(self, variable, table):

        self._mycursor.execute("SELECT {},COUNT(*) from {} GROUP BY {}".format(variable, table, variable))

        print('table {} variable {}, count:'.format(table, variable))
        for x in self._mycursor:
            if variable == "detection_type":
                value, cnt = x
                print('({}, {})'.format(detection_type_strings[value], cnt))
            else:
                print (x)


# filter functions return True if record should be skipped
def filter_psr_tracks(cat, record):
    if cat != 62:
        return True

def main(argv):

    parser = argparse.ArgumentParser(description='ASTERIX data item analysis')
    parser.add_argument('--framing', help='Framing True or False', required=True)

    args = parser.parse_args()

    assert args.framing is not None
    framing = args.framing

    print('framing {}'.format(framing))

    num_blocks = 0

    statistics_calc = TrackStatisticsCalculator()  # type: TrackStatisticsCalculator

    record_extractor = RecordExtractor(framing, statistics_calc.process_record, filter_psr_tracks)  # type: RecordExtractor

    start_time = time.time()

    for line in sys.stdin:
        #print(line)

        json_data = json.loads(line)

        record_extractor.find_records (json_data)

        num_blocks += 1

        end_time = time.time()
        print('blocks {0} time {1:.2f}s unfiltered records {2} filtered {3} rate {4} rec/s diff det {5}'.format(
            num_blocks, end_time-start_time, statistics_calc.num_records,
            record_extractor.num_filtered,
            int((record_extractor.num_records)/(end_time-start_time)), statistics_calc.diff_det_cnt_sum))

    statistics_calc.print()


if __name__ == "__main__":
    main(sys.argv[1:])