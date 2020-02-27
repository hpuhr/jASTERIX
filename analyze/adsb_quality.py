#!/usr/bin/python

import sys
import json
import time
import argparse
from typing import List, Dict

from util.record_extractor import RecordExtractor
from util.common import find_value, ValueStatistic, ValueStatisticList

class ADSBQualityStatisticsCalculator:
    def __init__(self):
        self.__num_records = 0

        self.__value_statistics = []  # type: List[ValueStatistic]

        # (VNS) : Version Not Supported
        self.__value_statistics.append(ValueStatistic('MOPS Version is supported by the GS',
                                                      "210.VNS",
                                                      {"0": "MOPS Version is supported by the GS",
                                                       "1": "MOPS Version is not supported by the GS"}))

        # (VN) : Version Number
        mops_vn_key_str = "210.VN"
        mops_vn_value_descriptions = {"0": "ED102/DO-260",
                                      "1": "DO-260A",
                                      "2": "ED102A/DO-260B"}
        self.__value_statistics.append(ValueStatistic('MOPS Version Number',
                                                      mops_vn_key_str, mops_vn_value_descriptions))

        # Emitter Category
        ecat_key_str = "020.ECAT"
        ecat_value_descriptions = {"0": "No ADS-B Emitter Category Information",
                                   "1": "light aircraft <= 15500 lbs",
                                   "2": "15500 lbs < small aircraft <75000 lbs",
                                   "3": "75000 lbs < medium a/c < 300000 lbs",
                                   "4": "High Vortex Large",
                                   "5": "300000 lbs <= heavy aircraft",
                                   "6": "highly manoeuvrable (5g acceleration capability) and high speed (>400 knots cruise)",
                                   "7": "reserved",
                                   "8": "reserved",
                                   "9": "reserved",
                                   "10": "rotocraft",
                                   "11": "glider / sailplane",
                                   "12": "lighter-than-air",
                                   "13": "unmanned aerial vehicle",
                                   "14": "space / transatmospheric vehicle",
                                   "15": "ultralight / handglider / paraglider",
                                   "16": "parachutist / skydiver",
                                   "17": "reserved",
                                   "18": "reserved",
                                   "19": "reserved",
                                   "20": "surface emergency vehicle",
                                   "21": "surface service vehicle",
                                   "22": "fixed ground or tethered obstruction",
                                   "23": "cluster obstacle",
                                   "24": "line obstacle"}

        self.__value_statistics.append(ValueStatistic('Emitter Category', ecat_key_str, ecat_value_descriptions))

        nacp_key_str = "090.NACp"
        nacp_value_descriptions = {"11": "EPU < 3 m, VEPU < 4 m",
                                   "10": "EPU < 10 m, VEPU < 15 m",
                                   "9": "EPU < 30 m, VEPU < 45 m",
                                   "8": "EPU < 0.05 NM (93 m)",
                                   "7": "EPU < 0.1 NM (185 m)",
                                   "6": "EPU < 0.3 NM (556 m)",
                                   "5": "EPU < 0.5 NM (926 m)",
                                   "4": "EPU < 1.0 NM (1852 m)",
                                   "3": "EPU < 2 NM (3704 m)",
                                   "2": "EPU < 4 NM (7408 m)",
                                   "1": "EPU < 10 NM (18520 km)",
                                   "0": "EPU > 10 NM or Unknown"}

        self.__value_statistics.append(ValueStatistic('Navigation Accuracy Category - Position',
                                                      nacp_key_str, nacp_value_descriptions))

        self.__value_statistics_lists = []  # type: List[ValueStatisticList]
        self.__value_statistics_lists.append(ValueStatisticList("MOPS Version for ECAT",
                                                                ecat_key_str, ecat_value_descriptions,
                                                                mops_vn_key_str, mops_vn_value_descriptions))

        self.__value_statistics_lists.append(ValueStatisticList("NACp for ECAT",
                                                                ecat_key_str, ecat_value_descriptions,
                                                                nacp_key_str, nacp_value_descriptions))

        #self.__value_statistics_lists.append(ValueStatisticList("NACp for Mode S Address",
        #                                                        "080.Target Address", [],
        #                                                        nacp_key, nacp_value_descriptions))



    @property
    def num_records(self):
        return self.__num_records

    def process_record(self, cat, record):

        assert cat == 21
        self.__num_records += 1

        for statistic in self.__value_statistics:
            statistic.process_record(record)

        for statistic in self.__value_statistics_lists:
            statistic.process_record(record)


    def print(self):
        print('num ads-b records {}'.format(self.__num_records))

        for statistic in self.__value_statistics:
            statistic.print()
            print ('-' * 80)

        for statistic in self.__value_statistics_lists:
            statistic.print()
            print('-' * 80)

def main(argv):

    parser = argparse.ArgumentParser(description='ASTERIX data item analysis')
    parser.add_argument('--framing', help='Framing True or False', required=True)

    args = parser.parse_args()

    assert args.framing is not None
    framing = args.framing

    num_blocks = 0

    statistics_calc = ADSBQualityStatisticsCalculator()  # type: ADSBQualityStatisticsCalculator

    # with filtering
    record_extractor = RecordExtractor(
        framing, statistics_calc.process_record, lambda cat, record: cat != 21)  # type: RecordExtractor

    start_time = time.time()

    for line in sys.stdin:
        #print(line)

        json_data = json.loads(line)

        record_extractor.find_records (json_data)

        num_blocks += 1

        end_time = time.time()
        print('blocks {0} time {1:.2f}s records {2} rate {3} rec/s'.format(
            num_blocks, end_time-start_time, statistics_calc.num_records,
            int(statistics_calc.num_records/(end_time-start_time))))

    statistics_calc.print()


if __name__ == "__main__":
    main(sys.argv[1:])