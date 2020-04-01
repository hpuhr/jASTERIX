from util.common import find_value

def get_airspeed_kt(record):
    if find_value("150.Air Speed", record) is None or find_value("150.IM", record) is None:
        return None

    speed = find_value("150.Air Speed", record)
    im = find_value("150.IM", record)

    # (IM)
    # = 0 Air Speed = IAS
    # if IAS, LSB = 2 NM/s
    if im == 0:
        return speed * 3600.0
    # = 1 Air Speed = Mach
    # if Mach, LSB = 0.001
    if im == 1:
        return speed * 0.001 * 666.739


def get_airspeed_mach(record):
    if find_value("150.Air Speed", record) is None or find_value("150.IM", record) is None:
        return None

    speed = find_value("150.Air Speed", record)
    im = find_value("150.IM", record)

    # (IM)
    # = 0 Air Speed = IAS
    # if IAS, LSB = 2 NM/s
    if im == 0:
        return speed * 0.185205
    # = 1 Air Speed = Mach
    # if Mach, LSB = 0.001
    if im == 1:
        return speed * 0.001


def get_adsb_alt_cap_as_double(record):

    # = 0 25 ft
    if find_value("040.ARC", record) == 0:
        return 25.0
    # = 1 100 ft
    elif find_value("040.ARC", record) == 1:
        return 100.0
    # = 2 Unknown
    # = 3 Invalid
    else:
        return None

def get_link_technology_mds(record):
    if find_value("210.LTT", record) is None:
        return None

    # = 2 1090 ES
    if find_value("210.LTT", record) == 2:
        return 'Y'
    else:
        return 'N'

def get_link_technology_other(record):
    if find_value("210.LTT", record) is None:
        return None

    # = 0 Other
    if find_value("210.LTT", record) == 0:
        return 'Y'
    else:
        return 'N'

def get_link_technology_uat(record):
    if find_value("210.LTT", record) is None:
        return None

    # = 1 UAT
    if find_value("210.LTT", record) == 1:
        return 'Y'
    else:
        return 'N'


def get_link_technology_vdl(record):
    if find_value("210.LTT", record) is None:
        return None

    # = 3 VDL 4
    if find_value("210.LTT", record) == 3:
        return 'Y'
    else:
        return 'N'




