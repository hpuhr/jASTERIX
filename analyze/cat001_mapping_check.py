#!/usr/bin/python

import sys
import json
import time
from typing import Dict
import argparse

from util.record_extractor import RecordExtractor
from util.common import *
from util.radar import *


class TrackStatisticsCalculator:
    def __init__(self, mysql_wrapper):
        self.__num_records = 0

        self._rec_num_cnt = 0
        self._diff_cnt_sum = 0

        self._mysql_wrapper = mysql_wrapper  # can be None
        assert self._mysql_wrapper is not None

        self._check_accuracy = {}

        # '001' count 176297
        # '001.010' count 152717 (86%)
        # '001.010.SAC' count 152717 (100%) min '6' max '98'
        # '001.010.SIC' count 152717 (100%) min '1' max '41'
        # '001.020' count 176297 (100%)
        # '001.020.ANT' count 176297 (100%) min '0' max '0'
        # '001.020.FX' count 176297 (100%) min '0' max '0'
        # '001.020.RAB' count 176297 (100%) min '0' max '1'
        # '001.020.SIM' count 176297 (100%) min '0' max '0'
        # '001.020.SPI' count 176297 (100%) min '0' max '1'
        # '001.020.SSR/PSR' count 176297 (100%) min '0' max '3'
        # '001.020.TYP' count 176297 (100%) min '0' max '1'
        # '001.030' count 2852 (1%)
        # '001.040' count 176033 (99%)
        # '001.040.RHO' count 176033 (100%) min '0.25' max '149.984375'
        # '001.040.THETA' count 176033 (100%) min '0.0' max '359.98352034398'
        # '001.042' count 38608 (21%)
        # '001.042.X-Component' count 38608 (100%) min '-119.703125' max '119.90625'
        # '001.042.Y-Component' count 38608 (100%) min '-119.546875' max '119.546875'
        # '001.070' count 168854 (95%)
        # '001.070.G' count 168854 (100%) min '0' max '1'
        # '001.070.L' count 168854 (100%) min '0' max '1'
        # '001.070.Mode-3/A reply' count 168854 (100%) min '0' max '7777'
        # '001.070.V' count 168854 (100%) min '0' max '1'
        # '001.080' count 1 (0%)
        # '001.080.QXi' count 1 (100%) min '1259' max '1259'
        # '001.090' count 165086 (93%)
        # '001.090.G' count 165086 (100%) min '0' max '1'
        # '001.090.Mode-C HEIGHT' count 165086 (100%) min '-1000.0' max '125100.0'
        # '001.090.V' count 165086 (100%) min '0' max '1'
        # '001.100' count 471 (0%)
        # '001.100.G' count 471 (100%) min '0' max '0'
        # '001.100.Mode-C reply' count 471 (100%) min '0' max '3946'
        # '001.100.QXi' count 471 (100%) min '0' max '3938'
        # '001.100.V' count 471 (100%) min '1' max '1'
        # '001.130' count 122950 (69%)
        # '001.131' count 53521 (30%)
        # '001.131.POWER' count 53521 (100%) min '-86.0' max '-31.0'
        # '001.141' count 176033 (99%)
        # '001.141.Truncated Time of Day' count 176033 (100%) min '0.0' max '511.9921875'
        # '001.150' count 2864 (1%)
        # '001.150.X2' count 2864 (100%) min '0' max '0'
        # '001.150.XA' count 2864 (100%) min '0' max '1'
        # '001.150.XC' count 2864 (100%) min '0' max '1'
        # '001.161' count 38872 (22%)
        # '001.161.TRACK/PLOT NUMBER' count 38872 (100%) min '1' max '508'
        # '001.170' count 38295 (21%)
        # '001.170.CON' count 38295 (100%) min '0' max '0'
        # '001.170.DOU' count 38295 (100%) min '0' max '0'
        # '001.170.FX' count 38295 (100%) min '0' max '1'
        # '001.170.GHO' count 38295 (100%) min '0' max '0'
        # '001.170.MAN' count 38295 (100%) min '0' max '1'
        # '001.170.RAD' count 38295 (100%) min '0' max '1'
        # '001.170.RDPC' count 38295 (100%) min '0' max '0'
        # '001.170.TRE' count 264 (0%) min '1' max '1'
        # '001.200' count 38608 (21%)
        # '001.200.CALCULATED GROUNDSPEED' count 38608 (100%) min '6.103515e-05' max '0.21838376669999998'
        # '001.200.CALCULATED HEADING' count 38608 (100%) min '0.0' max '359.89562971902'
        # '001.index' count 176297 (100%) min '960' max '43998448'
        # '001.length' count 176297 (100%) min '9' max '27'

        self._check_getters = {}

        self._check_getters["acas_aircraft_identification"] = lambda record: None
        self._check_getters["acas_alt_capability_ft"] = lambda record: None
        self._check_getters["acas_bds_1_a"] = lambda record: None
        self._check_getters["acas_bds_1_b"] = lambda record: None
        self._check_getters["acas_comm_capability"] = lambda record: None
        self._check_getters["acas_flight_status"] = lambda record: None
        self._check_getters["acas_modes_service"] = lambda record: None

        self._check_getters["alt_baro_ft"] = lambda record: find_value("090.Mode-C HEIGHT", record)
        self._check_getters["alt_baro_g"] = lambda record: get_as_verif_flag("090.G", record, False)
        self._check_getters["alt_baro_v"] = lambda record: get_as_verif_flag("090.V", record, True)

        self._check_getters["antenna"] = lambda record: get_rdp_antenna(record)
        self._check_getters["calc_alt_geo_ft"] = lambda record: None
        self._check_getters["callsign"] = lambda record: None
        self._check_getters["cat"] = lambda record: 1

        self._check_getters["civil_emergency"] = lambda record: get_civil_emergency(record)

        self._check_getters["detection_type"] = lambda record: find_value("020.TYP", record)
        self._check_getters["from_fft"] = lambda record: get_as_verif_flag("020.RAB", record, False)
        self._check_getters["ground_bit"] = lambda record: None

        self._check_getters["height_3d_ft"] = lambda record: None

        self._check_getters["mil_emergency"] = lambda record: get_as_verif_flag("020.ME", record, False)
        self._check_getters["mil_ident"] = lambda record: get_as_verif_flag("020.MI", record, False)

        self._check_getters["mode3a_code"] = lambda record: oct_to_dec(find_value("070.Mode-3/A reply", record))
        self._check_getters["mode3a_g"] = lambda record: get_as_verif_flag("070.G", record, False)
        self._check_getters["mode3a_v"] = lambda record: get_as_verif_flag("070.V", record, True)
        self._check_getters["mode3a_smo"] = lambda record: get_as_verif_flag("070.L", record, False)

        self._check_getters["modec_code_ft"] = lambda record: find_value("090.Mode-C HEIGHT", record)
        self._check_getters["modec_g"] = lambda record: get_as_verif_flag("090.G", record, False)
        self._check_getters["modec_v"] = lambda record: get_as_verif_flag("090.V", record, True)

        self._check_getters["mode4_friendly"] = lambda record: None

        self._check_getters["mssr_amplitude_dbm"] = lambda record: None
        self._check_getters["mssr_replies_num"] = lambda record: None

        self._check_getters["pos_azm_deg"] = lambda record: find_value("040.THETA", record)
        self._check_accuracy["pos_azm_deg"] = 10e-2
        self._check_getters["pos_range_nm"] = lambda record: find_value("040.RHO", record)
        self._check_accuracy["pos_range_nm"] = 10e-6

        self._check_getters["psr_amplitude_dbm"] = lambda record: None
        self._check_getters["psr_ssr_azm_diff_deg"] = lambda record: None
        self._check_getters["psr_ssr_range_diff_nm"] = lambda record: None

        self._check_getters["rdp_chain"] = lambda record: None

        self._check_getters["report_type"] = lambda record: find_value("020.TYP", record)
        self._check_getters["sac"] = lambda record: find_value("010.SAC", record)
        self._check_getters["sic"] = lambda record: find_value("010.SIC", record)

        self._check_getters["simulated_target"] = lambda record: get_as_verif_flag("020.SIM", record, False)
        self._check_getters["spi"] = lambda record: get_as_verif_flag("020.SPI", record, False)
        self._check_getters["ssr_runlength_deg"] = lambda record: None

        self._check_getters["target_addr"] = lambda record: None
        self._check_getters["test_target"] = lambda record: get_as_verif_flag("020.TST", record, False)
        # self._check_getters["tod"] = lambda record: multiply(find_value("140.Time-of-Day", record), 128.0)

        self._check_getters["track_climb_desc_mode"] = lambda record: None
        self._check_getters["track_confirmed"] = lambda record: get_as_verif_flag("170.CON", record, True)
        self._check_getters["track_doubtful"] = lambda record: get_as_verif_flag("170.DOU", record, False)
        self._check_getters["track_end"] = lambda record: get_as_verif_flag("170.TRE", record, False)
        self._check_getters["track_ghost_target"] = lambda record: get_as_verif_flag("170.GHO", record, False)

        self._check_getters["track_groundspeed_kt"] = lambda record: find_value("200.CALCULATED GROUNDSPEED", record)
        self._check_accuracy["track_groundspeed_kt"] = 10e-3
        self._check_getters["track_heading_deg"] = lambda record: find_value("200.CALCULATED HEADING", record)
        self._check_accuracy["track_heading_deg"] = 10e-3

        self._check_getters["track_manoeuvre_hori"] = lambda record: None
        self._check_getters["track_num"] = lambda record: find_value("161.TRACK/PLOT NUMBER", record)

        self._check_getters["track_rdp_chain"] = lambda record: get_rdp_chain_rdpc(record)

        self._check_getters["track_sigma_x_nm"] = lambda record: None
        self._check_getters["track_sigma_y_nm"] = lambda record: None
        self._check_getters["track_sigma_h_deg"] = lambda record: None
        self._check_getters["track_sigma_h_deg"] = lambda record: None

        self._check_getters["track_slant_corr"] = lambda record: None
        self._check_getters["track_sup"] = lambda record: None
        self._check_getters["track_type"] = lambda record: find_value("170.RAD", record)

        self._check_counts = {}
        self._check_differences = {}  # type: Dict[str:Dict[str:int]]  # var -> (msg -> cnt)

        selected_variables = 'rec_num,'+','.join(self._check_getters.keys())

        for var_name, get_lambda in self._check_getters.items():
            self._check_counts[var_name] = [0, 0]  # passed, failed

        self._mysql_wrapper.prepare_read(selected_variables, 'sd_radar', filter_str="cat=1")

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

        #print('cnt {} db {}'.format(records_total, row["rec_num"]))
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
            #print (json.dumps(record))

    def print(self):
        print('num cat048 records {}'.format(self.__num_records))

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
def filter_system_tracks(cat, record):
    return cat != 1


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
        framing, statistics_calc.process_record, filter_system_tracks)  # type: RecordExtractor

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