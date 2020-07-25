from reconst_util.chain import ADSBModeSChain
from reconst_util.reconstructed_chain import ReconstructedADSBModeSChain
from reconst_util.target_report import ADSBTargetReport
from reconst_util.geo_position import GeoPosition
from reconst_util.reference_update import ReferenceUpdate

import numpy as np
import nvector as nv

from filterpy.kalman import KalmanFilter, rts_smoother
from filterpy.common import Q_continuous_white_noise

class UMKalmanFilter2D:
    def __init__(self, name):
        self.name = name

        # measurement noise stddev
        #self.R_std = 10
        # the process noise stddev
        self.Q_std = 10

        self.f = KalmanFilter(dim_x=4, dim_z=2)
        #x = (x,y,x',y')

        # init state transition matrix
        self.f.F = np.array([[1, 1, 0, 0],
                             [0, 1, 0, 0],
                             [0, 0, 1, 1],
                             [0, 0, 0, 1]])

        # init process noise matrix
        self.f.Q = Q_continuous_white_noise(dim=2, dt=1, spectral_density=self.Q_std ** 2, block_size=2)

        #measurement function:
        self.f.H = np.array([[1, 0, 0, 0],
                             [0, 0, 1, 0]])

        #covariance matrix
        self.P_std = 10
        #self.f.P = np.eye(4) * 100.
        self.f.F = np.array([[self.P_std**2, 0, 0, 0],
                             [0, self.P_std**3, 0, 0],
                             [0, 0, self.P_std**2, 0],
                             [0, 0, 0, self.P_std**3]])

        #measurement noise
        #self.f.R = np.eye(2) * self.R_std ** 2

    def filter(self, chain):
        assert isinstance(chain, ADSBModeSChain)

        print('UMKalmanFilter2D: filtering chain utn {} ta {}'.format(chain.utn, hex(chain.target_address)))

        assert len(chain.target_reports)

        # calculate center_lat, center_long
        center_lat = 0
        center_long = 0

        for tod, target_report in chain.target_reports.items():  # type: ADSBTargetReport
            center_lat += target_report.get("pos_lat_deg")
            center_long += target_report.get("pos_long_deg")

        center_lat /= len(chain.target_reports)
        center_long /= len(chain.target_reports)

        center_pos = nv.GeoPoint(center_lat, center_long, 0, degrees=True)

        #print('UMKalmanFilter2D: center latitude {} longitude {}'.format(center_lat, center_long))

        first = True
        time_last = None

        ts = []
        zs = []
        Fs = []
        Qs = []
        Rs = []

        # process
        for tod, target_report in chain.target_reports.items():  # type: float,ADSBTargetReport
            #data_point 0: time, 1: x, 2: y, 3: fl, 4: date

            x, y, z = target_report.position.getENU(center_pos)  # east, north, up

            #tmp = GeoPosition()
            #tmp.setENU(x, y, z, center_pos)
            #print('org {} tmp {}'.format(target_report.position.getGeoPosStr(), tmp.getGeoPosStr()))

            #print('point tod {} x {} y {} z {}'.format(tod, x, y, z))

            if first:
                self.f.x = np.array([[x, 0., y, 0.]]).T
                time_last = tod

                first = False
                continue

            # z = get_sensor_reading()
            time_current = tod
            dt = time_current - time_last

            if dt == 0:
                print('{}: skipping same-time point at {}'.format(self.name, time_current))
                continue

            #print ('dt {}'.format(dt))
            assert dt > 0

            #ts.append((data_point[4], time_current)) #date,time
            ts.append(tod)

            # state transition matrix:, set time dependent
            Fs.append(np.array([[1, dt, 0, 0],
                                [0, 1, 0, 0],
                                [0, 0, 1, dt],
                                [0, 0, 0, 1]]))

            # the process noise, set time dependent
            #self.f.Q = Q_discrete_white_noise(dim=4, dt=dt, var=self.Q_std ** 2)
            Qs.append(Q_continuous_white_noise(dim=2, dt=dt, spectral_density=self.Q_std ** 2, block_size=2))

            zs.append(np.array([x, y]))  # TODO veloctities

            Rs.append(target_report.rs_2d)

            #print target_report

            time_last = time_current

        (mu, cov, _, _) = self.f.batch_filter(zs=zs, Fs=Fs, Qs=Qs, Rs=Rs)  # TODO Rs

        #print('ts {} zs {} mu {} cov {}'.format(len(ts), len(zs), len(mu), len(cov)))
        assert len(ts) == len(zs) == len(mu) == len(cov)

        (mu_smoothed, cov_smoothed, K, Pp) = rts_smoother(mu, cov, Fs, Qs)

        reconstructed = ReconstructedADSBModeSChain(chain.utn, chain.target_address)
        reconstructed.target_reports = chain.target_reports

        for cnt in range(0, len(mu)):  # until len(mu)-1
            tod = ts[cnt]
            assert tod in chain.target_reports

            target_report = chain.target_reports[tod]  # type: ADSBTargetReport

            ref_fil = ReferenceUpdate(chain.utn, tod)
            ref_fil.fromADSBTargetReport(target_report)
            ref_fil.sac = 0
            ref_fil.sic = 0

            #print('\ntod {}'.format(tod))

            #print('org x {} y {}'.format(zs[cnt][0], zs[cnt][1]))
            # x mu[cnt][0][0], y mu[cnt][2][0]
            ref_fil_pos = GeoPosition()
            #print('fil x {} y {}'.format(mu[cnt][0][0], mu[cnt][2][0]))
            ref_fil_pos.setENU(mu[cnt][0][0], mu[cnt][2][0], 0, center_pos)

            ref_fil.pos_lat_deg, ref_fil.pos_long_deg, _ = ref_fil_pos.getGeoPos()

            reconstructed.filtered_target_reports[tod] = ref_fil

            ref_smo = ReferenceUpdate(chain.utn, tod)
            ref_smo.fromADSBTargetReport(target_report)
            ref_smo.sac = 0
            ref_smo.sic = 1

            ref_smo_pos = GeoPosition()
            #print('smo x {} y {}'.format(mu_smoothed[cnt][0][0], mu_smoothed[cnt][2][0]))
            ref_smo_pos.setENU(mu_smoothed[cnt][0][0], mu_smoothed[cnt][2][0], 0, center_pos)

            ref_smo.pos_lat_deg, ref_fil.pos_long_deg, _ = ref_fil_pos.getGeoPos()

            reconstructed.smoothed_target_reports[tod] = ref_smo

            #print('org {}'.format(target_report.position.getGeoPosStr()))
            #print('fil {}'.format(ref_fil_pos.getGeoPosStr()))
            #print('smo {}'.format(ref_smo_pos.getGeoPosStr()))

        return reconstructed

        # for i in range (0, len(zs)):
        #     target_report = TargetReport (new_sensor_id)
        #     target_report.sensor_id = new_sensor_id
        #     target_report.date = ts[i][0]
        #     target_report.time_detection = ts[i][1]
        #     target_report.position_x = mu[i][0][0]
        #     target_report.position_y = mu[i][2][0]
        #     target_report.groundspeed_x = mu[i][1][0]
        #     target_report.groundspeed_y = mu[i][3][0]
        #     target_report.position_x_variance = cov[i][0][0]
        #     target_report.position_y_variance = cov[i][2][2]
        #     target_report.position_xy_covariance = cov[i][0][2]
        #     target_report.target_address = chain.key
        #     target_report.unique_track_num = chain.key
        #
        #     filtered_chain.addData(target_report)
        #
        #
        # return filtered_chain
