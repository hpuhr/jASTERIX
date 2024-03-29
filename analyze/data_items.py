#!/usr/bin/python3

import sys
import json
import time
from typing import Dict
import argparse

from util.record_extractor import RecordExtractor
from util.common import find_value


class DataItemValueStatistic:
    def __init__(self, key_name):
        self.key_name = key_name
        self.count = 0
        self.min = None
        self.max = None

    def process_value(self, value):

        #print ('process_value: key {} min {} max {} value {}'.format(self.key_name , self.min, self.max, value))

        self.count += 1

        if self.min is None:
            self.min = value
            self.max = value
        else:
            self.min = min(self.min, value)
            self.max = max(self.max, value)

    def print(self, count_parent):
        print('\'{}\' count {} ({}%) min \'{}\' max \'{}\''.format(self.key_name, self.count,
                                                                   int(100.0*self.count/count_parent), self.min, self.max))


class DataItemStatistic:
    def __init__(self, item_name):
        self.item_name = item_name
        self.count = 0
        self.sub_statistics = {}  # type: Dict[str, DataItemStatistic]
        self.value_statistics = {} # type: Dict[str, DataItemValueStatistic]

    def process_object(self, record_object):
        assert isinstance(record_object, dict)

        self.count += 1

        for key, value in record_object.items():

            if isinstance(value, dict):

                if key not in self.sub_statistics:
                    self.sub_statistics[key] = DataItemStatistic(self.item_name + '.' + key)

                self.sub_statistics[key].process_object(value)

            elif isinstance(value, (bool, int, float, str)):
                if key not in self.value_statistics:
                    self.value_statistics[key] = DataItemValueStatistic(self.item_name + '.' + key)

                self.value_statistics[key].process_value(value)

    def print(self, count_parent=None):

        if count_parent is None:
            print('\'{}\' count {}'.format(self.item_name, self.count))
        else:
            print('\'{}\' count {} ({}%)'.format(self.item_name, self.count, int(100.0 * self.count / count_parent)))

        for sub_name, sub_stat in sorted(self.sub_statistics.items()):
            sub_stat.print(self.count)

        for val_name, val_stat in sorted(self.value_statistics.items()):
            val_stat.print(self.count)

class DataItemStatisticsCalculator:
    def __init__(self, per_source):
        self._per_source = per_source
        self.__num_records = 0
        self.__statistics = {}  # type: Dict[(int, int), Dict[str, DataItemStatistic]]

    @property
    def num_records(self):
        return self.__num_records

    def process_record(self, cat, record):

        di010 = None
        di010 = find_value('010', record)

        if di010 is None:
            print("di 010 missing in '{}'".format(record))

        sac = None
        sic = None

        if self._per_source:
            sac = find_value('010.SAC', record)
            sic = find_value('010.SIC', record)

            assert sac is not None and sic is not None

        self.__num_records += 1

        cat_str = str(cat).zfill(3)

        if (sac, sic) not in self.__statistics:
            self.__statistics[(sac, sic)] = {}

        if cat_str not in self.__statistics[(sac, sic)]:
            self.__statistics[(sac, sic)][cat_str] = DataItemStatistic(cat_str)

        self.__statistics[(sac, sic)][cat_str].process_object(record)

    def print(self):
        print('num records {}'.format(self.__num_records))

        for (sac, sic), stat_dict in sorted(self.__statistics.items()):

            if self._per_source:
                print('data items for {}/{}'.format(sac, sic))
            else:
                print('data items')

            for cat, stat in sorted(stat_dict.items()):
                print()
                stat.print()
                print('\n\n')


# filter functions return True if record should be skipped
def filter_example(cat, record):
    if cat != 21:
        return True
    return find_value("090.NACp", record) is None


cat_list = None


def filter_cats(cat, record):
    global cat_list

    return cat not in cat_list

def main(argv):

    parser = argparse.ArgumentParser(description='ASTERIX data item analysis')
    parser.add_argument('--framing', help='Framing', default=False, action='store_true', required=False)
    parser.add_argument('--cats', help='ASTERIX categories to be analyzed as CSV', required=False)
    parser.add_argument('--per_source', help='Whether to do analysis per SAC/SIC', default=False, action='store_true', required=False)

    args = parser.parse_args()

    #assert args.framing is not None
    #assert args.framing == 'True' or args.framing == 'False'
    print('framing {} '.format(args.framing))

    global cat_list
    if args.cats is not None:
        cat_list = args.cats.split(",")
        cat_list = [int(i) for i in cat_list]

    print('cats {}'.format(cat_list))
    print('per-source {} '.format(args.per_source))

    num_blocks = 0

    statistics_calc = DataItemStatisticsCalculator(args.per_source)  # type: DataItemStatisticsCalculator

    if cat_list is None:  # without filtering
        record_extractor = RecordExtractor (args.framing, statistics_calc.process_record)  # type: RecordExtractor
    else:  # with filtering lambda
        record_extractor = RecordExtractor(args.framing, statistics_calc.process_record, filter_cats)  # type: RecordExtractor

    start_time = time.time()

    for line in sys.stdin:
        #print(line)

        try:
            json_data = json.loads(line)
        except:
            print("`exception occurred in json '{}'".format(line))
            continue

        record_extractor.find_records (json_data)

        num_blocks += 1

        end_time = time.time()
        print('blocks {0} time {1:.2f}s records {2} filtered {3} rate {4} rec/s'.format(
            num_blocks, end_time-start_time, statistics_calc.num_records,
            record_extractor.num_filtered,
            int((statistics_calc.num_records+record_extractor.num_filtered)/(end_time-start_time))))

    statistics_calc.print()


if __name__ == "__main__":
    main(sys.argv[1:])
