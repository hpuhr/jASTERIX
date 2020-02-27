#!/usr/bin/python

import sys
import json
import time
from typing import Dict
import argparse

from util.record_extractor import RecordExtractor
from util.common import find_value
import util.track

class TrackStatisticsCalculator:
    def __init__(self, mysql_wrapper):
        self.__num_records = 0

        self._rec_num_cnt = 0
        self._diff_cnt_sum = 0

        self._mysql_wrapper = mysql_wrapper  # can be None
        assert self._mysql_wrapper is not None

        self._check_getters = {}
        #self._check_getters["alt_baro_more_reliable"] = lambda record: find_value("080.MRH", record) # not set in db?
        self._check_getters["calc_alt_geo_ft"] = lambda record: find_value("130.Altitude", record)
        self._check_getters["calc_vertical_rate_ftm"] = lambda record: find_value("220.Rate of Climb/Descent", record)
        self._check_getters["callsign"] = lambda record: find_value("380.ID.Target Identification", record)
        #ds_id different
        #groundspeed_kt TODO
        #heading_deg TODO
        self._check_getters["tod"] = lambda record: float(find_value("070.Time Of Track Information", record))*128.0
        self._check_getters["track_num"] = lambda record: find_value("040.Track Number", record)
        self._check_getters["lm_alt_baro_ft"] = lambda record: find_value("340.MDC.Last Measured Mode C Code", record)
        self._check_getters["lm_alt_baro_g"] = lambda record: util.track.get_as_verif_flag("340.MDC.G", record, False)
        self._check_getters["lm_alt_baro_v"] = lambda record: util.track.get_as_verif_flag("340.MDC.V", record, True)

        self._check_counts = {}
        selected_variables = ','.join(self._check_getters.keys())

        for var_name, get_lambda in self._check_getters.items():
            self._check_counts[var_name] = [0, 0] # passed, failed

        self._mysql_wrapper.prepare_read(selected_variables, 'sd_track')

    @property
    def num_records(self):
        return self.__num_records

    @property
    def diff_cnt_sum(self):
        return self._diff_cnt_sum

    def process_record(self, cat, record):

        self.__num_records += 1

        row = self._mysql_wrapper.fetch_one()

        any_check_failed = False

        for var_name, get_lambda in self._check_getters.items():
            record_value = get_lambda(record)
            db_value = row[var_name]

            if record_value != db_value:  # failed
                print('record {} variable {} difference value record {} db {}'.format(
                    self.__num_records, var_name, record_value, db_value))
                any_check_failed = True

                self._check_counts[var_name][1] += 1
            else:  # passed
                self._check_counts[var_name][0] += 1

        if any_check_failed:
            self._diff_cnt_sum += 1

    def print(self):
        print('num cat062 records {}'.format(self.__num_records))

        print('rec num cnt {}'.format(self._rec_num_cnt))

        print('diff cnt sum {}'.format(self._diff_cnt_sum))

        for var_name, counts in self._check_counts.items():
            print('variable {} checks passed {} failed {}'.format(var_name, counts[0], counts[1]))


# filter functions return True if record should be skipped
def filter_psr_tracks(cat, record):
    if cat != 62:
        return True


def main(argv):

    parser = argparse.ArgumentParser(description='ASTERIX CAT062 mapping to sd_track analysis')
    parser.add_argument('--framing', help='Framing True or False', required=True)
    parser.add_argument('--mysql_info', help='MySQL server info as CSV: host;user;passwd;database', required=True)

    args = parser.parse_args()

    assert args.framing is not None
    framing = args.framing

    print('framing {}'.format(framing))
    print('mysql_info {}'.format(args.mysql_info))

    mysql_wrapper = None

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
            int(record_extractor.num_records/(end_time-start_time)), statistics_calc.diff_cnt_sum))

    statistics_calc.print()


if __name__ == "__main__":
    main(sys.argv[1:])