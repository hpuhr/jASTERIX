from util.common import find_value, time_str_from_seconds
from reconst_util.target_report import ADSBTargetReport

class ModeSChain:
    def __init__(self, utn, target_address):
        self._utn = utn
        self._target_address = target_address
        self._num_records = 0

        self._first_tod = None
        self._last_tod = None

    @property
    def utn(self):
        return self._utn

    @property
    def target_address(self):
        return self._target_address

class ADSBModeSChain(ModeSChain):
    def __init__(self, utn, target_address):
        ModeSChain.__init__(self, utn, target_address)

        self._target_reports = {}  # type: Dict[float, ADSBTargetReport]  # tod -> TargetReport wrapping json

    @property
    def target_reports(self):
        return self._target_reports

    def process_record(self, cat, record):

        assert cat == 21

        self._num_records += 1

        # process time
        tod = find_value("073.Time of Message Reception for Position", record)

        self._target_reports[tod] = ADSBTargetReport(record)

        # tod = find_value("070.Time Of Track Information", record)
        # assert tod is not None
        #
        if self._first_tod is None:
            self._first_tod = tod
            self._last_tod = tod

        if tod > self._last_tod:
            self._last_tod = tod
        elif tod < self._last_tod:
            print('warn: utn {} ta {} record {} time jump from {} to {} diff {}'.format(
                self._utn, hex(self._target_address), self._num_records,
                time_str_from_seconds(self._last_tod), time_str_from_seconds(tod),
                time_str_from_seconds(self._last_tod-tod)))