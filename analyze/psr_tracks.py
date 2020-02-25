#!/usr/bin/python

import sys
import json
import time
from typing import Dict
import argparse

from util.record_extractor import RecordExtractor
from util.common import find_value


class TrackStatisticsCalculator:
    def __init__(self):
        self.__num_records = 0
        self.__num_psr_records = 0

    @property
    def num_records(self):
        return self.__num_records

    @property
    def um_psr_records(self):
        return self.__num_psr_records

    def process_record(self, cat, record):

        self.__num_records += 1

        if self.is_psr_track_update(record):
            self.__num_psr_records += 1

    def is_psr_track_update (self, record):
        # '062.080' count 1214471 (100%)
        # '062.080.AAC' count 1214471 (100%) min '0' max '0'
        # '062.080.ADS' count 1214471 (100%) min '1' max '1'
        # '062.080.AFF' count 1214471 (100%) min '0' max '0'
        # '062.080.AMA' count 1214471 (100%) min '0' max '0'
        # '062.080.CNF' count 1214471 (100%) min '0' max '1'
        # '062.080.CST' count 1214471 (100%) min '0' max '1'
        # '062.080.FPC' count 1214471 (100%) min '0' max '0'
        # '062.080.FX' count 1214471 (100%) min '1' max '1'
        # '062.080.FX2' count 1214471 (100%) min '1' max '1'
        # '062.080.FX3' count 1214471 (100%) min '1' max '1'
        # '062.080.FX4' count 1214471 (100%) min '0' max '0'
        # '062.080.KOS' count 1214471 (100%) min '0' max '1'
        # '062.080.MD4' count 1214471 (100%) min '0' max '0'
        # '062.080.MD5' count 1214471 (100%) min '0' max '0'
        # '062.080.MDS' count 1214471 (100%) min '0' max '1'
        # '062.080.ME' count 1214471 (100%) min '0' max '0'
        # '062.080.MI' count 1214471 (100%) min '0' max '0'
        # '062.080.MON' count 1214471 (100%) min '0' max '1'
        # '062.080.MRH' count 1214471 (100%) min '0' max '1'
        # '062.080.PSR' count 1214471 (100%) min '0' max '1'
        # '062.080.SIM' count 1214471 (100%) min '0' max '0'
        # '062.080.SPI' count 1214471 (100%) min '0' max '1'
        # '062.080.SRC' count 1214471 (100%) min '3' max '6'
        # '062.080.SSR' count 1214471 (100%) min '0' max '1'
        # '062.080.STP' count 1214471 (100%) min '0' max '0'
        # '062.080.SUC' count 1214471 (100%) min '0' max '0'
        # '062.080.TSB' count 1214471 (100%) min '0' max '1'
        # '062.080.TSE' count 1214471 (100%) min '0' max '1'

        if find_value(["080", "TSB"], record) != 0:  # first track message
            return False

        if find_value(["080", "TSE"], record) != 0:  # end track
            return False

        if find_value(["080", "MON"], record) != 1:
            return False

        # if find_value(["080", "CST"], record) != 0:
        #    return False

        if find_value(["080", "PSR"], record) != 0:
            return False

        if find_value(["080", "SSR"], record) != 1:
            return False

        if find_value(["080", "MDS"], record) != 1:
            return False

        if find_value(["080", "ADS"], record) != 1:
            return False

        return True

    def print(self):
        print('num cat062 records {}'.format(self.__num_records))
        print('num psr track update records {}'.format(self.__num_psr_records))


# filter functions return True if record should be skipped
def filter_psr_tracks(cat, record):
    if cat != 62:
        return True

def main(argv):

    parser = argparse.ArgumentParser(description='ASTERIX data item analysis')
    parser.add_argument('--framing', help='Framing True or False', required=True)

    args = parser.parse_args()

    assert args.framing is not None
    framing = args.framing

    print('framing {}'.format(framing))

    num_blocks = 0

    statistics_calc = TrackStatisticsCalculator()  # type: TrackStatisticsCalculator

    record_extractor = RecordExtractor(framing, statistics_calc.process_record, filter_psr_tracks)  # type: RecordExtractor

    start_time = time.time()

    for line in sys.stdin:
        #print(line)

        json_data = json.loads(line)

        record_extractor.find_records (json_data)

        num_blocks += 1

        end_time = time.time()
        print('blocks {0} time {1:.2f}s unfiltered records {2} filtered {3} rate {4} rec/s'.format(
            num_blocks, end_time-start_time, statistics_calc.num_records,
            record_extractor.num_filtered,
            int((record_extractor.num_records)/(end_time-start_time))))

    statistics_calc.print()


if __name__ == "__main__":
    main(sys.argv[1:])