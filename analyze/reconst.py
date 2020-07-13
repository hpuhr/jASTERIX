import sys
import json
import time
from typing import Dict
from collections import defaultdict
import argparse

from util.record_extractor import RecordExtractor
from util.common import find_value, time_str_from_seconds
from reconst_util.chain import ADSBModeSChain
from reconst_util.umkalman2d import UMKalmanFilter2D

def filter_records(cat, record):
    if cat != 21 and cat != 62:
        return True

    target_addr = find_value("080.Target Address", record)
    assert target_addr is not None
    return target_addr != 3957892

    #return False


class ChainCalculator:
    def __init__(self):
        self.__num_records = 0

        self._utn_cnt = 0
        self._ta2utn = {}  # type: Dict[int,int]  # ta -> utn

        self._adsb_chains = {}  # type: Dict[int,ADSBModeSChain]  # utn -> chain
        #self._adsb_getters = {}
        #self._adsb_getters["target_addr"] = lambda record: find_value("080.Target Address", record)


    @property
    def num_records(self):
        return self.__num_records

    @property
    def adsb_chains(self):
        return self._adsb_chains

    def process_record(self, cat, record):

        self.__num_records += 1

        if cat == 21:
            self.process_adsb_record(cat, record)
        else:
            assert False

    def process_adsb_record(self, cat, record):

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

        chain.process_record(cat, record)


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

        record_extractor.find_records(json_data)

        num_blocks += 1

        end_time = time.time()
        print('blocks {0} time {1:.2f}s unfiltered records {2} filtered {3} rate {4} rec/s'.format(
            num_blocks, end_time-start_time, chain_calc.num_records, record_extractor.num_filtered,
            int(record_extractor.num_records/(end_time-start_time))))

    print('got {} ads-b chains'.format(len(chain_calc.adsb_chains)))

    reconst_filter = UMKalmanFilter2D('UMKalmanFilter2D')

    for ta, adsb_chain in chain_calc.adsb_chains.items():  # type: int,ADSBModeSChain
        reconstructed = reconst_filter.filter(adsb_chain)
        reconstructed.plot()


if __name__ == "__main__":
    main(sys.argv[1:])