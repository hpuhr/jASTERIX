#!/usr/bin/python

import sys
import json
import time

from util.record_extractor import RecordExtractor

def pretty_print(d, indent=0):
    for key, value in sorted(d.items()):
        print('\t' * indent + str(key))

        if isinstance(value, dict):
            pretty_print(value, indent+1)
        else:
            print('\t' * (indent+1) + str(value))

class DataItemStatistics:
    def __init__(self):
        self.__num_records = 0
        self.__num_records_cat = {}
        self.__data_items = {}

    @property
    def num_records(self):
        return self.__num_records

    def process_record(self, cat, record):

        self.__num_records += 1

        if cat not in self.__num_records_cat:
            self.__num_records_cat[cat] = 1
        else:
            self.__num_records_cat[cat] += 1

        if cat not in self.__data_items:
            self.__data_items[cat] = {}

        self.add_data_items(record, self.__data_items[cat])

    def add_data_items(self, record_object, counter_object):

        assert isinstance(record_object, dict)

        for key, value in record_object.items():

            if key not in counter_object:
                counter_object[key] = {}
                counter_object[key]['count'] = 1
            else:
                counter_object[key]['count'] += 1

            if isinstance(value, dict):
                self.add_data_items(value, counter_object[key])
            elif isinstance(value, (bool, int, float, str)):
                if 'min' not in counter_object[key]:
                    counter_object[key]['min'] = value
                    counter_object[key]['max'] = value
                else:
                    counter_object[key]['min'] = min(counter_object[key]['min'], value)
                    counter_object[key]['max'] = max(counter_object[key]['max'], value)

    def print(self):
        print('num records {}'.format(self.__num_records))
        print('num records per cat')
        pretty_print(self.__num_records_cat)
        print('data items')
        pretty_print(self.__data_items)

def main(argv):

    if '--framing' in sys.argv:
        framing = True
    else:
        framing = False

    print ('framing {}'.format(framing))

    num_lines = 0

    data_item_statistics = DataItemStatistics()  # type: DataItemStatistics
    record_extractor = RecordExtractor (framing, data_item_statistics.process_record)  # type: RecordExtractor

    start_time = time.time()

    for line in sys.stdin:
        #print(line)

        json_data = json.loads(line)

        record_extractor.find_records (json_data)

        #print (json.dumps(loaded_json))
        num_lines += 1

        end_time = time.time()
        print('lines {0} time {1:.2f}s rate {2} rec/s'.format(
            num_lines, end_time-start_time, int(data_item_statistics.num_records/(end_time-start_time))))

    data_item_statistics.print()


if __name__ == "__main__":
    main(sys.argv[1:])