from reconst_util.chain import ADSBModeSChain
from reconst_util.reconstructed_chain import ReconstructedADSBModeSChain
from reconst_util.target_report import ADSBTargetReport, v0_pos_accuracies, v0_spd_accuracies, v12_pos_accuracies, v12_spd_accuracies
from reconst_util.geo_position import GeoPosition
from reconst_util.reference_update import ReferenceUpdate

import numpy as np
import nvector as nv
import math

from filterpy.kalman import KalmanFilter, rts_smoother
from filterpy.common import Q_continuous_white_noise

class UMKalmanFilter2D:
    def __init__(self, name):
        self.name = name

        # measurement noise stddev
        #self.R_std = 10
        # the process noise stddev
        self.Q_std = 50

        self.f = KalmanFilter(dim_x=4, dim_z=4)
        #x = (x,y,x',y')

        # init state transition matrix
        #self.f.F = np.array([[1, 1, 0, 0],
        #                     [0, 1, 0, 0],
        #                     [0, 0, 1, 1],
        #                     [0, 0, 0, 1]])

        # init process noise matrix
        #self.f.Q = Q_continuous_white_noise(dim=4, dt=1, spectral_density=self.Q_std ** 2, block_size=2)

        #measurement function:
        self.f.H = np.array([[1, 0, 0, 0],
                             [0, 1, 0, 0],
                             [0, 0, 1, 0],
                             [0, 0, 0, 1]])

        #covariance matrix
        #self.P_std = 10
        #self.f.P = np.eye(4) * 100.
        #self.f.F = np.array([[self.P_std**2, 0, 0, 0],
        #                     [0, self.P_std**3, 0, 0],
        #                     [0, 0, self.P_std**2, 0],
        #                     [0, 0, 0, self.P_std**3]])

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

            groundspeed_kt = target_report.get('groundspeed_kt')
            track_angle_deg = target_report.get('track_angle_deg')

            got_speed = groundspeed_kt is not None and track_angle_deg is not None

            if got_speed:
                track_angle_rad = np.deg2rad(90 - track_angle_deg)  # convert to math angle and to rad
                groundspeed_ms = groundspeed_kt * 0.514444
                #print(' groundspeed kt {} ms {}'.format(groundspeed_kt, groundspeed_ms))

                v_x = groundspeed_ms * np.cos(track_angle_rad)
                v_y = groundspeed_ms * np.sin(track_angle_rad)
            else:
                v_x = 0
                v_y = 0

            if first:
                self.f.x = np.array([[x, v_x, y, v_y]]).T
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

            # measurement
            zs.append(np.array([x, v_x, y, v_y]))

            # measurement noise
            pos_acc_stddev_m = None
            spd_acc_stddev_ms = None

            nucr_nacv = target_report.get('nucr_nacv')
            got_spd_acc = nucr_nacv is not None

            if target_report.get("mops_version") is not None:
                vn = target_report.get("mops_version")

                if vn == 0:
                    nucp_nic = target_report.get("nucp_nic")
                    if nucp_nic is not None and nucp_nic in v0_pos_accuracies:
                        pos_acc_stddev_m = v0_pos_accuracies[nucp_nic]

                    if got_spd_acc and nucr_nacv in v0_spd_accuracies:
                        spd_acc_stddev_ms = v0_spd_accuracies[nucr_nacv]

                elif vn == 1 or vn == 2:
                    nac_p = target_report.get("nac_p")
                    if nac_p is not None and nac_p in v12_pos_accuracies:
                        pos_acc_stddev_m = v12_pos_accuracies[nac_p]

                    if got_spd_acc and nucr_nacv in v12_spd_accuracies:
                        spd_acc_stddev_ms = v12_spd_accuracies[nucr_nacv]

            if pos_acc_stddev_m is None:
                pos_acc_stddev_m = 50  # m stddev default noise

            if spd_acc_stddev_ms is None:
                spd_acc_stddev_ms = 20
            if not got_speed:  # velocities not set
                spd_acc_stddev_ms = 500

            #print('pos x {} y {}  v_x {} v_y {} pos_stddev {} v_stddev {}'.format(
            #    x, y, v_x, v_y, pos_acc_stddev_m, vel_acc_stddev_ms))

            Rs.append(np.array([[pos_acc_stddev_m ** 2, 0, 0, 0],
                                [0, spd_acc_stddev_ms ** 2, 0, 0],
                                [0, 0, pos_acc_stddev_m ** 2, 0],
                                [0, 0, 0, spd_acc_stddev_ms ** 2]]))

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

            # filterned
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

            v_x = mu[cnt][1][0]
            v_y = mu[cnt][3][0]

            heading_math_rad = math.atan2(v_y, v_x)
            heading_deg = 90 - np.rad2deg(heading_math_rad)

            groundspeed_ms = math.sqrt(v_x ** 2 + v_y ** 2)
            groundspeed_kt = groundspeed_ms / 0.514444

            ref_fil.heading_deg = heading_deg
            ref_fil.groundspeed_kt = groundspeed_kt

            reconstructed.filtered_target_reports[tod] = ref_fil

            # smoothed
            ref_smo = ReferenceUpdate(chain.utn, tod)
            ref_smo.fromADSBTargetReport(target_report)
            ref_smo.sac = 0
            ref_smo.sic = 1

            ref_smo_pos = GeoPosition()
            #print('smo x {} y {}'.format(mu_smoothed[cnt][0][0], mu_smoothed[cnt][2][0]))
            ref_smo_pos.setENU(mu_smoothed[cnt][0][0], mu_smoothed[cnt][2][0], 0, center_pos)

            ref_smo.pos_lat_deg, ref_fil.pos_long_deg, _ = ref_fil_pos.getGeoPos()

            v_x = mu_smoothed[cnt][1][0]
            v_y = mu_smoothed[cnt][3][0]

            heading_math_rad = math.atan2(v_y, v_x)
            heading_deg = 90 - np.rad2deg(heading_math_rad)

            groundspeed_ms = math.sqrt(v_x ** 2 + v_y ** 2)
            groundspeed_kt = groundspeed_ms / 0.514444

            ref_smo_pos.heading_deg = heading_deg
            ref_smo_pos.groundspeed_kt = groundspeed_kt

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
