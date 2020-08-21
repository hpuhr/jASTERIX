from compasslib.data.datasourcesensor import DataSourceSensor
from compasslib.data.datasourcetracker import DataSourceTracker
from compasslib.data.datasourcetype import DataSourceType
from compasslib.data.datasource import DataSource, status_row_count
from compasslib.common.utils import *
from compasslib.track.track import Track
from compasslib.track.trackclassification import TrackClassification
from compasslib.report.report import *
from compasslib.data.datastore import DataStore
import compasslib.config.config_global as config_global
from compasslib.reconstruct.umkalman2d import UMKalmanFilter2D
from compasslib.reconstruct.umkalman3d import UMKalmanFilter3D
from compasslib.reconstruct.exkalman3d import ExtendedKalmanFilter3D
from compasslib.reconstruct.unscentedkalman3d import UnscentedKalmanFilter3D
from compasslib.reconstruct.imm import IMMFilter
from compasslib.data.targetreportcollection import TargetReportCollection

from operator import itemgetter

import numpy as np

import array
import math
import datetime
import random


class Reconstructor:
    """Calculates reference trajectories/tracks using nifty math"""

    def __init__(self, datastore):

        print ('reconstructor: setting up')
        self._datastore = datastore

        self._excluded_sources = {}

        if config_global.reconstruction_exclude_sources:
            sensors = self._datastore.sensors()

            found = False
            for src_name in config_global.reconstruction_exclude_sources:
                for sensor_id, sensor in sensors.iteritems():
                    if src_name == sensor.name or src_name == sensor.name:
                        if sensor.sensor_type not in self._excluded_sources:
                            self._excluded_sources [sensor.sensor_type] = []
                        self._excluded_sources [sensor.sensor_type].append(str(sensor_id))
                        found = True

                if not found:
                    print ('WARN: reconstructor excluded source {} not found'.format(src_name))

    def work(self):
        print ('reconstructor: working')

        key_lambda = lambda x: x.target_address
        subkey_lambda = lambda x: x.time_detection

        np.set_printoptions(suppress=True, precision=2, linewidth=200)

        # load data from sources
        src_collection = self.collectData(key_lambda, subkey_lambda)

        # get new sensor id
        ref_src = self._datastore.data_sources[DataSourceType.REFTRAJ]
        #ref_src = self._datastore.data_sources[DataSourceType.TRACK]
        assert ref_src
        ref_sensor_id = ref_src.getOrAddSensorId(config_global.reconstruction_run_name)

        # filter collected chains
        no_modes_chain, filtered_collection = self.filterChains(src_collection, key_lambda, subkey_lambda, ref_sensor_id)

        if config_global.reconstruction_associate_no_mode_s:
            # associate non-mode-s target reports
            unassociated_target_reports = self.associateNoModeSTargetReports (no_modes_chain, filtered_collection,
                                                                          original_collection=src_collection)
            # remove the none chain from original data
            src_collection.removeChain (None)

            # re-filter original data
            no_modes_chain, filtered_collection = self.filterChains(src_collection, key_lambda, subkey_lambda,
                                                                    ref_sensor_id)

        #extrapolate if required
        if config_global.reconstruction_extrapolate:
            print ('reconstructor: extrapolating to full seconds')
            for key, filtered_chain in filtered_collection.data.iteritems():
                filtered_chain.extrapolateToFullSeconds()

        data = filtered_collection.getDataDicts()
        print ('reconstructor: got {} filtered target reports'.format(len(data)))

        for key, xlim, ylim in config_global.reconstruction_plot_keys:
            test_chain = filtered_collection.getChain (key) # 5022335, 3772899
            if test_chain is not None:
                print ('reconstructor: plotting chain {}'.format(key))
                test_chain.plot(xlim, ylim)

        sorted_data = sorted(data, key=itemgetter('time_detection'))

        assert len(sorted_data) == len(data)

        # inserted as track, the speed must be multiplied by 100, since stupidly stored as int cm/s
        #for key, filtered_chain in filtered_collection.data.iteritems():
        #    for target_report in filtered_chain.getChain():
        #        target_report.groundspeed_x *= 100
        #        target_report.groundspeed_y *= 100

        if len(sorted_data) > 0 and config_global.reconstruction_write_db:
            print ('reconstructor: inserting data')
            for target_report_dict in sorted_data:
                #print target_report_dict
                ref_src.insertData(target_report_dict)
            print ('reconstructor: post-processing reference')
            ref_src.updateCount()
            ref_src.postProcess()
            #ref_src.postProcess(self._datastore.data_sources, False)
            #ref_src.findTrackReferenceCorrelations(self._datastore.data_sources[DataSourceType.REFTRAJ])

    def collectData (self, key_lambda, subkey_lambda):
        src_collection = None

        print ('reconstructor: collecting sensor information')

        variables = ['rec_num', 'sensor_id', 'time_detection', 'position_x', 'position_y', 'flight_level',
                     'target_address', 'date', 'position_x_variance', 'position_y_variance', 'position_xy_covariance']

        for src_type, source in self._datastore.data_sources.iteritems():
            if source.hasData():

                filter_str = None

                if src_type in self._excluded_sources:
                    filter_str = 'sensor_id not in ('+",".join(self._excluded_sources [src_type])+')'

                print ('reconstructor: collecting target reports from {}, filter {}'.format(source.name, filter_str))

                if src_collection is None:
                    src_collection = source.getTargetReportCollection(variables, key_lambda, subkey_lambda, filter_str)
                else:
                    src_collection.addData(source, source.getTargetReports(variables, filter_str))

        return src_collection

    def filterChains (self, src_collection, key_lambda, subkey_lambda, ref_sensor_id):
        filtered_collection = TargetReportCollection(key_lambda, subkey_lambda)

        no_modes_chain = None

        print ('reconstructor: doing chaining based on Mode S')

        for key, target_chain in src_collection.data.iteritems():

            if key is None:
                assert no_modes_chain is None
                no_modes_chain = target_chain.getChain()
                continue


            if len(target_chain) < config_global.reconstruction_min_num_updates:
                print ('reconstructor: skipping short {}'.format(target_chain))
                continue

            if key in config_global.reconstruction_exclude_mode_s \
                    or (config_global.reconstruction_limit_mode_s_to is not None
                        and key not in config_global.reconstruction_limit_mode_s_to): #exlucde certain addresses
                print ('reconstructor: skipping excluded {}'.format(target_chain))
                continue

            #print ('filtering {}'.format(target_chain))

            kalman_filter = UMKalmanFilter2D ('UMKalman2D')
            kalman_filter = UMKalmanFilter3D('UMKalman3D')
            #kalman_filter = ExtendedKalmanFilter3D ('ExKalman3D')
            kalman_filter = UnscentedKalmanFilter3D ('FragrantKalman3D')
            kalman_filter = IMMFilter ('IMM3D')

            filtered_chain = kalman_filter.filter(chain=target_chain, new_sensor_id=ref_sensor_id, smooth=False)

            if filtered_chain:
                filtered_collection.addChain(key, filtered_chain)

        return no_modes_chain, filtered_collection


    def associateNoModeSTargetReports (self, no_modes_chain, filtered_collection, original_collection):

        print ('reconstructor: processing non-Mode S data, len {}'.format(len(no_modes_chain)))

        no_modes_unassociated_chain = []

        cnt=0
        matches_cnt = 0

        for target_report in no_modes_chain:
            matches = {} # score -> filtered chain

            if cnt % 2000 == 0:
                print ('processed {} of {} ({}%)'.format(cnt, len(no_modes_chain), int(100*cnt/len(no_modes_chain))))

            for key, filtered_chain in filtered_collection.data.iteritems():
                if not filtered_chain.existsAtTime (target_report.time_detection):
                    continue

                score = filtered_chain.getPositionMatchScore (target_report)

                if score is not None:
                    matches[score] = filtered_chain

            if len(matches) > 0:
                #for score_cnt in range(0, 3):
                #    min_key = min(matches.keys())
                #    print ('score #{}: {} to {}'.format(score_cnt, min_key, matches[min_key]))
                #    del matches[min_key]

                min_key = min(matches.keys())

                if min_key <= config_global.reconstruction_primary_association_score_threshold:
                    chain_key = matches[min_key].key

                    associated_chain = original_collection.getChain(chain_key)
                    assert associated_chain is not None

                    associated_chain.addData (target_report)

                    matches_cnt += 1
                else:
                    no_modes_unassociated_chain.append(target_report)
            else:
                no_modes_unassociated_chain.append(target_report)

            cnt += 1

        print ('reconstructor: chaining non-Mode S done, found {} matches ({}%), rest {}'.format(
            matches_cnt, int(100*matches_cnt/len(no_modes_chain)), len(no_modes_unassociated_chain)))

        return no_modes_unassociated_chain
