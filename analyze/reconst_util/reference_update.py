
from util.common import find_value, time_str_from_seconds
from reconst_util.target_report import ADSBTargetReport

class ReferenceUpdate(object):
    def __init__(self, utn, tod):
        self.utn = utn  # track_num
        self.tod = tod

        self.callsign = None
        self.mode3a_code = None
        self.pos_lat_deg = None
        self.pos_long_deg = None
        self.pos_std_dev_lat_deg = None
        self.pos_std_dev_latlong_corr_coeff = None
        self.pos_std_dev_long_deg = None
        self.sac = None
        self.sic = None
        self.target_addr = None
        self.modec_code_ft = None
        self.groundspeed_kt = None
        self.heading_deg = None

        self.pos_std_dev_x_m = None
        self.pos_std_dev_xy_corr_coeff = None
        self.pos_std_dev_y_m = None


    def fromADSBTargetReport (self, target_report):
        assert isinstance(target_report, ADSBTargetReport)
        assert self.tod == target_report.get("tod")

        self.callsign = target_report.get("callsign")
        self.mode3a_code = target_report.get("mode3a_code")
        self.pos_lat_deg = target_report.get("pos_lat_deg")
        self.pos_long_deg = target_report.get("pos_long_deg")
        #self.sac = target_report.get("sac")
        #self.sic = target_report.get("saic")
        self.target_addr = target_report.get("target_addr")
        self.modec_code_ft = target_report.get("alt_baro_ft")
        self.groundspeed_kt = target_report.get("groundspeed_kt")
        self.heading_deg = target_report.get("track_angle_deg")

    def asJSON (self):
        data = {}

        data['tod'] = self.tod
        data['callsign'] = self.callsign
        data['mode3a_code'] = self.mode3a_code
        data['pos_lat_deg'] = self.pos_lat_deg
        data['pos_long_deg'] = self.pos_long_deg
        data['pos_std_dev_lat_deg'] = self.pos_std_dev_lat_deg
        data['pos_std_dev_latlong_corr_coeff'] = self.pos_std_dev_latlong_corr_coeff
        data['pos_std_dev_long_deg'] = self.pos_std_dev_long_deg
        data['sac'] = self.sac
        data['sic'] = self.sic
        data['ds_id']  = self.sac * 255 + self.sic
        data['target_addr'] = self.target_addr
        data['modec_code_ft'] = self.modec_code_ft
        data['groundspeed_kt'] = self.groundspeed_kt
        data['heading_deg'] = self.heading_deg

        return data


