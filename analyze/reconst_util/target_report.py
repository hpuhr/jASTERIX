#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from reconst_util.geo_position import GeoPosition

from util.common import *
from util.adsb import *


class TargetReport(object):
    def __init__(self, cat, json_data):
        self.__cat = cat
        self.__json_data = json_data

        self._getters = {}
        
        if self.__cat == 21:
            self._getters["aircraft_leng_width"] = lambda record: find_value("271.L+W", record)
            self._getters["airspeed_kt"] = lambda record: get_airspeed_kt(record)
            self._getters["airspeed_mach"] = lambda record: get_airspeed_mach(record)
            self._getters["alt_baro_ft"] = lambda record: multiply(find_value("145.Flight Level", record), 100.0)
            self._getters["alt_geo_ft"] = lambda record: find_value("140.Geometric Height", record)
            self._getters["alt_reporting_capability_ft"] = lambda record: get_adsb_alt_cap_as_double(record)

            self._getters["baro_vertical_rate_ftm"] = lambda record: find_value("155.Barometric Vertical Rate",
                                                                                      record)
            self._getters["baro_vertical_rate_re"] = lambda record: get_as_verif_flag("155.RE", record, False)
            self._getters["calc_alt_geo_ft"] = lambda record: None

            self._getters["callsign"] = lambda record: find_value("170.Target Identification", record)
            self._getters["cat"] = lambda record: 21

            self._getters["descriptor_atp"] = lambda record: find_value("040.ATP", record)
            self._getters["differential_correction"] = lambda record: find_value("040.DCR", record)

            self._getters["emitter_category"] = lambda record: get_ecat_str(record)

            self._getters["figure_of_merit_acas"] = lambda record: None
            self._getters["figure_of_merit_differential"] = lambda record: None
            self._getters["figure_of_merit_multiple"] = lambda record: None
            self._getters["final_sel_altitude_approach"] = lambda record: None
            self._getters["final_sel_altitude_ft"] = lambda record: None
            self._getters["final_sel_altitude_hold"] = lambda record: None
            self._getters["final_sel_altitude_vertical"] = lambda record: None

            self._getters["from_fft"] = lambda record: get_as_verif_flag("040.RAB", record, False)
            self._getters["geo_height_accuracy"] = lambda record: find_value("090.GVA", record)
            self._getters["geo_vertical_rate_ftm"] = lambda record: find_value("157.Geometric Vertical Rate",
                                                                                     record)
            self._getters["ground_bit"] = lambda record: get_as_verif_flag("040.GBS", record, False)

            self._getters["inter_sel_altitude_ft"] = lambda record: None
            self._getters["inter_sel_altitude_info"] = lambda record: None
            self._getters["inter_sel_altitude_source"] = lambda record: None

            self._getters["link_technology_cdti"] = lambda record: 'N'
            self._getters["link_technology_mds"] = lambda record: get_link_technology_mds(record)
            self._getters["link_technology_other"] = lambda record: get_link_technology_other(record)
            self._getters["link_technology_uat"] = lambda record: get_link_technology_uat(record)
            self._getters["link_technology_vdl"] = lambda record: get_link_technology_vdl(record)

            self._getters["magnetic_heading_deg"] = lambda record: find_value("152.Magnetic Heading", record)

            self._getters["mode3a_code"] = lambda record: oct_to_dec(find_value("070.Mode-3/A", record))

            self._getters["mops_version"] = lambda record: find_value("210.VN", record)
            self._getters["nac_p"] = lambda record: find_value("090.NACp", record)
            self._getters["nic_baro"] = lambda record: get_as_verif_flag("090.NICbaro", record, False)
            self._getters["nucp_nic"] = lambda record: find_value("090.NUCp or NIC", record)
            self._getters["nucr_nacv"] = lambda record: find_value("090.NUCr or NACv", record)
            self._getters["position_accuracy"] = lambda record: find_value("090.NUCp or NIC", record)

            self._getters["op_status_ra"] = lambda record: get_as_verif_flag("008.RA", record, False)
            self._getters["op_status_sa"] = lambda record: get_as_verif_flag("008.SA", record, False)
            self._getters["op_status_tcas"] = lambda record: get_as_verif_flag("008.Not TCAS", record, True)

            self._getters["pic"] = lambda record: find_value("090.PIC", record)

            self._getters["pos_lat_deg"] = lambda record: find_value("130.Latitude", record)
            self._getters["pos_long_deg"] = lambda record: find_value("130.Longitude", record)

            self._getters["report_type"] = lambda record: 3
            self._getters["roll_angle_deg"] = lambda record: find_value("230.Roll Angle", record)

            self._getters["sac"] = lambda record: find_value("010.SAC", record)
            self._getters["sam"] = lambda record: find_value("132.MAM", record)
            self._getters["selected_alt_capability"] = lambda record: get_as_verif_flag("040.SAA", record, True)
            self._getters["service_id"] = lambda record: find_value("015.Service Identification", record)
            self._getters["sic"] = lambda record: find_value("010.SIC", record)
            self._getters["sil"] = lambda record: find_value("090.SIL", record)
            self._getters["sils"] = lambda record: get_as_verif_flag("090.SIL-Supplement", record, False)
            self._getters["simulated_target"] = lambda record: get_as_verif_flag("040.SIM", record, False)
            self._getters["spi"] = lambda record: get_as_verif_flag("200.SS", record, False)

            self._getters["surveillance_status"] = lambda record: get_surveillance_status_str(record)

            self._getters["target_addr"] = lambda record: find_value("080.Target Address", record)
            self._getters["target_status"] = lambda record: find_value("200.PS", record)
            self._getters["test_target"] = lambda record: get_as_verif_flag("040.TST", record, False)

            self._getters["toap"] = lambda record: None
            self._getters["toav"] = lambda record: None
            self._getters["tod"] = lambda record: multiply(
                find_value("073.Time of Message Reception for Position", record), 128.0)

            self._getters["tod_accuracy"] = lambda record: None
            self._getters["tod_calculated"] = lambda record: None

            self._getters["torp"] = lambda record: multiply(
                find_value("073.Time of Message Reception for Position", record), 128.0)
            self._getters["torp_fsi"] = lambda record: find_value("074.FSI", record)
            self._getters["torp_precise"] = lambda record: find_value(
                "074.Time of Message Reception of Position - high precision", record)

            self._getters["torv"] = lambda record: multiply(
                find_value("075.Time of Message Reception of Velocity", record), 128.0)
            self._getters["torv_fsi"] = lambda record: find_value("076.FSI", record)
            self._getters["torv_precise"] = lambda record: find_value(
                "076.Time of Message Reception of Velocity - high precision", record)

            self._getters["track_angle_deg"] = lambda record: find_value("160.Track Angle", record)
            self._getters["true_airspeed_kt"] = lambda record: find_value("151.True Air Speed", record)
            self._getters["turnrate_degps"] = lambda record: find_value("165.TAR", record)

            self._getters["turnrate_indicator"] = lambda record: None
            self._getters["velocity_accuracy"] = lambda record: None

        self._position = GeoPosition()
        self._position.setGeoPos(self.get("pos_lat_deg"), self.get("pos_long_deg"))
        #print("wgs84 {} ecef {}".format(self._position.getGeoPosStr(), self._position.getECEFStr()))

    @property
    def cat(self):
        return self.__cat

    @property
    def json_data(self):
        return self.__json_data

    @property
    def position(self):
        return self._position

    def get(self, var_name):
        assert isinstance(var_name, str)
        assert var_name in self._getters
        return self._getters[var_name](self.__json_data)
