#!/usr/bin/python

import sys
import json
import time
from typing import Dict
import argparse

from util.record_extractor import RecordExtractor
from util.common import *
from util.track import *

class TrackStatisticsCalculator:
    def __init__(self, mysql_wrapper):
        self.__num_records = 0

        self._rec_num_cnt = 0
        self._diff_cnt_sum = 0

        self._mysql_wrapper = mysql_wrapper  # can be None
        assert self._mysql_wrapper is not None

        self._check_accuracy = {}

        self._check_getters = {}
        self._check_getters["alt_baro_ft"] = lambda record: find_value("135.Calculated Track Barometric Altitude", record)
        self._check_getters["alt_baro_more_reliable"] = lambda record: invert(find_value("080.MRH", record)) # not set in db?, invert

        self._check_getters["alt_geo_ft"] = lambda record: find_value("130.Altitude", record)
        self._check_getters["calc_alt_geo_ft"] = lambda record: find_value("130.Altitude", record)
        self._check_getters["calc_vertical_rate_ftm"] = lambda record: find_value("220.Rate of Climb/Descent", record)
        self._check_getters["callsign"] = lambda record: find_value("380.ID.Target Identification", record)
        self._check_getters["cat"] = lambda record: 62
        self._check_getters["civil_emergency"] = lambda record: find_value("080.EMS", record)
        self._check_getters["detection_type"] = lambda record: get_detection_type(record)
        #ds_id different
        self._check_getters["fpl_callsign"] = lambda record: find_value("390.CSN.Callsign", record)
        self._check_getters["groundspeed_kt"] = lambda record: get_speed(record)
        self._check_accuracy["groundspeed_kt"] = 10e-2  # weird that so different, depends on reference point, not system center
        self._check_getters["heading_deg"] = lambda record: get_track_angle(record)
        self._check_accuracy["heading_deg"] = 10e-2  # depends on reference point, not system center

        self._check_getters["lm_alt_baro_ft"] = lambda record: find_value("340.MDC.Last Measured Mode C Code", record)
        self._check_getters["lm_alt_baro_g"] = lambda record: get_as_verif_flag("340.MDC.G", record, False)
        self._check_getters["lm_alt_baro_v"] = lambda record: get_as_verif_flag("340.MDC.V", record, True)
        self._check_getters["lm_alt_geo_ft"] = lambda record: find_value("380.GAL.Geometric Altitude", record)

        self._check_getters["lm_mode3a_code"] = lambda record: oct_to_dec(find_value("340.MDA.Mode-3/A reply", record))
        self._check_getters["lm_mode3a_g"] = lambda record: get_as_verif_flag("340.MDA.G", record, False)
        self._check_getters["lm_mode3a_s"] = lambda record: get_as_verif_flag("340.MDA.L", record, False)
        self._check_getters["lm_mode3a_v"] = lambda record: get_as_verif_flag("340.MDA.V", record, True)

        self._check_getters["measured_alt_baro_age_s"] = lambda record: find_value("295.MDA.Age", record)
        self._check_getters["measured_alt_baro_ft"] = lambda record: find_value("136.Measured Flight Level", record) # not feet but FL
        self._check_getters["measured_mode3a_age_s"] = lambda record: find_value("295.MDA.Age", record)

        self._check_getters["mil_emergency"] = lambda record: get_as_verif_flag("080.ME", record, False)

        self._check_getters["mode3a_code"] = lambda record: oct_to_dec(find_value("060.Mode-3/A reply", record))
        # mode3a_g not in cat?
        # mode3a_v not in cat?

        self._check_getters["modec_age"] = lambda record: find_value("295.MFL.Age", record)
        self._check_getters["modec_code_ft"] = lambda record: multiply(find_value("136.Measured Flight Level", record), 100.0)

        self._check_getters["mof_long"] = lambda record: find_value("200.LONG", record)
        self._check_getters["mof_trans"] = lambda record: find_value("200.TRANS", record)
        self._check_getters["mof_vert"] = lambda record: find_value("200.VERT", record)

        self._check_getters["multiple_sources"] = lambda record: get_as_verif_flag("080.MON", record, True)

        self._check_getters["pos_local_x_nm"] = lambda record: multiply(find_value("100.X", record), M2NM)
        self._check_accuracy["pos_local_x_nm"] = 10e-9
        self._check_getters["pos_local_y_nm"] = lambda record: multiply(find_value("100.Y", record), M2NM)
        self._check_accuracy["pos_local_y_nm"] = 10e-9

        self._check_getters["pos_sys_x_nm"] = lambda record: multiply(find_value("100.X", record), M2NM)
        self._check_accuracy["pos_sys_x_nm"] = 10e-9
        self._check_getters["pos_sys_y_nm"] = lambda record: multiply(find_value("100.Y", record), M2NM)
        self._check_accuracy["pos_sys_y_nm"] = 10e-9

        self._check_getters["pos_lat_deg"] = lambda record: find_value("105.Latitude", record)
        self._check_accuracy["pos_lat_deg"] = 10e-9
        self._check_getters["pos_long_deg"] = lambda record: find_value("105.Longitude", record)
        self._check_accuracy["pos_long_deg"] = 10e-9

        self._check_getters["report_type"] = lambda record: 2
        self._check_getters["sac"] = lambda record: find_value("010.SAC", record)
        self._check_getters["selected_alt_baro_ft"] = lambda record: find_value("380.SAL.Selected Altitude", record)
        self._check_accuracy["selected_alt_baro_ft"] = 10e-9

        self._check_getters["sic"] = lambda record: find_value("010.SIC", record)

        self._check_getters["simulated_target"] = lambda record: get_as_verif_flag("080.SIM", record, False)
        self._check_getters["spi"] = lambda record: get_as_verif_flag("080.SPI", record, False)
        self._check_getters["target_addr"] = lambda record: find_value("380.ADR.Target Address", record)
        self._check_getters["tod"] = lambda record: multiply(find_value("070.Time Of Track Information", record), 128.0)

        self._check_getters["tracked_alt_baro_ft"] = lambda record: find_value("135.Calculated Track Barometric Altitude", record)
        self._check_getters["tracked_mode3a_code"] = lambda record: oct_to_dec(find_value("60.Mode 3/A reply", record))

        self._check_getters["track_amalgamated"] = lambda record: get_as_verif_flag("080.AMA", record, False)
        self._check_getters["track_coasted"] = lambda record: get_as_verif_flag("080.CST", record, False)
        self._check_getters["track_confirmed"] = lambda record: get_as_verif_flag("080.CNF", record, True)
        self._check_getters["track_created"] = lambda record: get_as_verif_flag("080.TSB", record, False)
        self._check_getters["track_end"] = lambda record: get_as_verif_flag("080.TSE", record, False)

        self._check_getters["track_num"] = lambda record: find_value("040.Track Number", record)

        self._check_getters["track_psr_age"] = lambda record: find_value("290.PSR.Age", record)
        self._check_getters["turnrate_degps"] = lambda record: find_value("380.TAR.Track Angle Rate", record)

        self._check_counts = {}
        self._check_differences = {}

        selected_variables = 'rec_num,'+','.join(self._check_getters.keys())

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

        rec_num = row["rec_num"]

        for var_name, get_lambda in self._check_getters.items():
            record_value = get_lambda(record)
            db_value = row[var_name]

            if record_value is not None and db_value is not None and var_name in self._check_accuracy:
                check = math.fabs(record_value-db_value) < self._check_accuracy[var_name]
            else:
                check = record_value == db_value

            if not check:  # failed
                self._check_counts[var_name][1] += 1

                if var_name not in self._check_differences:
                    self._check_differences[var_name] = []

                if len(self._check_differences[var_name]) < 10:
                    self._check_differences[var_name].append('value record \'{}\' db \'{}\''.format(record_value, db_value))

                any_check_failed = True

            else:  # passed
                self._check_counts[var_name][0] += 1

        if any_check_failed:
            self._diff_cnt_sum += 1

    def print(self):
        print('num cat062 records {}'.format(self.__num_records))

        print('rec num cnt {}'.format(self._rec_num_cnt))

        print('diff cnt sum {}'.format(self._diff_cnt_sum))

        for var_name, counts in self._check_counts.items():
            count_sum = counts[0] + counts[1]
            if count_sum:
                if counts[1]:  # some have failed
                    print('variable \'{0}\': checks passed {1} ({2:.3f}%) failed {3} ({4:.3f}%)'.format(
                        var_name, counts[0], 100.0*counts[0]/count_sum, counts[1], 100.0*counts[1]/count_sum))

                    assert var_name in self._check_differences

                    for msg in self._check_differences[var_name]:
                        print('\t'+msg)
                else:
                    print('variable \'{}\': {} checks passed'.format(var_name, counts[0]))

            else:
                print('variable \'{}\': no data')


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
        print('blocks {0} time {1:.2f}s unfiltered records {2} filtered {3} rate {4} rec/s diff {5}'.format(
            num_blocks, end_time-start_time, statistics_calc.num_records, record_extractor.num_filtered,
            int(record_extractor.num_records/(end_time-start_time)), statistics_calc.diff_cnt_sum))

    statistics_calc.print()


if __name__ == "__main__":
    main(sys.argv[1:])