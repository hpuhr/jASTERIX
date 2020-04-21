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

        # '021' count 343335
        # '021.008' count 88245 (25%)
        # '021.008.ARV' count 88245 (100%) min '0' max '1'
        # '021.008.CDTI/A' count 88245 (100%) min '0' max '0'
        # '021.008.Not TCAS' count 88245 (100%) min '0' max '1'
        # '021.008.RA' count 88245 (100%) min '0' max '0'
        # '021.008.SA' count 88245 (100%) min '0' max '1'
        # '021.008.TC' count 88245 (100%) min '0' max '0'
        # '021.008.TS' count 88245 (100%) min '0' max '1'
        # '021.010' count 343335 (100%)
        # '021.010.SAC' count 343335 (100%) min '104' max '104'
        # '021.010.SIC' count 343335 (100%) min '38' max '80'
        # '021.015' count 343335 (100%)
        # '021.015.Service Identification' count 343335 (100%) min '0' max '1'
        # '021.016' count 343335 (100%)
        # '021.016.RP' count 343335 (100%) min '0.0' max '1.0'
        # '021.020' count 235916 (68%)
        # '021.020.ECAT' count 235916 (100%) min '0' max '21'
        # '021.040' count 343335 (100%)
        # '021.040.ARC' count 343335 (100%) min '0' max '2'
        # '021.040.ATP' count 343335 (100%) min '0' max '1'
        # '021.040.CL' count 303760 (88%) min '0' max '0'
        # '021.040.DCR' count 303760 (88%) min '0' max '0'
        # '021.040.FX' count 343335 (100%) min '0' max '1'
        # '021.040.FX2' count 303760 (88%) min '0' max '0'
        # '021.040.GBS' count 303760 (88%) min '0' max '1'
        # '021.040.RAB' count 343335 (100%) min '0' max '0'
        # '021.040.RC' count 343335 (100%) min '0' max '0'
        # '021.040.SAA' count 303760 (88%) min '0' max '1'
        # '021.040.SIM' count 303760 (88%) min '0' max '0'
        # '021.040.TST' count 303760 (88%) min '0' max '0'
        # '021.070' count 139042 (40%)
        # '021.070.Mode-3/A' count 139042 (100%) min '0' max '7151'
        # '021.073' count 343335 (100%)
        # '021.073.Time of Message Reception for Position' count 343335 (100%) min '14414.109375' max '28801.0625'
        # '021.074' count 117591 (34%)
        # '021.074.FSI' count 117591 (100%) min '0' max '2'
        # '021.074.Time of Message Reception of Position - high precision' count 117591 (100%) min '1.339428120174e-05' max '0.9999990329879529'
        # '021.075' count 230792 (67%)
        # '021.075.Time of Message Reception of Velocity' count 230792 (100%) min '14413.8984375' max '28800.890625'
        # '021.076' count 49845 (14%)
        # '021.076.FSI' count 49845 (100%) min '0' max '2'
        # '021.076.Time of Message Reception of Velocity - high precision' count 49845 (100%) min '3.5450793627050004e-05' max '0.999962919092656'
        # '021.077' count 343335 (100%)
        # '021.077.Time Of ASTERIX Report Transmission' count 343335 (100%) min '14414.40625' max '28801.171875'
        # '021.080' count 343335 (100%)
        # '021.080.Target Address' count 343335 (100%) min '3416777' max '14979656'
        # '021.090' count 343335 (100%)
        # '021.090.FX' count 343335 (100%) min '0' max '1'
        # '021.090.FX2' count 152295 (44%) min '0' max '1'
        # '021.090.FX3' count 117591 (34%) min '0' max '1'
        # '021.090.FX4' count 114029 (33%) min '0' max '0'
        # '021.090.GVA' count 117591 (34%) min '0' max '2'
        # '021.090.NACp' count 152295 (44%) min '0' max '11'
        # '021.090.NICbaro' count 152295 (44%) min '0' max '1'
        # '021.090.NUCp or NIC' count 343335 (100%) min '0' max '11'
        # '021.090.NUCr or NACv' count 343335 (100%) min '0' max '2'
        # '021.090.PIC' count 114029 (33%) min '6' max '14'
        # '021.090.SDA' count 117591 (34%) min '0' max '3'
        # '021.090.SIL' count 152295 (44%) min '0' max '3'
        # '021.090.SIL-Supplement' count 117591 (34%) min '0' max '0'
        # '021.130' count 343335 (100%)
        # '021.130.Latitude' count 343335 (100%) min '28.19776018137' max '46.14574930316'
        # '021.130.Longitude' count 343335 (100%) min '-22.90883076443' max '-1.11888874448'
        # '021.132' count 117591 (34%)
        # '021.132.MAM' count 117591 (100%) min '-95.0' max '-23.0'
        # '021.140' count 209098 (60%)
        # '021.140.Geometric Height' count 209098 (100%) min '175.0' max '43600.0'
        # '021.145' count 240394 (70%)
        # '021.145.Flight Level' count 240394 (100%) min '2.0' max '431.75'
        # '021.146' count 39596 (11%)
        # '021.146.Altitude' count 39596 (100%) min '3000.0' max '41000.0'
        # '021.146.SAS' count 39596 (100%) min '1' max '1'
        # '021.146.Source' count 39596 (100%) min '2' max '3'
        # '021.155' count 37081 (10%)
        # '021.155.Barometric Vertical Rate' count 37081 (100%) min '-4675.0' max '5312.5'
        # '021.155.RE' count 37081 (100%) min '0' max '0'
        # '021.157' count 12762 (3%)
        # '021.157.Geometric Vertical Rate' count 12762 (100%) min '-6275.0' max '6081.25'
        # '021.157.RE' count 12762 (100%) min '0' max '0'
        # '021.160' count 49845 (14%)
        # '021.160.Ground Speed' count 49845 (100%) min '0.0257568333' max '0.1474609224'
        # '021.160.RE' count 49845 (100%) min '0' max '0'
        # '021.160.Track Angle' count 49845 (100%) min '0.0' max '359.87365706278'
        # '021.161' count 343335 (100%)
        # '021.161.Track Number' count 343335 (100%) min '1' max '4063'
        # '021.170' count 238248 (69%)
        # '021.170.Target Identification' count 238248 (100%) min '????????' max 'VVV641  '
        # '021.200' count 285627 (83%)
        # '021.200.ICF' count 285627 (100%) min '0' max '1'
        # '021.200.LNAV' count 285627 (100%) min '0' max '1'
        # '021.200.PS' count 285627 (100%) min '0' max '0'
        # '021.200.SS' count 285627 (100%) min '0' max '2'
        # '021.210' count 343335 (100%)
        # '021.210.LTT' count 343335 (100%) min '2' max '2'
        # '021.210.VN' count 343335 (100%) min '0' max '2'
        # '021.210.VNS' count 343335 (100%) min '0' max '1'
        # '021.271' count 487 (0%)
        # '021.271.B2 low' count 487 (100%) min '0' max '0'
        # '021.271.CDTI/S' count 487 (100%) min '0' max '1'
        # '021.271.FX' count 487 (100%) min '1' max '1'
        # '021.271.IDENT' count 487 (100%) min '0' max '0'
        # '021.271.L+W' count 487 (100%) min '1' max '11'
        # '021.271.POA' count 487 (100%) min '0' max '0'
        # '021.271.RAS' count 487 (100%) min '0' max '0'
        # '021.295' count 65795 (19%)
        # '021.295.Aircraft Operational Status' count 41255 (62%)
        # '021.295.Aircraft Operational Status.Age' count 41255 (100%) min '0.0' max '16.7'
        # '021.295.Barometric Vertical Rate' count 37081 (56%)
        # '021.295.Barometric Vertical Rate.Age' count 37081 (100%) min '0.0' max '9.8'
        # '021.295.Flight Level' count 59395 (90%)
        # '021.295.Flight Level.age' count 59395 (100%) min '0.0' max '0.0'
        # '021.295.Geometric Height' count 44565 (67%)
        # '021.295.Geometric Height.Age' count 44565 (100%) min '0.0' max '9.9'
        # '021.295.Geometric Vertical Rate' count 12762 (19%)
        # '021.295.Geometric Vertical Rate.Age' count 12762 (100%) min '0.0' max '10.0'
        # '021.295.Ground Vector' count 49845 (75%)
        # '021.295.Ground Vector.Age' count 49845 (100%) min '0.0' max '10.0'
        # '021.295.Intermediate State Selected Altitude' count 39597 (60%)
        # '021.295.Intermediate State Selected Altitude.Age' count 39597 (100%) min '0.0' max '25.0'
        # '021.295.Mode 3/A' count 41627 (63%)
        # '021.295.Mode 3/A.Age' count 41627 (100%) min '0.0' max '25.5'
        # '021.295.Quality Indicators' count 41724 (63%)
        # '021.295.Quality Indicators.Age' count 41724 (100%) min '0.0' max '16.7'
        # '021.295.Surface Capabilities and Characteristics' count 487 (0%)
        # '021.295.Surface Capabilities and Characteristics.Age' count 487 (100%) min '0.0' max '25.5'
        # '021.295.Target Identification' count 56204 (85%)
        # '021.295.Target Identification.Age' count 56204 (100%) min '0.0' max '25.5'
        # '021.295.Target Status' count 41767 (63%)
        # '021.295.Target Status.Age' count 41767 (100%) min '0.0' max '25.5'
        # '021.REF' count 79918 (23%) min '0400' max 'fc08280525108845f19000'
        # '021.index' count 343335 (100%) min '11' max '31807354'
        # '021.length' count 343335 (100%) min '31' max '91'

        self._check_getters = {}
        self._check_getters["aircraft_leng_width"] = lambda record: find_value("271.L+W", record)
        self._check_getters["airspeed_kt"] = lambda record: get_airspeed_kt(record)
        self._check_getters["airspeed_mach"] = lambda record: get_airspeed_mach(record)
        self._check_getters["alt_baro_ft"] = lambda record: multiply(find_value("145.Flight Level", record), 100.0)
        self._check_getters["alt_geo_ft"] = lambda record: find_value("140.Geometric Height", record)
        self._check_getters["alt_reporting_capability_ft"] = lambda record: get_adsb_alt_cap_as_double(record)

        self._check_getters["baro_vertical_rate_ftm"] = lambda record: find_value("155.Barometric Vertical Rate", record)
        self._check_getters["baro_vertical_rate_re"] = lambda record: get_as_verif_flag("155.RE", record, False)
        self._check_getters["calc_alt_geo_ft"] = lambda record: None

        self._check_getters["callsign"] = lambda record: find_value("170.Target Identification", record)
        self._check_getters["cat"] = lambda record: 21

        self._check_getters["descriptor_atp"] = lambda record: find_value("040.ATP", record)
        self._check_getters["differential_correction"] = lambda record: find_value("040.DCR", record)

        self._check_getters["emitter_category"] = lambda record: get_ecat_str(record)

        self._check_getters["figure_of_merit_acas"] = lambda record: None
        self._check_getters["figure_of_merit_differential"] = lambda record: None
        self._check_getters["figure_of_merit_multiple"] = lambda record: None
        self._check_getters["final_sel_altitude_approach"] = lambda record: None
        self._check_getters["final_sel_altitude_ft"] = lambda record: None
        self._check_getters["final_sel_altitude_hold"] = lambda record: None
        self._check_getters["final_sel_altitude_vertical"] = lambda record: None

        self._check_getters["from_fft"] = lambda record: get_as_verif_flag("040.RAB", record, False)
        self._check_getters["geo_height_accuracy"] = lambda record: find_value("090.GVA", record)
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

        self._check_getters["mops_version"] = lambda record: find_value("210.VN", record)
        self._check_getters["nac_p"] = lambda record: find_value("090.NACp", record)
        self._check_getters["nic_baro"] = lambda record: get_as_verif_flag("090.NICbaro", record, False)
        self._check_getters["nucp_nic"] = lambda record: find_value("090.NUCp or NIC", record)
        self._check_getters["nucr_nacv"] = lambda record: find_value("090.NUCr or NACv", record)
        self._check_getters["position_accuracy"] = lambda record: find_value("090.NUCp or NIC", record)

        self._check_getters["op_status_ra"] = lambda record: get_as_verif_flag("008.RA", record, False)
        self._check_getters["op_status_sa"] = lambda record: get_as_verif_flag("008.SA", record, False)
        self._check_getters["op_status_tcas"] = lambda record: get_as_verif_flag("008.Not TCAS", record, True)

        self._check_getters["pic"] = lambda record: find_value("090.PIC", record)

        self._check_getters["pos_lat_deg"] = lambda record: find_value("130.Latitude", record)
        self._check_accuracy["pos_lat_deg"] = 10e-9
        self._check_getters["pos_long_deg"] = lambda record: find_value("130.Longitude", record)
        self._check_accuracy["pos_long_deg"] = 10e-9

        self._check_getters["report_type"] = lambda record: 3
        self._check_getters["roll_angle_deg"] = lambda record: find_value("230.Roll Angle", record)

        self._check_getters["sac"] = lambda record: find_value("010.SAC", record)
        self._check_getters["sam"] = lambda record: find_value("132.MAM", record)
        self._check_getters["selected_alt_capability"] = lambda record: get_as_verif_flag("040.SAA", record, True)
        self._check_getters["service_id"] = lambda record: find_value("015.Service Identification", record)
        self._check_getters["sic"] = lambda record: find_value("010.SIC", record)
        self._check_getters["sil"] = lambda record: find_value("090.SIL", record)
        self._check_getters["sils"] = lambda record: get_as_verif_flag("090.SIL-Supplement", record, False)
        self._check_getters["simulated_target"] = lambda record: get_as_verif_flag("040.SIM", record, False)
        self._check_getters["spi"] = lambda record: get_as_verif_flag("200.SS", record, False)

        self._check_getters["surveillance_status"] = lambda record: get_surveillance_status_str(record)

        self._check_getters["target_addr"] = lambda record: find_value("080.Target Address", record)
        self._check_getters["target_status"] = lambda record: find_value("200.PS", record)
        self._check_getters["test_target"] = lambda record: get_as_verif_flag("040.TST", record, False)

        self._check_getters["toap"] = lambda record: None
        self._check_getters["toav"] = lambda record: None
        self._check_getters["tod"] = lambda record: multiply(find_value("073.Time of Message Reception for Position", record), 128.0)

        self._check_getters["tod_accuracy"] = lambda record: None
        self._check_getters["tod_calculated"] = lambda record: None

        self._check_getters["torp"] = lambda record: multiply(find_value("073.Time of Message Reception for Position", record), 128.0)
        self._check_getters["torp_fsi"] = lambda record: find_value("074.FSI", record)
        self._check_getters["torp_precise"] = lambda record: find_value("074.Time of Message Reception of Position - high precision", record)
        self._check_accuracy["torp_precise"] = 10e-6

        self._check_getters["torv"] = lambda record: multiply(find_value("075.Time of Message Reception of Velocity", record), 128.0)
        self._check_getters["torv_fsi"] = lambda record: find_value("076.FSI", record)
        self._check_getters["torv_precise"] = lambda record: find_value("076.Time of Message Reception of Velocity - high precision", record)
        self._check_accuracy["torv_precise"] = 10e-6


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

        self._mysql_wrapper.prepare_read(selected_variables, 'sd_ads', order_var_str='rec_num')
        self._current_row = None
        self._data_done = False

    @property
    def num_records(self):
        return self.__num_records

    @property
    def diff_cnt_sum(self):
        return self._diff_cnt_sum

    def process_record(self, cat, record):

        if self._data_done:
            return

        self.__num_records += 1

        if self._current_row is None:
            self._current_row = self._mysql_wrapper.fetch_one()

            if self._current_row is None:
                print('end of db data')
                self._data_done = True
                return

        tod_rec = find_value("073.Time of Message Reception for Position", record)*128.0
        target_addr = find_value("080.Target Address", record)

        #rec_num = self._current_row["rec_num"]
        tod_db = self._current_row["tod"]
        target_addr_db = self._current_row["target_addr"]

        if target_addr_db != target_addr or tod_db != tod_rec:  # some mlat reports might be skipped since too fast updates for same track
            #print('skipping record {} time {}, waiting for tod_db {}'.format(self.__num_records, tod_rec, tod_db))
            #print(json.dumps(record, indent=4))
            return

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
    assert args.framing == 'True' or args.framing == 'False'
    framing = args.framing == 'True'

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