#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from reconst_util.geo_position import GeoPosition

from util.common import *
from util.adsb import *

import numpy as np

class TargetReport(object):
    def __init__(self, cat, json_data):
        self.__cat = cat
        self._json_data = json_data

        self._position = None

        #print("wgs84 {} ecef {}".format(self._position.getGeoPosStr(), self._position.getECEFStr()))

    @property
    def cat(self):
        return self.__cat

    @property
    def json_data(self):
        return self._json_data

    @property
    def position(self):
        return self._position

# stddev in meters

v0_pos_accuracies = {
    1: 9260,
    2: 4630,
    3: 926,
    4: 463,
    5: 231.5,
    6: 92.5,
    7: 46.5,
    8: 5,
    9: 1.5
}

v0_spd_accuracies = {
    1: 5,
    2: 1.5,
    3: 0.5
}

v12_pos_accuracies = {
    1: 9260,
    2: 3704,
    3: 1852,
    4: 926,
    5: 463,
    6: 278,
    7: 92.5,
    8: 46.5,
    9: 15,
    10: 5,
    11: 1.5
}

v12_spd_accuracies = {
    1: 5,
    2: 1.5,
    3: 0.5,
    4: 0.15
}

class ADSBTargetReport(TargetReport):
    _getters = {
        "aircraft_leng_width": lambda record: find_value("271.L+W", record),
        "airspeed_kt": lambda record: get_airspeed_kt(record),
        "airspeed_mach": lambda record: get_airspeed_mach(record),
        "alt_baro_ft": lambda record: multiply(find_value("145.Flight Level", record), 100.0),
        "alt_geo_ft": lambda record: find_value("140.Geometric Height", record),
        "alt_reporting_capability_ft": lambda record: get_adsb_alt_cap_as_double(record),
        "baro_vertical_rate_ftm": lambda record: find_value("155.Barometric Vertical Rate", record),
        "baro_vertical_rate_re": lambda record: get_as_verif_flag("155.RE", record, False),
        "calc_alt_geo_ft": lambda record: None,
        "callsign": lambda record: find_value("170.Target Identification", record),
        "cat": lambda record: 21,
        "descriptor_atp": lambda record: find_value("040.ATP", record),
        "differential_correction": lambda record: find_value("040.DCR", record),
        "emitter_category": lambda record: get_ecat_str(record),
        "figure_of_merit_acas": lambda record: None,
        "figure_of_merit_differential": lambda record: None,
        "figure_of_merit_multiple": lambda record: None,
        "final_sel_altitude_approach": lambda record: None,
        "final_sel_altitude_ft": lambda record: None,
        "final_sel_altitude_hold": lambda record: None,
        "final_sel_altitude_vertical": lambda record: None,
        "from_fft": lambda record: get_as_verif_flag("040.RAB", record, False),
        "geo_height_accuracy": lambda record: find_value("090.GVA", record),
        "geo_vertical_rate_ftm": lambda record: find_value("157.Geometric Vertical Rate", record),
        "ground_bit": lambda record: get_as_verif_flag("040.GBS", record, False),
        "groundspeed_kt": lambda record: multiply(find_value("160.Ground Speed", record), 3600),  # nm/s to knots
        "inter_sel_altitude_ft": lambda record: None, "inter_sel_altitude_info": lambda record: None,
        "inter_sel_altitude_source": lambda record: None, "link_technology_cdti": lambda record: 'N',
        "link_technology_mds": lambda record: get_link_technology_mds(record),
        "link_technology_other": lambda record: get_link_technology_other(record),
        "link_technology_uat": lambda record: get_link_technology_uat(record),
        "link_technology_vdl": lambda record: get_link_technology_vdl(record),
        "magnetic_heading_deg": lambda record: find_value("152.Magnetic Heading", record),
        "mode3a_code": lambda record: oct_to_dec(find_value("070.Mode-3/A", record)),
        "mops_version": lambda record: find_value("210.VN", record),
        "nac_p": lambda record: find_value("090.NACp", record),
        "nic_baro": lambda record: get_as_verif_flag("090.NICbaro", record, False),
        "nucp_nic": lambda record: find_value("090.NUCp or NIC", record),
        "nucr_nacv": lambda record: find_value("090.NUCr or NACv", record),
        "position_accuracy": lambda record: find_value("090.NUCp or NIC", record),
        "op_status_ra": lambda record: get_as_verif_flag("008.RA", record, False),
        "op_status_sa": lambda record: get_as_verif_flag("008.SA", record, False),
        "op_status_tcas": lambda record: get_as_verif_flag("008.Not TCAS", record, True),
        "pic": lambda record: find_value("090.PIC", record),
        "pos_lat_deg": lambda record: find_value("130.Latitude", record),
        "pos_long_deg": lambda record: find_value("130.Longitude", record),
        "report_type": lambda record: 3,
        "roll_angle_deg": lambda record: find_value("230.Roll Angle", record),
        "sac": lambda record: find_value("010.SAC", record),
        "sam": lambda record: find_value("132.MAM", record),
        "selected_alt_capability": lambda record: get_as_verif_flag("040.SAA", record, True),
        "service_id": lambda record: find_value("015.Service Identification", record),
        "sic": lambda record: find_value("010.SIC", record),
        "sil": lambda record: find_value("090.SIL", record),
        "sils": lambda record: get_as_verif_flag("090.SIL-Supplement", record, False),
        "simulated_target": lambda record: get_as_verif_flag("040.SIM", record, False),
        "spi": lambda record: get_as_verif_flag("200.SS", record, False),
        "surveillance_status": lambda record: get_surveillance_status_str(record),
        "target_addr": lambda record: find_value("080.Target Address", record),
        "target_status": lambda record: find_value("200.PS", record),
        "test_target": lambda record: get_as_verif_flag("040.TST", record, False),
        "toap": lambda record: None,
        "toav": lambda record: None,
        "tod": lambda record: find_value("071.Time of Applicability for Position", record),
        "tod2": lambda record: find_value("073.Time of Message Reception for Position", record),
        "tod_accuracy": lambda record: None,
        "tod_calculated": lambda record: None,
        "torp": lambda record: find_value("073.Time of Message Reception for Position", record),
        "torp_fsi": lambda record: find_value("074.FSI", record),
        "torp_precise": lambda record: find_value("074.Time of Message Reception of Position - high precision"),
        "torv": lambda record: find_value("075.Time of Message Reception of Velocity", record),
        "torv_fsi": lambda record: find_value("076.FSI", record),
        "torv_precise": lambda record: find_value("076.Time of Message Reception of Velocity - high precision", record),
        "track_angle_deg": lambda record: find_value("160.Track Angle", record),
        "true_airspeed_kt": lambda record: find_value("151.True Air Speed", record),
        "turnrate_degps": lambda record: find_value("165.TAR", record),
        "turnrate_indicator": lambda record: None,
        "velocity_accuracy": lambda record: None
    }

    def __init__(self, json_data):
        TargetReport.__init__(self, 21, json_data)

        self._position = GeoPosition()
        self._position.setGeoPos(self.get("pos_lat_deg"), self.get("pos_long_deg"))

        #print('pos {}'.format(self._position.getGeoPosStr()))


    def get(self, var_name):
        assert isinstance(var_name, str)
        assert var_name in ADSBTargetReport._getters
        return ADSBTargetReport._getters[var_name](self._json_data)
