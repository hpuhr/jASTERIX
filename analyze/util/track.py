from util.common import find_value

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

    else:
        if not psr_updated:
            return 5  # ssr, mode-s
        else:
            return 7  # cmb, mode-s

    if find_value("380.ADR.Target Address", record) is not None:
        return 5

    if find_value("060.Mode-3/A reply", record) is not None \
            or find_value("136.Measured Flight Level", record) is not None:
        return 2

    return 0  # unknown


def get_as_verif_flag(key_str, record, invert):

    value = find_value(key_str, record)
    if invert:
        # None if find_value("340.MDC.V", record) is None else ('N' if find_value("340.MDC.V", record) == 1 else 'Y')
        if value is None:
            return None

        if value == 0:
            return 'Y'
        else:
            return 'N'

    else:
        # None if find_value("340.MDC.G", record) is None else ('Y' if find_value("340.MDC.G", record) == 1 else 'N')
        if value is None:
            return None

        if value == 1:
            return 'Y'
        else:
            return 'N'
