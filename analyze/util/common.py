from typing import List, Dict


def find_value(key_location_str, json_data):

    assert isinstance(key_location_str, str)

    current_json = json_data
    for key in key_location_str.split('.'):
        if key not in current_json:
            return None
        else:
            current_json = current_json[key]
            continue

    return current_json


def invert(value):
    if value is None:
        return None

    return not value


def get_as_verif_flag(key_str, record, invert):

    value = find_value(key_str, record)

    if value is None:
        return None

    if invert:
        if value == 0:
            return 'Y'
        else:
            return 'N'
    else:
        if value == 1:
            return 'Y'
        else:
            return 'N'


def oct_to_dec (value):
    if value is None:
        return None
    return int(str(value), 8)


def multiply(value, factor):
    if value is None:
        return None
    return value * factor


def time_str_from_seconds(seconds):
    hours = int(seconds / 3600)
    minutes = int((seconds-(hours*3600))/60)
    seconds_remain = seconds-(hours*3600)-minutes*60

    return "{0}:{1}:{2:06.3f}".format(str(int(hours)).zfill(2), str(int(minutes)).zfill(2), round(seconds_remain,3))

class ValueStatistic:
    def __init__(self, name, key_location_str, value_descriptions):

        assert isinstance(name, str)
        assert isinstance(key_location_str, str)
        assert isinstance(value_descriptions, dict)

        self.__name = name
        self.__key_location_str = key_location_str  # type: str
        self.__value_descriptions = value_descriptions  # type: Dict[str, str]

        self.__num_records = 0
        self.__num_value_none = 0
        self.__num_value_not_none = 0

        self.__num_values = {}  # type: Dict[str, int]

        for value_desc in self.__value_descriptions:
            assert value_desc not in self.__num_values
            self.__num_values[value_desc] = 0

    def process_record(self, record):

        self.__num_records += 1
        value = find_value(self.__key_location_str, record)

        if value is None:
            self.__num_value_none += 1
        else:
            self.__num_value_not_none += 1

            value_str = str(value)

            assert value_str in self.__num_values
            self.__num_values[value_str] += 1

    @property
    def num_records(self):
        return self.__num_records

    def print(self, resort=False, parent_num_records=None):

        if parent_num_records is None:
            print('\n{}:'.format(self.__name))
        else:
            print('\n{0} records {1} ({2:.2f}%):'.format(self.__name,
                                                         self.__num_records,
                                                         100.0 * self.__num_records / parent_num_records))

        #print('\trecords: {}'.format(self.__num_records))
        print('\tnone: {0} ({1:.2f}%)'.format(self.__num_value_none,
                                              100.0*self.__num_value_none/self.__num_records))

        if resort:
            for value_str, value_cnt in sorted(self.__num_values.items()):
                if value_cnt == 0:
                    continue
                print('\t{0}: {1} ({2:.2f}%)'.format(self.__value_descriptions[value_str],
                                                     value_cnt,
                                                     100.0 * value_cnt / self.__num_value_not_none))
        else:
            for value_str, value_cnt in self.__num_values.items():
                if value_cnt == 0:
                    continue
                print('\t{0}: {1} ({2:.2f}%)'.format(self.__value_descriptions[value_str],
                                                     value_cnt,
                                                     100.0 * value_cnt / self.__num_value_not_none))


class ValueStatisticList:
    def __init__(self, name, list_key_location_str, list_value_descriptions, key_location_str, value_descriptions):

        assert isinstance(name, str)
        assert isinstance(list_key_location_str, str)
        assert isinstance(list_value_descriptions, dict)
        assert isinstance(key_location_str, str)
        assert isinstance(value_descriptions, dict)

        self.__name = name
        self.__list_key_location_str = list_key_location_str
        self.__list_value_descriptions = list_value_descriptions  # type: Dict[str, str] # cam be empty
        self.__key_location_str = key_location_str
        self.__value_descriptions = value_descriptions  # type: Dict[str, str]

        self.__num_records = 0
        self.__num_list_key_none = 0
        self.__num_list_key_not_none = 0

        self.__value_statistics = {}  # type: Dict[str, ValueStatistic]

    def process_record(self, record):
        self.__num_records += 1
        list_key = find_value(self.__list_key_location_str, record)

        list_key_str = 'None'

        if list_key is None:
            self.__num_list_key_none += 1
        else:
            self.__num_list_key_not_none += 1
            list_key_str = str(list_key)

            if list_key_str in self.__list_value_descriptions:
                list_key_str = self.__list_value_descriptions[list_key_str]

        if list_key_str not in self.__value_statistics:
            self.__value_statistics[list_key_str] = ValueStatistic(self.__name+': \''+list_key_str+'\'',
                                                                   self.__key_location_str,
                                                                   self.__value_descriptions)

        self.__value_statistics[list_key_str].process_record(record)

    @property
    def num_records(self):
        return self.__num_records

    def print(self, resort=False):
        print('\n{}:'.format(self.__name))

        for list_key, statistic in sorted(self.__value_statistics.items()):
            statistic.print(resort, self.__num_records)
