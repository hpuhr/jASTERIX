#!/usr/bin/python

import sys
import json
import time
from typing import Dict
import argparse

from util.record_extractor import RecordExtractor
from util.common import *
from util.track import *
from util.mlat import *


class TrackStatisticsCalculator:
    def __init__(self, mysql_wrapper):
        self.__num_records = 0

        self._rec_num_cnt = 0
        self._diff_cnt_sum = 0

        self._mysql_wrapper = mysql_wrapper  # can be None
        assert self._mysql_wrapper is not None

        self._check_accuracy = {}

        # '020' count 3868
        # '020.010' count 3868 (100%)
        # '020.010.SAC' count 3868 (100%) min '6' max '6'
        # '020.010.SIC' count 3868 (100%) min '35' max '35'
        # '020.020' count 3868 (100%)
        # '020.020.CHN' count 3868 (100%) min '0' max '0'
        # '020.020.CRT' count 3868 (100%) min '0' max '0'
        # '020.020.DME' count 3868 (100%) min '0' max '0'
        # '020.020.FX' count 3868 (100%) min '1' max '1'
        # '020.020.FX2' count 3868 (100%) min '0' max '0'
        # '020.020.GBS' count 3868 (100%) min '0' max '0'
        # '020.020.HF' count 3868 (100%) min '0' max '0'
        # '020.020.MS' count 3868 (100%) min '1' max '1'
        # '020.020.OT' count 3868 (100%) min '0' max '0'
        # '020.020.RAB' count 3868 (100%) min '0' max '0'
        # '020.020.SIM' count 3868 (100%) min '0' max '0'
        # '020.020.SPI' count 3868 (100%) min '0' max '0'
        # '020.020.SSR' count 3868 (100%) min '0' max '0'
        # '020.020.TST' count 3868 (100%) min '0' max '0'
        # '020.020.UAT' count 3868 (100%) min '0' max '0'
        # '020.020.VDL4' count 3868 (100%) min '0' max '0'
        # '020.030' count 14 (0%)
        # '020.041' count 3868 (100%)
        # '020.041.Latitude' count 3868 (100%) min '50.58215378051' max '50.70448378615'
        # '020.041.Longitude' count 3868 (100%) min '5.31040112012' max '5.55753948882'
        # '020.042' count 3868 (100%)
        # '020.042.X' count 3868 (100%) min '-9375.5' max '8127.0'
        # '020.042.Y' count 3868 (100%) min '-6035.0' max '7590.0'
        # '020.070' count 3859 (99%)
        # '020.070.G' count 3859 (100%) min '0' max '0'
        # '020.070.L' count 3859 (100%) min '0' max '1'
        # '020.070.Mode-3/A code' count 3859 (100%) min '102' max '7167'
        # '020.070.V' count 3859 (100%) min '0' max '0'
        # '020.090' count 3672 (94%)
        # '020.090.Flight Level' count 3672 (100%) min '3.5' max '380.0'
        # '020.090.G' count 3672 (100%) min '0' max '0'
        # '020.090.V' count 3672 (100%) min '0' max '0'
        # '020.105' count 192 (4%)
        # '020.105.Geometric Height' count 192 (100%) min '750.0' max '762.5'
        # '020.110' count 192 (4%)
        # '020.110.Measured Height' count 192 (100%) min '0.0' max '0.0'
        # '020.140' count 3868 (100%)
        # '020.140.Time of Day' count 3868 (100%) min '7302.84375' max '10801.4375'
        # '020.161' count 3868 (100%)
        # '020.161.Track Number' count 3868 (100%) min '153' max '4001'
        # '020.170' count 3868 (100%)
        # '020.170.CDM' count 3868 (100%) min '3' max '3'
        # '020.170.CNF' count 3868 (100%) min '0' max '1'
        # '020.170.CST' count 3868 (100%) min '0' max '0'
        # '020.170.FX' count 3868 (100%) min '0' max '0'
        # '020.170.MAH' count 3868 (100%) min '0' max '0'
        # '020.170.STH' count 3868 (100%) min '1' max '1'
        # '020.170.TRE' count 3868 (100%) min '0' max '1'
        # '020.202' count 3868 (100%)
        # '020.202.Vx' count 3868 (100%) min '-258.75' max '138.0'
        # '020.202.Vy' count 3868 (100%) min '-174.75' max '222.25'
        # '020.210' count 3868 (100%)
        # '020.210.Ax' count 3868 (100%) min '-2.75' max '2.0'
        # '020.210.Ay' count 3868 (100%) min '-1.75' max '1.25'
        # '020.220' count 3868 (100%)
        # '020.220.Target Address' count 3868 (100%) min '4220914' max '11375362'
        # '020.230' count 3868 (100%)
        # '020.230.AIC' count 3868 (100%) min '0' max '1'
        # '020.230.ARC' count 3868 (100%) min '0' max '1'
        # '020.230.B1A' count 3868 (100%) min '0' max '1'
        # '020.230.B1B' count 3868 (100%) min '0' max '5'
        # '020.230.COM' count 3868 (100%) min '0' max '1'
        # '020.230.MSSC' count 3868 (100%) min '0' max '1'
        # '020.230.STAT' count 3868 (100%) min '0' max '1'
        # '020.245' count 3103 (80%)
        # '020.245.STI' count 3103 (100%) min '2' max '2'
        # '020.245.Target Identification' count 3103 (100%) min 'FDX6020 ' max 'TAY987A '
        # '020.250' count 3854 (99%)
        # '020.250.REP' count 3854 (100%) min '1' max '5'
        # '020.400' count 3868 (100%)
        # '020.400.REP' count 3868 (100%) min '16' max '16'
        # '020.REF' count 3868 (100%)
        # '020.REF.DA' count 3868 (100%)
        # '020.REF.DA.FL' count 3868 (100%)
        # '020.REF.DA.FL.FL' count 3868 (100%) min '0.0' max '25.5'
        # '020.REF.GVV' count 3868 (100%)
        # '020.REF.GVV.Ground Speed' count 3868 (100%) min '0.0' max '0.178222638'
        # '020.REF.GVV.RE' count 3868 (100%) min '0' max '0'
        # '020.REF.GVV.Track Angle' count 3868 (100%) min '0.0' max '332.03979477076'
        # '020.REF.PA' count 3868 (100%)
        # '020.REF.PA.DOP' count 3868 (100%)
        # '020.REF.PA.DOP.DOP-x' count 3868 (100%) min '0.0' max '948.75'
        # '020.REF.PA.DOP.DOP-xy' count 3868 (100%) min '-1269.25' max '14.0'
        # '020.REF.PA.DOP.DOP-y' count 3868 (100%) min '0.0' max '1927.25'
        # '020.REF.PA.SDC' count 3868 (100%)
        # '020.REF.PA.SDC.COV-XY (Covariance Component)' count 3868 (100%) min '-71.25' max '7.25'
        # '020.REF.PA.SDC.SDC (X-Component)' count 3868 (100%) min '0.0' max '61.5'
        # '020.REF.PA.SDC.SDC (Y-Component)' count 3868 (100%) min '0.0' max '87.75'
        # '020.REF.PA.SDH' count 192 (4%)
        # '020.REF.PA.SDH.SDH' count 192 (100%) min '170.0' max '7177.0'
        # '020.REF.PA.SDW' count 3868 (100%)
        # '020.REF.PA.SDW.COV-WGS (Lat/Long Covariance Component)' count 3868 (100%) min '-0.17578098688' max '8.583056e-05'
        # '020.REF.PA.SDW.SDW (Latitude Component)' count 3868 (100%) min '5.36441e-06' max '0.17578098688'
        # '020.REF.PA.SDW.SDW (Longitude Component)' count 3868 (100%) min '5.36441e-06' max '0.17578098688'
        # '020.REF.TRT' count 3868 (100%)
        # '020.REF.TRT.Time Of ASTERIX Report Transmission' count 3868 (100%) min '7303.125' max '10801.7265625'
        # '020.index' count 3868 (100%) min '897842' max '43995060'
        # '020.length' count 3868 (100%) min '89' max '142'

        self._check_getters = {}
        self._check_getters["acas_aircraft_identification"] = lambda record: get_as_verif_flag("230.AIC", record, True) # why inverted?
        self._check_getters["acas_alt_capability_ft"] = lambda record: get_alt_cap_as_double(record)
        self._check_getters["acas_bds_1_a"] = lambda record: find_value("230.B1A", record)
        self._check_getters["acas_bds_1_b"] = lambda record: find_value("230.B1B", record)
        self._check_getters["acas_comm_capability"] = lambda record: find_value("230.COM", record)
        self._check_getters["acas_flight_status"] = lambda record: find_value("230.STAT", record)
        self._check_getters["acas_modes_service"] = lambda record: get_as_verif_flag("230.MSSC", record, False)

        self._check_getters["accel_ax_ms2"] = lambda record: find_value("210.Ax", record)
        self._check_accuracy["accel_ax_ms2"] = 10e-2
        self._check_getters["accel_ay_ms2"] = lambda record: find_value("210.Ay", record)
        self._check_accuracy["accel_ay_ms2"] = 10e-2

        self._check_getters["alt_baro_ft"] = lambda record: multiply(find_value("090.Flight Level", record), 100.0)
        self._check_getters["alt_baro_g"] = lambda record: get_as_verif_flag("090.G", record, False)
        self._check_getters["alt_baro_v"] = lambda record: get_as_verif_flag("090.V", record, True)

        self._check_getters["alt_geo_ft"] = lambda record: find_value("105.Geometric Height", record)
        self._check_getters["calc_alt_geo_ft"] = lambda record: find_value("110.Measured Height", record)

        self._check_getters["callsign"] = lambda record: find_value("245.Target Identification", record)
        self._check_getters["cat"] = lambda record: 20

        self._check_getters["flight_level_ft"] = lambda record: multiply(find_value("090.Flight Level", record), 100.0)
        self._check_getters["flight_level_g"] = lambda record: get_as_verif_flag("090.G", record, False)
        self._check_getters["flight_level_v"] = lambda record: get_as_verif_flag("090.V", record, True)

        self._check_getters["geo_alt_std_dev_m"] = lambda record: multiply(find_value("REF.PA.SDH.SDH", record), 1.0)
        self._check_getters["geo_alt_std_dev_ft"] = lambda record: multiply(find_value("REF.PA.SDH.SDH", record), M2FT)
        self._check_getters["ground_bit"] = lambda record: get_as_verif_flag("020.GBS", record, False)
        self._check_getters["height_3d_ft"] = lambda record: find_value("105.Geometric Height", record)

        self._check_getters["mode3a_code"] = lambda record: oct_to_dec(find_value("070.Mode-3/A code", record))
        self._check_getters["mode3a_g"] = lambda record: get_as_verif_flag("070.G", record, False)
        self._check_getters["mode3a_v"] = lambda record: get_as_verif_flag("070.V", record, True)
        self._check_getters["mode3a_smo"] = lambda record: get_as_verif_flag("070.L", record, False)

        self._check_getters["pos_dop_x"] = lambda record: find_value("REF.PA.DOP.DOP-x", record)
        self._check_getters["pos_dop_y"] = lambda record: find_value("REF.PA.DOP.DOP-y", record)
        self._check_getters["pos_dop_xy"] = lambda record: find_value("REF.PA.DOP.DOP-xy", record)

        self._check_getters["pos_lat_deg"] = lambda record: find_value("041.Latitude", record)
        self._check_accuracy["pos_lat_deg"] = 10e-9
        self._check_getters["pos_long_deg"] = lambda record: find_value("041.Longitude", record)
        self._check_accuracy["pos_long_deg"] = 10e-9

        self._check_getters["pos_local_x_nm"] = lambda record: multiply(find_value("042.X", record), M2NM)
        self._check_accuracy["pos_local_x_nm"] = 10e-6
        self._check_getters["pos_local_y_nm"] = lambda record: multiply(find_value("042.Y", record), M2NM)
        self._check_accuracy["pos_local_y_nm"] = 10e-6

        self._check_getters["pos_smoothed"] = lambda record: get_as_verif_flag("170.STH", record, False)

        self._check_getters["pos_std_dev_correlation_coeff"] = lambda record: find_value("REF.PA.SDC.COV-XY (Covariance Component)", record)
        self._check_accuracy["pos_std_dev_correlation_coeff"] = 10e-6
        self._check_getters["pos_std_dev_latlong_corr_coeff"] = lambda record: find_value("REF.PA.SDW.COV-WGS (Lat/Long Covariance Component)", record)
        self._check_accuracy["pos_std_dev_latlong_corr_coeff"] = 10e-6

        self._check_getters["pos_std_dev_lat_deg"] = lambda record: find_value("REF.PA.SDW.SDW (Latitude Component)", record)
        self._check_accuracy["pos_std_dev_lat_deg"] = 10e-9
        self._check_getters["pos_std_dev_long_deg"] = lambda record: find_value("REF.PA.SDW.SDW (Longitude Component)", record)
        self._check_accuracy["pos_std_dev_long_deg"] = 10e-9

        self._check_getters["pos_std_dev_x_m"] = lambda record: find_value("REF.PA.SDC.SDC (X-Component)", record)
        self._check_accuracy["pos_std_dev_x_m"] = 10e-6
        self._check_getters["pos_std_dev_y_m"] = lambda record: find_value("REF.PA.SDC.SDC (Y-Component)", record)
        self._check_accuracy["pos_std_dev_y_m"] = 10e-6

        self._check_getters["rdp_chain"] = lambda record: get_rdp_chain(record)
        self._check_getters["sac"] = lambda record: find_value("010.SAC", record)
        self._check_getters["sic"] = lambda record: find_value("010.SIC", record)

        self._check_getters["simulated_target"] = lambda record: get_as_verif_flag("020.SIM", record, False)
        self._check_getters["spi"] = lambda record: get_as_verif_flag("020.SPI", record, False)
        self._check_getters["target_addr"] = lambda record: find_value("220.Target Address", record)
        self._check_getters["test_target"] = lambda record: get_as_verif_flag("020.TST", record, False)
        self._check_getters["tod"] = lambda record: multiply(find_value("140.Time of Day", record), 128.0)

        self._check_getters["track_climb_desc_mode"] = lambda record: find_value("170.CDM", record)

        self._check_getters["track_coasted"] = lambda record: get_as_verif_flag("170.CST", record, False)
        self._check_getters["track_confirmed"] = lambda record: get_as_verif_flag("170.CNF", record, True)
        self._check_getters["track_end"] = lambda record: get_as_verif_flag("170.CNF", record, False)
        self._check_getters["track_ghost_target"] = lambda record: get_as_verif_flag("170.GHO", record, False)
        self._check_getters["track_manoeuvre_hori"] = lambda record: get_as_verif_flag("170.MAH", record, False)
        self._check_getters["track_num"] = lambda record: find_value("161.Track Number", record)

        self._check_getters["trd_dme"] = lambda record: get_as_verif_flag("020.DME", record, False)
        self._check_getters["trd_hf"] = lambda record: get_as_verif_flag("020.HF", record, False)
        self._check_getters["trd_ms"] = lambda record: get_as_verif_flag("020.MS", record, False)
        self._check_getters["trd_ssr"] = lambda record: get_as_verif_flag("020.SSR", record, False)
        self._check_getters["trd_uat"] = lambda record: get_as_verif_flag("020.UAT", record, False)
        self._check_getters["trd_vdl4"] = lambda record: get_as_verif_flag("020.VDL4", record, False)

        self._check_getters["velocity_vx_ms"] = lambda record: find_value("202.Vx", record)
        self._check_accuracy["velocity_vx_ms"] = 10e-3
        self._check_getters["velocity_vy_ms"] = lambda record: find_value("202.Vy", record)
        self._check_accuracy["velocity_vy_ms"] = 10e-3

        self._check_counts = {}
        self._check_differences = {}  # type: Dict[str:Dict[str:int]]  # var -> (msg -> cnt)

        selected_variables = 'rec_num,'+','.join(self._check_getters.keys())

        for var_name, get_lambda in self._check_getters.items():
            self._check_counts[var_name] = [0, 0]  # passed, failed

        self._mysql_wrapper.prepare_read(selected_variables, 'sd_mlat')
        self._current_row = None

    @property
    def num_records(self):
        return self.__num_records

    @property
    def diff_cnt_sum(self):
        return self._diff_cnt_sum

    def process_record(self, cat, record):

        self.__num_records += 1

        if self._current_row is None:
            self._current_row = self._mysql_wrapper.fetch_one()

        tod_rec = find_value("140.Time of Day", record)*128.0

        #rec_num = self._current_row["rec_num"]
        tod_db = self._current_row["tod"]

        if tod_db != tod_rec: # some mlat reports might be skipped since too fast updates for same track
            #print('skipping record {}, waiting for tod_db {}'.format(self.__num_records, tod_db))
            #print(json.dumps(record, indent=4))
            return
        #else:
        #    print('processing record:'+json.dumps(record, indent=4))

        row = self._current_row

        any_check_failed = False

        #assert rec_num == records_total

        for var_name, get_lambda in self._check_getters.items():
            record_value = get_lambda(record)
            db_value = row[var_name]

            if record_value is not None and db_value is not None and var_name in self._check_accuracy:
                check = math.fabs(record_value-db_value) < self._check_accuracy[var_name]
            else:
                check = record_value == db_value

            if not check:  # failed
                self._check_counts[var_name][1] += 1

                #print(' var {} value record \'{}\' db \'{}\''.format(var_name, record_value, db_value))

                if var_name not in self._check_differences:
                    self._check_differences[var_name] = {}

                diff_msg = 'value record \'{}\' db \'{}\''.format(record_value, db_value)

                if diff_msg in self._check_differences[var_name]:
                    self._check_differences[var_name][diff_msg] += 1
                elif len(self._check_differences[var_name]) < 10:
                    self._check_differences[var_name][diff_msg] = 1

                any_check_failed = True

            else:  # passed
                self._check_counts[var_name][0] += 1

        if any_check_failed:
            self._diff_cnt_sum += 1


        self._current_row = None

    def print(self):
        print('num cat020 records {}'.format(self.__num_records))

        print('rec num cnt {}'.format(self._rec_num_cnt))

        print('diff cnt sum {}'.format(self._diff_cnt_sum))

        for var_name, counts in self._check_counts.items():
            count_sum = counts[0] + counts[1]
            if count_sum:
                if counts[1]:  # some have failed
                    print('variable \'{0}\': checks passed {1} ({2:.3f}%) failed {3} ({4:.3f}%)'.format(
                        var_name, counts[0], 100.0*counts[0]/count_sum, counts[1], 100.0*counts[1]/count_sum))

                    assert var_name in self._check_differences

                    for msg, cnt in self._check_differences[var_name].items():
                        print('\t{}: {}'.format(msg, cnt))
                else:
                    print('variable \'{}\': {} checks passed'.format(var_name, counts[0]))

            else:
                print('variable \'{}\': no data')


# filter functions return True if record should be skipped
def filter_mlat(cat, record):
    global records_total
    records_total += 1
    if cat != 20:
        return True
    return False


def main(argv):

    parser = argparse.ArgumentParser(description='ASTERIX CAT020 mapping to sd_mlat analysis')
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
        framing, statistics_calc.process_record, filter_mlat)  # type: RecordExtractor

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