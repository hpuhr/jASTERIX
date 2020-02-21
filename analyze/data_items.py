#!/usr/bin/python

import sys
import json
import time
from typing import Dict

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
    def __init__(self):
        self.__num_records = 0
        self.__statistics = {} # type: Dict[str, DataItemStatistic]

    @property
    def num_records(self):
        return self.__num_records

    def process_record(self, cat, record):

        self.__num_records += 1

        cat_str = str(cat).zfill(3)

        if cat_str not in self.__statistics:
            self.__statistics[cat_str] = DataItemStatistic(cat_str)

        self.__statistics[cat_str].process_object(record)

    def print(self):
        print('num records {}'.format(self.__num_records))

        print('data items')
        for cat, stat in sorted(self.__statistics.items()):
            print()
            stat.print()


# filter functions return True if record should be skipped
def filter_cat(cat, record):
    return cat != 21


def filter_cat21(cat, record):
    if cat != 21:
        return True
    return find_value(["090", "NACp"], record) is None


def main(argv):

    if '--framing' in sys.argv:
        framing = True
    else:
        framing = False

    print ('framing {}'.format(framing))

    num_blocks = 0

    statistics_calc = DataItemStatisticsCalculator()  # type: DataItemStatisticsCalculator

    # without filtering
    #record_extractor = RecordExtractor(framing, statistics_calc.process_record)  # type: RecordExtractor
    # with filtering function
    record_extractor = RecordExtractor (framing, statistics_calc.process_record, filter_cat)  # type: RecordExtractor
    # with filtering lambda
    #record_extractor = RecordExtractor(framing, statistics_calc.process_record, lambda cat, record: cat != 21)  # type: RecordExtractor


    start_time = time.time()

    for line in sys.stdin:
        #print(line)

        json_data = json.loads(line)

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