import matplotlib.pyplot as plt

import json

from reconst_util.target_report import ADSBTargetReport
from reconst_util.reference_update import ReferenceUpdate

class ReconstructedADSBModeSChain:
    def __init__(self, utn, target_address):
        self._utn = utn
        self._target_address = target_address
        self._num_records = 0

        self._target_reports = {}  # type: Dict[float, ADSBTargetReport]  # tod -> TargetReport wrapping json
        self._filtered_target_reports = {}  # type: Dict[float, ReferenceUpdate]
        self._smoothed_target_reports = {}  # type: Dict[float, ReferenceUpdate]

    @property
    def target_reports(self):
        return self._target_reports

    @target_reports.setter
    def target_reports(self, val):
        self._target_reports = val

    @property
    def filtered_target_reports(self):
        return self._filtered_target_reports

    @filtered_target_reports.setter
    def filtered_target_reports(self, val):
        self._filtered_target_reports = val

    @property
    def smoothed_target_reports(self):
        return self._smoothed_target_reports

    @smoothed_target_reports.setter
    def smoothed_target_reports(self, val):
        self._smoothed_target_reports = val

    def plot(self):
        org_lat = [tr.position.getGeoLatitude() for tod, tr in self._target_reports.items()]
        org_long = [tr.position.getGeoLongitude() for tod, tr in self._target_reports.items()]
        plt.plot(org_long, org_lat, 'b.-', label='org')  #color='#0000FF'

        fil_lat = [tr.pos_lat_deg for tod, tr in self._filtered_target_reports.items()]
        fil_long = [tr.pos_long_deg for tod, tr in self._filtered_target_reports.items()]
        plt.plot(fil_long, fil_lat, 'g.-', label='fil')  #color='#0000FF'

        smo_lat = [tr.pos_lat_deg for tod, tr in self._smoothed_target_reports.items()]
        smo_long = [tr.pos_long_deg for tod, tr in self._smoothed_target_reports.items()]
        plt.plot(smo_long, smo_lat, 'r.-', label='smo')  #color='#0000FF'

        plt.grid()
        plt.legend(loc='upper right')
        plt.show()

    def addOriginalAsJSON(self, json_data):
        # original target reports
        for tod, target_report in self._target_reports.items():  # type: float,ADSBTargetReport
            json_data.append(target_report.json_data)

    def addFiltereddAsJSON(self, json_data):
        for tod, target_report in self._filtered_target_reports.items():  # type: float,ReferenceUpdate
            json_data.append(target_report.asJSON())

    def addSmoothedAsJSON(self, json_data):
        for tod, target_report in self._smoothed_target_reports.items():  # type: float,ReferenceUpdate
            json_data.append(target_report.asJSON())

