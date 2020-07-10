import sys
import json
import time
from typing import Dict
from collections import defaultdict
import argparse

from util.record_extractor import RecordExtractor
from util.common import find_value, time_str_from_seconds
import util.track

def filter_records(cat, record):
    if cat != 21 and cat != 62:
        return True

    return False

global_first_tod = None
global_last_tod = None


class ModeSChain:
    def __init__(self, utn, target_address):
        self._utn = utn
        self._target_address = target_address
        self._num_records = 0

        self._first_tod = None
        self._last_tod = None

        #self._mode_a_codes = {}
        #self._target_addresses = {}
        #self._target_identifications = {}


class ADSBModeSChain(ModeSChain):
    def __init__(self, utn, target_address):
        ModeSChain.__init__(self, utn, target_address)

        self._target_reports = {}  # type: Dict[float,obj]  # tod -> json

    def process_record(self, record):

        self._num_records += 1

        # process time
        tod = find_value("073.Time of Message Reception for Position", record) * 128.0

        self._target_reports[tod] = record

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


class ChainCalculator:
    def __init__(self):
        self.__num_records = 0

        self._utn_cnt = 0
        self._ta2utn = {}  # type: Dict[int,int]  # ta -> utn

        self._adsb_chains = {}  # type: Dict[int,ModeSChain]  # utn -> chain
        #self._adsb_getters = {}
        #self._adsb_getters["target_addr"] = lambda record: find_value("080.Target Address", record)


    @property
    def num_records(self):
        return self.__num_records

    def process_record(self, cat, record):

        self.__num_records += 1

        if cat == 21:
            self.process_adsb_record(record)
        else:
            assert False

    def process_adsb_record(self, record):

        target_addr = find_value("080.Target Address", record)
        assert target_addr is not None

        if target_addr not in self._ta2utn:
            utn = self._utn_cnt
            self._utn_cnt += 1

            self._ta2utn[target_addr] = utn
        else:
            utn = self._ta2utn[target_addr]

        if utn not in self._adsb_chains:
            #print('ChainCalculator: new ads-b chain utn {} ta {}'.format(utn, hex(target_addr)))
            self._adsb_chains[utn] = ADSBModeSChain(utn, target_addr)

        chain = self._adsb_chains[utn]
        assert isinstance(chain, ADSBModeSChain)

        chain.process_record(record)

        # tod = find_value("070.Time Of Track Information", record)
        # assert tod is not None
        #
        # global global_first_tod
        # global global_last_tod
        #
        # if global_first_tod is None:
        #     global_first_tod = tod
        #     global_last_tod = tod
        #
        # if tod > global_last_tod:
        #     global_last_tod = tod
        #
        # track_num = find_value("040.Track Number", record)
        # assert track_num is not None
        #
        # track_begin = find_value("080.TSB", record)
        # assert track_begin is not None
        #
        # track_end = find_value("080.TSE", record)
        # assert track_end is not None
        #
        # if track_num not in self._tn2utn_map:  # does not exist yet
        #     current_utn = self._utn_cnt  # create new utn
        #     self._utn_cnt += 1
        #
        #     self._tn2utn_map[track_num] = current_utn  # put in mapping
        #
        #     self._active_targets[current_utn] = ModeSChain(current_utn, track_num)
        #
        # utn = self._tn2utn_map[track_num]
        # assert utn in self._active_targets
        #
        # self._active_targets[utn].process_record(cat, record)
        #
        # if track_end == 1:
        #     self._finalized_targets[utn] = self._active_targets[utn]
        #     del self._active_targets[utn]
        #     del self._tn2utn_map[track_num]


def main(argv):

    parser = argparse.ArgumentParser(description='ASTERIX CAT062 dubious tracks')
    parser.add_argument('--framing', help='Framing True or False', required=True)

    args = parser.parse_args()

    assert args.framing is not None
    assert args.framing == 'True' or args.framing == 'False'
    framing = args.framing == 'True'

    print('framing {}'.format(framing))

    num_blocks = 0

    chain_calc = ChainCalculator()  # type: ChainCalculator

    record_extractor = RecordExtractor(framing, chain_calc.process_record, filter_records)  # type: RecordExtractor

    start_time = time.time()

    for line in sys.stdin:
        #print(line)

        json_data = json.loads(line)

        record_extractor.find_records (json_data)

        num_blocks += 1

        end_time = time.time()
        print('blocks {0} time {1:.2f}s unfiltered records {2} filtered {3} rate {4} rec/s'.format(
            num_blocks, end_time-start_time, chain_calc.num_records, record_extractor.num_filtered,
            int(record_extractor.num_records/(end_time-start_time))))

    #chain_calc.finalize()
    #chain_calc.print()


if __name__ == "__main__":
    main(sys.argv[1:])