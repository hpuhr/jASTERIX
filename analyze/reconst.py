import sys
import json
import time
from typing import Dict
from collections import defaultdict
import argparse
import time

from util.record_extractor import RecordExtractor
from util.common import find_value, time_str_from_seconds
from reconst_util.chain import ADSBModeSChain
from reconst_util.reconstructed_chain import ReconstructedADSBModeSChain
from reconst_util.umkalman2d import UMKalmanFilter2D
from reconst_util.multidict import OrderedMultiDict

def filter_records(cat, record):
    if cat != 21 and cat != 62:
        return True

    #target_addr = find_value("080.Target Address", record)
    #assert target_addr is not None
    #return target_addr != 3957892 and target_addr != 4344286 # first
    #return target_addr != 4344286 # missing tr

    return False


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

def check_usable_ADSB_chain (adsb_chain):
    assert isinstance(adsb_chain, ADSBModeSChain)

    usable = True
    not_usable_reasons = []

    mops_versions = adsb_chain.getMOPSVersions()

    if len(mops_versions) == 0:
        usable = False
        not_usable_reasons.append('No MOPS version')

    for unusable_mops_version in [None, 0, 1]:
        if unusable_mops_version in mops_versions:
            usable = False
            not_usable_reasons.append('MOPS version {} found'.format(unusable_mops_version))

    return usable, not_usable_reasons

def check_usable_reconstructed_chain (rec_chain):
    assert isinstance(rec_chain, ReconstructedADSBModeSChain)

    usable = True
    not_usable_reasons = []

    max_stddev = rec_chain.getMaxSmoothedStdDev()

    #print('chain utn {} ta {} max stddev {}'.format(rec_chain._utn, hex(rec_chain._target_address), max_stddev))

    if max_stddev == -1:
        usable = False
        not_usable_reasons.append('No StdDev found')

    if max_stddev > 25:
        usable = False
        not_usable_reasons.append('Max Smoothed StdDev {}'.format(max_stddev))

    return usable, not_usable_reasons

def main(argv):
    start_time = time.time()

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

        input_json_data = json.loads(line)

        record_extractor.find_records(input_json_data)

        num_blocks += 1

        end_time = time.time()
        print('blocks {0} time {1:.2f}s unfiltered records {2} filtered {3} rate {4} rec/s'.format(
            num_blocks, end_time-start_time, chain_calc.num_records, record_extractor.num_filtered,
            int(record_extractor.num_records/(end_time-start_time))))

    print('got {} ads-b chains'.format(len(chain_calc.adsb_chains)))

    reconst_filter = UMKalmanFilter2D('UMKalmanFilter2D')

    #org_json_data = []
    fil_json_data = []
    smo_json_data = []

    usable_chains_cnt = 0
    not_usable_chains_cnt = 0

    usable_tr_cnt = 0
    not_usable_tr_cnt = 0

    for ta, adsb_chain in chain_calc.adsb_chains.items():  # type: int,ADSBModeSChain
        # check if usable

        usable, not_usable_reasons = check_usable_ADSB_chain(adsb_chain)

        if not usable:
            print('chain utn {} ta {} not usable since {}'.format(adsb_chain.utn, hex(adsb_chain.target_address), not_usable_reasons))
            not_usable_chains_cnt += 1
            not_usable_tr_cnt += len(adsb_chain.target_reports)
            continue

        # filter
        reconstructed = reconst_filter.filter(adsb_chain)  # type: ReconstructedADSBModeSChain

        #reconstructed.plot()

        usable, not_usable_reasons = check_usable_reconstructed_chain(reconstructed)

        if not usable:
            print('chain utn {} ta {} not usable since {}'.format(adsb_chain.utn, hex(adsb_chain.target_address), not_usable_reasons))
            not_usable_chains_cnt += 1
            not_usable_tr_cnt += len(adsb_chain.target_reports)
            continue
        else:
            print('chain utn {} ta {} usable'.format(adsb_chain.utn, hex(adsb_chain.target_address)))
            usable_chains_cnt += 1
            usable_tr_cnt += len(adsb_chain.target_reports)

        #reconstructed.addOriginalAsJSON(org_json_data)
        #reconstructed.addFiltereddAsJSON(fil_json_data)
        reconstructed.addSmoothedAsJSON(smo_json_data)

    chains_cnt = len(chain_calc.adsb_chains)

    print('num chains {} usable {} ({:.2%}) unusable {} ({:.2%})'.format(
        chains_cnt, usable_chains_cnt, usable_chains_cnt/chains_cnt,
        not_usable_chains_cnt, not_usable_chains_cnt/chains_cnt))

    tr_cnt = usable_tr_cnt+not_usable_tr_cnt
    print('num target reports {} usable {} ({:.2%}) unusable {} ({:.2%})'.format(
        tr_cnt, usable_tr_cnt, usable_tr_cnt/tr_cnt,
        not_usable_tr_cnt, not_usable_tr_cnt/tr_cnt))

    #org_json_data.sort(key=lambda x: x['073']['Time of Message Reception for Position'])
    #fil_json_data.sort(key=lambda x: x['tod'])
    smo_json_data.sort(key=lambda x: x['tod'])

    #print('UGA len {} {} {} {}'.format(len(chain_calc.adsb_chains), len(org_json_data), len(fil_json_data), len(smo_json_data)))
    #assert len(org_json_data)-len(chain_calc.adsb_chains) == len(fil_json_data) == len(smo_json_data)

    export_json = {}
    export_json['data_blocks'] = []

    #adsb_data_block = {}
    #adsb_data_block['category'] = 21
    #adsb_data_block['content'] = {}
    #adsb_data_block['content']['records'] = org_json_data

    #export_json['data_blocks'].append(adsb_data_block)

    #fil_data_block = {}
    #fil_data_block['category'] = 255
    #fil_data_block['content'] = {}
    #fil_data_block['content']['records'] = fil_json_data

    #export_json['data_blocks'].append(fil_data_block)

    smo_data_block = {}
    smo_data_block['category'] = 255
    smo_data_block['content'] = {}
    smo_data_block['content']['records'] = smo_json_data

    export_json['data_blocks'].append(smo_data_block)

    with open('reconst.json', 'w') as outfile:
        json.dump(export_json, outfile, indent=4)

    print('done after {}'.format(time_str_from_seconds(time.time() - start_time)))


if __name__ == "__main__":
    main(sys.argv[1:])
