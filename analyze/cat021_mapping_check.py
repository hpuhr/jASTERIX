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
from util.adsb import *


class TrackStatisticsCalculator:
    def __init__(self, mysql_wrapper):
        self.__num_records = 0

        self._rec_num_cnt = 0
        self._diff_cnt_sum = 0

        self._mysql_wrapper = mysql_wrapper  # can be None
        assert self._mysql_wrapper is not None

        self._check_accuracy = {}

        # '021' count 803774
        # '021.010' count 803774 (100%)
        # '021.010.SAC' count 803774 (100%) min '50' max '50'
        # '021.010.SIC' count 803774 (100%) min '80' max '80'
        # '021.015' count 803774 (100%)
        # '021.015.Service Identification' count 803774 (100%) min '0' max '0'
        # '021.016' count 803774 (100%)
        # '021.016.RP' count 803774 (100%) min '2.0' max '2.0'
        # '021.020' count 360837 (44%)
        # '021.020.ECAT' count 360837 (100%) min '1' max '15'
        # '021.040' count 803774 (100%)
        # '021.040.ARC' count 803774 (100%) min '0' max '2'
        # '021.040.ATP' count 803774 (100%) min '0' max '0'
        # '021.040.CL' count 778569 (96%) min '0' max '0'
        # '021.040.DCR' count 778569 (96%) min '0' max '0'
        # '021.040.FX' count 803774 (100%) min '0' max '1'
        # '021.040.FX2' count 778569 (96%) min '0' max '0'
        # '021.040.GBS' count 778569 (96%) min '0' max '1'
        # '021.040.RAB' count 803774 (100%) min '0' max '0'
        # '021.040.RC' count 803774 (100%) min '0' max '0'
        # '021.040.SAA' count 778569 (96%) min '0' max '1'
        # '021.040.SIM' count 778569 (96%) min '0' max '0'
        # '021.040.TST' count 778569 (96%) min '0' max '0'
        # '021.070' count 483867 (60%)
        # '021.070.Mode-3/A' count 483867 (100%) min '20' max '7761'
        # '021.073' count 803774 (100%)
        # '021.073.Time of Message Reception for Position' count 803774 (100%) min '28798.953125' max '32398.09375'
        # '021.075' count 779842 (97%)
        # '021.075.Time of Message Reception of Velocity' count 779842 (100%) min '28793.0234375' max '32398.0625'
        # '021.077' count 803774 (100%)
        # '021.077.Time Of ASTERIX Report Transmission' count 803774 (100%) min '28801.0390625' max '32398.203125'
        # '021.080' count 803774 (100%)
        # '021.080.Target Address' count 803774 (100%) min '36852' max '12605630'
        # '021.090' count 803774 (100%)
        # '021.090.FX' count 803774 (100%) min '0' max '1'
        # '021.090.FX2' count 377008 (46%) min '0' max '0'
        # '021.090.NACp' count 377008 (46%) min '0' max '11'
        # '021.090.NICbaro' count 377008 (46%) min '0' max '1'
        # '021.090.NUCp or NIC' count 803774 (100%) min '0' max '11'
        # '021.090.NUCr or NACv' count 803774 (100%) min '0' max '4'
        # '021.090.SIL' count 377008 (46%) min '0' max '3'
        # '021.130' count 803774 (100%)
        # '021.130.Latitude' count 803774 (100%) min '42.3585134635' max '52.940277593320005'
        # '021.130.Longitude' count 803774 (100%) min '3.5712000181000003' max '23.206706139370002'
        # '021.140' count 739387 (91%)
        # '021.140.Geometric Height' count 739387 (100%) min '600.0' max '204793.75'
        # '021.145' count 781541 (97%)
        # '021.145.Flight Level' count 781541 (100%) min '7.75' max '450.0'
        # '021.150' count 1 (0%)
        # '021.150.Air Speed' count 1 (100%) min '1288' max '1288'
        # '021.150.IM' count 1 (100%) min '0' max '0'
        # '021.151' count 6732 (0%)
        # '021.151.RE' count 6732 (100%) min '0' max '0'
        # '021.151.True Air Speed' count 6732 (100%) min '60.0' max '476.0'
        # '021.155' count 299422 (37%)
        # '021.155.Barometric Vertical Rate' count 299422 (100%) min '-6275.0' max '6850.0'
        # '021.155.RE' count 299422 (100%) min '0' max '0'
        # '021.157' count 476223 (59%)
        # '021.157.Geometric Vertical Rate' count 476223 (100%) min '-32637.5' max '5762.5'
        # '021.157.RE' count 476223 (100%) min '0' max '1'
        # '021.160' count 777182 (96%)
        # '021.160.Ground Speed' count 777182 (100%) min '0.0' max '0.16082762025'
        # '021.160.RE' count 777182 (100%) min '0' max '0'
        # '021.160.Track Angle' count 777182 (100%) min '0.0' max '359.8846433909'
        # '021.161' count 803774 (100%)
        # '021.161.Track Number' count 803774 (100%) min '3' max '4095'
        # '021.170' count 769075 (95%)
        # '021.170.Target Identification' count 769075 (100%) min '1758BK  ' max 'YUPZM   '
        # '021.200' count 803774 (100%)
        # '021.200.ICF' count 803774 (100%) min '0' max '0'
        # '021.200.LNAV' count 803774 (100%) min '0' max '0'
        # '021.200.PS' count 803774 (100%) min '0' max '0'
        # '021.200.SS' count 803774 (100%) min '0' max '3'
        # '021.210' count 803774 (100%)
        # '021.210.LTT' count 803774 (100%) min '2' max '2'
        # '021.210.VN' count 803774 (100%) min '0' max '2'
        # '021.210.VNS' count 803774 (100%) min '0' max '0'
        # '021.260' count 209 (0%)
        # '021.260.MB DATA' count 209 (100%) min 'e2000000000000' max 'e2c20004f9af44'
        # '021.index' count 803774 (100%) min '11' max '53324910'
        # '021.length' count 803774 (100%) min '31' max '62'

        self._check_getters = {}
        self._check_getters["airspeed_kt"] = lambda record: get_airspeed_kt(record)
        self._check_getters["airspeed_mach"] = lambda record: get_airspeed_mach(record)
        self._check_getters["alt_baro_ft"] = lambda record: multiply(find_value("145.Flight Level", record), 100.0)
        self._check_getters["alt_geo_ft"] = lambda record: find_value("140.Geometric Height", record)
        self._check_getters["alt_reporting_capability_ft"] = lambda record: get_adsb_alt_cap_as_double(record)

        self._check_getters["baro_vertical_rate_ftm"] = lambda record: find_value("155.Barometric Vertical Rate", record)
        self._check_getters["calc_alt_geo_ft"] = lambda record: None

        self._check_getters["callsign"] = lambda record: find_value("170.Target Identification", record)
        self._check_getters["cat"] = lambda record: 21

        self._check_getters["descriptor_atp"] = lambda record: find_value("040.ATP", record)
        self._check_getters["differential_correction"] = lambda record: find_value("040.DCR", record)

        self._check_getters["emitter_category"] = lambda record: find_value("020.ECAT", record)

        self._check_getters["figure_of_merit_acas"] = lambda record: None
        self._check_getters["figure_of_merit_differential"] = lambda record: None
        self._check_getters["figure_of_merit_multiple"] = lambda record: None
        self._check_getters["final_sel_altitude_approach"] = lambda record: None
        self._check_getters["final_sel_altitude_ft"] = lambda record: None
        self._check_getters["final_sel_altitude_hold"] = lambda record: None
        self._check_getters["final_sel_altitude_vertical"] = lambda record: None

        self._check_getters["from_fft"] = lambda record: get_as_verif_flag("040.RAB", record, False)
        self._check_getters["geo_vertical_rate_ftm"] = lambda record: find_value("157.Geometric Vertical Rate", record)
        self._check_getters["ground_bit"] = lambda record: get_as_verif_flag("040.GBS", record, False)

        self._check_getters["inter_sel_altitude_ft"] = lambda record: None
        self._check_getters["inter_sel_altitude_info"] = lambda record: None
        self._check_getters["inter_sel_altitude_source"] = lambda record: None

        self._check_getters["link_technology_cdti"] = lambda record: 'N'
        self._check_getters["link_technology_mds"] = lambda record: get_link_technology_mds(record)
        self._check_getters["link_technology_other"] = lambda record: get_link_technology_other(record)
        self._check_getters["link_technology_uat"] = lambda record: get_link_technology_uat(record)
        self._check_getters["link_technology_vdl"] = lambda record: get_link_technology_vdl(record)

        self._check_getters["magnetic_heading_deg"] = lambda record: find_value("152.Magnetic Heading", record)

        self._check_getters["mode3a_code"] = lambda record: oct_to_dec(find_value("070.Mode-3/A", record))

        self._check_getters["position_accuracy"] = lambda record: find_value("090.NUCp or NIC", record)

        self._check_getters["pos_lat_deg"] = lambda record: find_value("130.Latitude", record)
        self._check_accuracy["pos_lat_deg"] = 10e-9
        self._check_getters["pos_long_deg"] = lambda record: find_value("130.Longitude", record)
        self._check_accuracy["pos_long_deg"] = 10e-9

        self._check_getters["report_type"] = lambda record: 3
        self._check_getters["roll_angle_deg"] = lambda record: find_value("230.Roll Angle", record)

        self._check_getters["sac"] = lambda record: find_value("010.SAC", record)
        self._check_getters["selected_alt_capability"] = lambda record: get_as_verif_flag("040.SAA", record, True)
        self._check_getters["sic"] = lambda record: find_value("010.SIC", record)
        self._check_getters["simulated_target"] = lambda record: get_as_verif_flag("040.SIM", record, False)
        self._check_getters["spi"] = lambda record: get_as_verif_flag("200.SS", record, False)

        self._check_getters["target_addr"] = lambda record: find_value("080.Target Address", record)
        self._check_getters["target_status"] = lambda record: find_value("200.PS", record)
        self._check_getters["test_target"] = lambda record: get_as_verif_flag("040.TST", record, False)

        self._check_getters["toap"] = lambda record: None
        self._check_getters["toav"] = lambda record: None
        self._check_getters["tod"] = lambda record: multiply(find_value("073.Time of Message Reception for Position", record), 128.0)

        self._check_getters["tod_accuracy"] = lambda record: None
        self._check_getters["tod_calculated"] = lambda record: None

        self._check_getters["torp"] = lambda record: multiply(find_value("073.Time of Message Reception for Position", record), 128.0)
        self._check_getters["torv"] = lambda record: multiply(find_value("075.Time of Message Reception of Velocity", record), 128.0)

        self._check_getters["track_angle_deg"] = lambda record: find_value("160.Track Angle", record)
        self._check_accuracy["track_angle_deg"] = 10e-3
        self._check_getters["true_airspeed_kt"] = lambda record: find_value("151.True Air Speed", record)
        self._check_getters["turnrate_degps"] = lambda record: find_value("165.TAR", record)

        self._check_getters["turnrate_indicator"] = lambda record: None
        self._check_getters["velocity_accuracy"] = lambda record: None

        self._check_counts = {}
        self._check_differences = {}  # type: Dict[str:Dict[str:int]]  # var -> (msg -> cnt)

        selected_variables = 'rec_num,'+','.join(self._check_getters.keys())

        for var_name, get_lambda in self._check_getters.items():
            self._check_counts[var_name] = [0, 0]  # passed, failed

        self._mysql_wrapper.prepare_read(selected_variables, 'sd_ads')
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

            #print(' var {} check {} value record \'{}\' db \'{}\''.format(var_name, check, record_value, db_value))

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
        print('num cat021 records {}'.format(self.__num_records))

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
def filter_adsb(cat, record):
    if cat != 21:
        return True
    return False


def main(argv):

    parser = argparse.ArgumentParser(description='ASTERIX CAT021 mapping to sd_ads analysis')
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
        framing, statistics_calc.process_record, filter_adsb)  # type: RecordExtractor

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