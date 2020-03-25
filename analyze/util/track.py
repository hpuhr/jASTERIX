from util.common import *
import math

def get_detection_type(record):
    if find_value("080.CST", record) == 1:
        return 0  # no detection

    psr_updated = find_value("080.PSR", record) == 0
    ssr_updated = find_value("080.SSR", record) == 0
    mds_updated = find_value("080.MDS", record) == 0
    ads_updated = find_value("080.ADS", record) == 0

    if not mds_updated:
        if psr_updated and not ssr_updated:

            if find_value("290.MLT.Age", record) is not None:
                # age not 63.75
                mlat_age = find_value("290.MLT.Age", record)

                if mlat_age <= 12.0:
                    return 3

            return 1  # single psr, no mode-s

        if not psr_updated and ssr_updated:
            return 2  # single ssr, no mode-s

        if psr_updated and ssr_updated:
            return 3  # cmb, no mode-s

        # not psr_updated and not ssr_updated:
        if find_value("380.ADR.Target Address", record) is not None:
            return 5

        if find_value("060.Mode-3/A reply", record) is not None \
                or find_value("136.Measured Flight Level", record) is not None:
            return 2

        return 0  # unknown

    else:
        if not psr_updated:
            return 5  # ssr, mode-s
        else:
            return 7  # cmb, mode-s




def get_track_angle(record):

    v_x = find_value("185.Vx", record)
    v_y = find_value("185.Vy", record)

    if v_x is None or v_y is None:
        return None

    track_angle = math.atan2(v_x, v_y) * RAD2DEG
    if track_angle < 0:
        track_angle += 360.0

    return track_angle

def get_speed(record):

    v_x = find_value("185.Vx", record)
    v_y = find_value("185.Vy", record)

    if v_x is None or v_y is None:
        return None

    return math.sqrt(pow(v_x, 2) + pow(v_y, 2)) * 1.94384
