
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
        self.sac = None
        self.sic = None
        self.target_addr = None
        self.modec_code_ft = None
        self.groundspeed_kt = None
        self.heading_deg = None

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
        self.modec_code_ft = target_report.get("mode3a_code")
        self.groundspeed_kt = target_report.get("groundspeed_kt")
        self.heading_deg = target_report.get("track_angle_deg")


