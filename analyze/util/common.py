from typing import List

def find_value(key_location, json_data):
    assert isinstance(key_location, List)

    current_json = json_data
    for key in key_location:
        if key not in current_json:
            return None
        else:
            current_json = current_json[key]
            continue

    return current_json


class ValueStatistic:
    def __init__(self, name, key_location, value_descriptions):
        self.__name = name
        self.__key_location = key_location  # type: List[str]
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
        value = find_value(self.__key_location, record)

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
    def __init__(self, name, list_key_location, list_value_descriptions, key_location, value_descriptions):
        self.__name = name
        self.__list_key_location = list_key_location  # type: List[str]
        self.__list_value_descriptions = list_value_descriptions  # type: Dict[str, str] # cam be empty
        self.__key_location = key_location  # type: List[str]
        self.__value_descriptions = value_descriptions  # type: Dict[str, str]

        self.__num_records = 0
        self.__num_list_key_none = 0
        self.__num_list_key_not_none = 0

        self.__value_statistics = {}  # type: Dict[str, ValueStatistic]

    def process_record(self, record):
        self.__num_records += 1
        list_key = find_value(self.__list_key_location, record)

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
                                                                   self.__key_location,
                                                                   self.__value_descriptions)

        self.__value_statistics[list_key_str].process_record(record)

    @property
    def num_records(self):
        return self.__num_records

    def print(self, resort=False):
        print('\n{}:'.format(self.__name))

        for list_key, statistic in sorted(self.__value_statistics.items()):
            statistic.print(resort, self.__num_records)
