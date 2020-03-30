#!/usr/bin/python

import sys
import json
import time
from typing import Dict
import argparse

from util.record_extractor import RecordExtractor
from util.common import *
from util.radar import *

records_total = 0


class TrackStatisticsCalculator:
    def __init__(self, mysql_wrapper):
        self.__num_records = 0

        self._rec_num_cnt = 0
        self._diff_cnt_sum = 0

        self._mysql_wrapper = mysql_wrapper  # can be None
        assert self._mysql_wrapper is not None

        self._check_accuracy = {}

        # '048' count 330095
        # '048.010' count 330095 (100%)
        # '048.010.SAC' count 330095 (100%) min '4' max '112'
        # '048.010.SIC' count 330095 (100%) min '1' max '190'
        # '048.020' count 330095 (100%)
        # '048.020.FOE/FRI' count 11955 (3%) min '0' max '0'
        # '048.020.FX' count 330095 (100%) min '0' max '1'
        # '048.020.FX2' count 11955 (3%) min '0' max '0'
        # '048.020.ME' count 11955 (3%) min '0' max '0'
        # '048.020.MI' count 11955 (3%) min '0' max '0'
        # '048.020.RAB' count 330095 (100%) min '0' max '1'
        # '048.020.RDP' count 330095 (100%) min '0' max '1'
        # '048.020.SIM' count 330095 (100%) min '0' max '0'
        # '048.020.SPI' count 330095 (100%) min '0' max '1'
        # '048.020.TST' count 11955 (3%) min '0' max '1'
        # '048.020.TYP' count 330095 (100%) min '0' max '7'
        # '048.030' count 661 (0%)
        # '048.040' count 324277 (98%)
        # '048.040.RHO' count 324277 (100%) min '0.13671875' max '207.87890625'
        # '048.040.THETA' count 324277 (100%) min '0.0' max '359.9945066721'
        # '048.042' count 210203 (63%)
        # '048.042.X-Component' count 210203 (100%) min '-149.90625' max '151.3984375'
        # '048.042.Y-Component' count 210203 (100%) min '-150.2890625' max '137.109375'
        # '048.070' count 298843 (90%)
        # '048.070.G' count 298843 (100%) min '0' max '1'
        # '048.070.L' count 298843 (100%) min '0' max '1'
        # '048.070.Mode-3/A reply' count 298843 (100%) min '100' max '7777'
        # '048.070.V' count 298843 (100%) min '0' max '1'
        # '048.090' count 297309 (90%)
        # '048.090.Flight Level' count 297309 (100%) min '-10.0' max '949.0'
        # '048.090.G' count 297309 (100%) min '0' max '1'
        # '048.090.V' count 297309 (100%) min '0' max '1'
        # '048.100' count 110 (0%)
        # '048.100.G' count 110 (100%) min '0' max '1'
        # '048.100.Mode-C reply' count 110 (100%) min '0' max '3938'
        # '048.100.QXi' count 110 (100%) min '0' max '2560'
        # '048.100.V' count 110 (100%) min '0' max '1'
        # '048.120' count 3838 (1%)
        # '048.120.CAL' count 3838 (100%)
        # '048.120.CAL.CAL' count 3838 (100%) min '-186' max '186'
        # '048.120.CAL.D' count 3838 (100%) min '0' max '0'
        # '048.130' count 246719 (74%)
        # '048.130.APD' count 7461 (3%)
        # '048.130.APD.value' count 7461 (100%) min '-1.07666015625' max '1.07666015625'
        # '048.130.PAM' count 56185 (22%)
        # '048.130.PAM.value' count 56185 (100%) min '22' max '230'
        # '048.130.PRL' count 47084 (19%)
        # '048.130.PRL.value' count 47084 (100%) min '0.0' max '11.2060546875'
        # '048.130.RPD' count 7527 (3%)
        # '048.130.RPD.value' count 7527 (100%) min '-0.109375' max '0.10546875'
        # '048.130.SAM' count 233219 (94%)
        # '048.130.SAM.value' count 233219 (100%) min '-87' max '-18'
        # '048.130.SRL' count 9 (0%)
        # '048.130.SRL.value' count 9 (100%) min '0.966796875' max '3.251953125'
        # '048.130.SRR' count 197664 (80%)
        # '048.130.SRR.value' count 197664 (100%) min '1' max '14'
        # '048.140' count 330095 (100%)
        # '048.140.Time-of-Day' count 330095 (100%) min '7173.9609375' max '10801.8125'
        # '048.161' count 297926 (90%)
        # '048.161.TRACK NUMBER' count 297926 (100%) min '1' max '4095'
        # '048.170' count 297926 (90%)
        # '048.170.CDM' count 297926 (100%) min '0' max '3'
        # '048.170.CNF' count 297926 (100%) min '0' max '1'
        # '048.170.DOU' count 297926 (100%) min '0' max '1'
        # '048.170.FX' count 297926 (100%) min '0' max '1'
        # '048.170.GHO' count 16340 (5%) min '0' max '1'
        # '048.170.MAH' count 297926 (100%) min '0' max '1'
        # '048.170.RAD' count 297926 (100%) min '0' max '2'
        # '048.170.SUP' count 16340 (5%) min '0' max '0'
        # '048.170.TCC' count 16340 (5%) min '0' max '1'
        # '048.170.TRE' count 16340 (5%) min '0' max '1'
        # '048.200' count 292643 (88%)
        # '048.200.CALCULATED GROUNDSPEED' count 292643 (100%) min '0.0' max '3051.122737075959'
        # '048.200.CALCULATED HEADING' count 292643 (100%) min '0.0' max '359.9945066721'
        # '048.220' count 309394 (93%)
        # '048.220.AIRCRAFT ADDRESS' count 309394 (100%) min '262192' max '12591332'
        # '048.230' count 309394 (93%)
        # '048.230.AIC' count 309394 (100%) min '0' max '1'
        # '048.230.ARC' count 309394 (100%) min '0' max '1'
        # '048.230.B1A' count 309394 (100%) min '0' max '1'
        # '048.230.B1B' count 309394 (100%) min '0' max '13'
        # '048.230.COM' count 309394 (100%) min '0' max '3'
        # '048.230.MSSC' count 309394 (100%) min '0' max '1'
        # '048.230.STAT' count 309394 (100%) min '0' max '7'
        # '048.240' count 296630 (89%)
        # '048.240.Aircraft Identification' count 296630 (100%) min '00000000' max 'ZXP02   '
        # '048.250' count 222881 (67%)
        # '048.250.REP' count 222881 (100%) min '1' max '6'
        # '048.260' count 76 (0%)
        # '048.260.MB DATA' count 76 (100%) min '30800000000000' max '30800000000000'
        # '048.index' count 330095 (100%) min '321' max '43998675'
        # '048.length' count 330095 (100%) min '12' max '92'

        self._check_getters = {}

        self._check_getters["acas_aircraft_identification"] = lambda record: get_as_verif_flag("230.AIC", record, False) # why inverted?
        self._check_getters["acas_alt_capability_ft"] = lambda record: get_alt_cap_as_double(record)
        self._check_getters["acas_bds_1_a"] = lambda record: find_value("230.B1A", record)
        self._check_getters["acas_bds_1_b"] = lambda record: find_value("230.B1B", record)
        self._check_getters["acas_comm_capability"] = lambda record: find_value("230.COM", record)
        self._check_getters["acas_flight_status"] = lambda record: find_value("230.STAT", record)
        self._check_getters["acas_modes_service"] = lambda record: get_as_verif_flag("230.MSSC", record, False)

        self._check_getters["alt_baro_ft"] = lambda record: multiply(find_value("090.Flight Level", record), 100.0)
        self._check_getters["alt_baro_g"] = lambda record: get_as_verif_flag("090.G", record, False)
        self._check_getters["alt_baro_v"] = lambda record: get_as_verif_flag("090.V", record, True)

        self._check_getters["antenna"] = lambda record: None
        self._check_getters["calc_alt_geo_ft"] = lambda record: None

        self._check_getters["callsign"] = lambda record: find_value("240.Aircraft Identification", record)
        self._check_getters["cat"] = lambda record: 48

        self._check_getters["civil_emergency"] = lambda record: get_civil_emergency(record)

        self._check_getters["detection_type"] = lambda record: find_value("020.TYP", record)
        self._check_getters["from_fft"] = lambda record: get_as_verif_flag("020.RAB", record, False)
        self._check_getters["ground_bit"] = lambda record: get_ground_bit(record)
        self._check_getters["height_3d_ft"] = lambda record: None

        self._check_getters["mil_emergency"] = lambda record: get_as_verif_flag("020.ME", record, False)
        self._check_getters["mil_ident"] = lambda record: get_as_verif_flag("020.MI", record, False)

        self._check_getters["mode3a_code"] = lambda record: oct_to_dec(find_value("070.Mode-3/A reply", record))
        self._check_getters["mode3a_g"] = lambda record: get_as_verif_flag("070.G", record, False)
        self._check_getters["mode3a_v"] = lambda record: get_as_verif_flag("070.V", record, True)
        self._check_getters["mode3a_smo"] = lambda record: get_as_verif_flag("070.L", record, False)

        self._check_getters["modec_code_ft"] = lambda record: multiply(find_value("090.Flight Level", record), 100.0)
        self._check_getters["modec_g"] = lambda record: get_as_verif_flag("090.G", record, False)
        self._check_getters["modec_v"] = lambda record: get_as_verif_flag("090.V", record, True)

        self._check_getters["mode4_friendly"] = lambda record: get_mode4_friendly(record)

        self._check_getters["mssr_amplitude_dbm"] = lambda record: find_value("130.SAM.value", record)
        self._check_getters["mssr_replies_num"] = lambda record: find_value("130.SRR.value", record)

        self._check_getters["pos_azm_deg"] = lambda record: find_value("040.THETA", record)
        self._check_accuracy["pos_azm_deg"] = 10e-2
        self._check_getters["pos_range_nm"] = lambda record: find_value("040.RHO", record)
        self._check_accuracy["pos_range_nm"] = 10e-6

        self._check_getters["psr_amplitude_dbm"] = lambda record: find_value("130.PAM.value", record)
        self._check_getters["psr_ssr_azm_diff_deg"] = lambda record: find_value("130.APD.value", record)
        self._check_getters["psr_ssr_range_diff_nm"] = lambda record: find_value("130.RPD.value", record)

        self._check_getters["rdp_chain"] = lambda record: get_rdp_chain(record)

        self._check_getters["report_type"] = lambda record: get_plot_track(record)
        self._check_getters["sac"] = lambda record: find_value("010.SAC", record)
        self._check_getters["sic"] = lambda record: find_value("010.SIC", record)


        self._check_getters["simulated_target"] = lambda record: get_as_verif_flag("020.SIM", record, False)
        self._check_getters["spi"] = lambda record: get_as_verif_flag("020.SPI", record, False)
        self._check_getters["ssr_runlength_deg"] = lambda record: find_value("130.SRL.value", record)

        self._check_getters["target_addr"] = lambda record: find_value("220.AIRCRAFT ADDRESS", record)
        self._check_getters["test_target"] = lambda record: get_as_verif_flag("020.TST", record, False)
        self._check_getters["tod"] = lambda record: multiply(find_value("140.Time-of-Day", record), 128.0)

        self._check_getters["track_climb_desc_mode"] = lambda record: get_climb_descend_mode(record)
        self._check_getters["track_confirmed"] = lambda record: get_as_verif_flag("170.CNF", record, True)
        self._check_getters["track_doubtful"] = lambda record: get_as_verif_flag("170.DOU", record, False)
        self._check_getters["track_end"] = lambda record: get_as_verif_flag("170.TRE", record, False)
        self._check_getters["track_ghost_target"] = lambda record: get_as_verif_flag("170.GHO", record, False)

        self._check_getters["track_groundspeed_kt"] = lambda record: find_value("200.CALCULATED GROUNDSPEED", record)
        self._check_accuracy["track_groundspeed_kt"] = 10e-3
        self._check_getters["track_heading_deg"] = lambda record: find_value("200.CALCULATED HEADING", record)
        self._check_accuracy["track_heading_deg"] = 10e-3

        self._check_getters["track_manoeuvre_hori"] = lambda record: get_as_verif_flag("170.MAH", record, False)
        self._check_getters["track_num"] = lambda record: find_value("161.TRACK NUMBER", record)

        self._check_getters["track_sigma_x_nm"] = lambda record: find_value("210.Sigma (X)", record)
        self._check_getters["track_sigma_y_nm"] = lambda record: find_value("210.Sigma (Y)", record)
        self._check_getters["track_sigma_h_deg"] = lambda record: find_value("210.Sigma (V)", record)
        self._check_getters["track_sigma_h_deg"] = lambda record: find_value("210.Sigma (H)", record)

        self._check_getters["track_slant_corr"] = lambda record: get_as_verif_flag("170.TCC", record, False)
        self._check_getters["track_sup"] = lambda record: get_as_verif_flag("170.SUP", record, False)
        self._check_getters["track_type"] = lambda record: None

        # self._check_getters["trd_dme"] = lambda record: get_as_verif_flag("020.DME", record, False)
        # self._check_getters["trd_hf"] = lambda record: get_as_verif_flag("020.HF", record, False)
        # self._check_getters["trd_ms"] = lambda record: get_as_verif_flag("020.MS", record, False)
        # self._check_getters["trd_ssr"] = lambda record: get_as_verif_flag("020.SSR", record, False)
        # self._check_getters["trd_uat"] = lambda record: get_as_verif_flag("020.UAT", record, False)
        # self._check_getters["trd_vdl4"] = lambda record: get_as_verif_flag("020.VDL4", record, False)
        #
        # self._check_getters["velocity_vx_ms"] = lambda record: find_value("202.Vx", record)
        # self._check_accuracy["velocity_vx_ms"] = 10e-3
        # self._check_getters["velocity_vy_ms"] = lambda record: find_value("202.Vy", record)
        # self._check_accuracy["velocity_vy_ms"] = 10e-3


        self._check_counts = {}
        self._check_differences = {}  # type: Dict[str:Dict[str:int]]  # var -> (msg -> cnt)

        selected_variables = 'rec_num,'+','.join(self._check_getters.keys())

        for var_name, get_lambda in self._check_getters.items():
            self._check_counts[var_name] = [0, 0]  # passed, failed

        self._mysql_wrapper.prepare_read(selected_variables, 'sd_radar', filter_str="cat=48")

    @property
    def num_records(self):
        return self.__num_records

    @property
    def diff_cnt_sum(self):
        return self._diff_cnt_sum

    def process_record(self, cat, record):

        self.__num_records += 1
        global records_total

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
    global records_total
    records_total += 1

    return cat != 48


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