#!/usr/bin/python

import sys
import json
import time
from typing import Dict
import argparse

from util.record_extractor import RecordExtractor
from util.common import find_value

def print_func(cat, record):
    print (json.dumps(record, indent=4))


# filter functions return True if record should be skipped
def filter_func(cat, record):
    if cat != 62:
        return True

    #return find_value("380.ADR.Target Address", record) != int("8964f9", 16)
    return find_value("040.Track Number", record) != 4592

def main(argv):

    parser = argparse.ArgumentParser(description='ASTERIX data item analysis')
    parser.add_argument('--framing', help='Framing True or False', required=True)

    args = parser.parse_args()

    assert args.framing is not None
    assert args.framing == 'True' or args.framing == 'False'
    framing = args.framing == 'True'

    print('framing {}'.format(framing))

    num_blocks = 0

    record_extractor = RecordExtractor(framing, print_func, filter_func)  # type: RecordExtractor


    start_time = time.time()

    for line in sys.stdin:
        #print(line)

        json_data = json.loads(line)

        record_extractor.find_records (json_data)

        num_blocks += 1

        end_time = time.time()


if __name__ == "__main__":
    main(sys.argv[1:])