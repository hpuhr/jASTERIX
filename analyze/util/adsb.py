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

def get_ecat_str(record):
    if find_value("020.ECAT", record) is None:
        return None

    ecat = find_value("020.ECAT", record)

    # 0 = No ADS-B Emitter Category Information
    # value record '0' db 'NO_INFO': 20672
    if ecat == 0:
        return 'NO_INFO'

    # 1 = light aircraft <= 15500 lbs
    # value record '1' db 'LIGHT_AIRCRAFT': 2479
    elif ecat == 1:
        return 'LIGHT_AIRCRAFT'

    # 2 = 15500 lbs < small aircraft <75000 lbs
    # value record '2' db 'SMALL_AIRCRAFT': 1941
    elif ecat == 2:
        return 'SMALL_AIRCRAFT'

    # 3 = 75000 lbs < medium a/c < 300000 lbs
    # value record '3' db 'MEDIUM_AIRCRAFT': 44098
    elif ecat == 3:
        return 'MEDIUM_AIRCRAFT'

    # 4 = High Vortex Large
    # value record '4' db 'HIGH_VORTEX_LARGE': 7232
    elif ecat == 4:
        return 'HIGH_VORTEX_LARGE'

    # 5 = 300000 lbs <= heavy aircraft
    # value record '5' db 'HEAVY_AIRCRAFT': 56392
    elif ecat == 5:
        return 'HEAVY_AIRCRAFT'

    # 6 = highly manoeuvrable (5g acceleration capability) and high speed (>400 knots cruise)
    elif ecat == 6:
        return 'HIGHLY_MANOEUVRABLE'

    # 7 to 9 = reserved
    elif ecat in (7, 8, 9):
        return None  # ?

    # 10 = rotocraft
    elif ecat == 10:
        return 'ROTOCRAFT'  # ?

    # 11 = glider / sailplane
    elif ecat == 11:
        return 'GLIDER'  # ?

    # 12 = lighter-than-air
    elif ecat == 12:
        return 'LIGHTER_THAN_AIR'  # ?

    # 13 = unmanned aerial vehicle
    elif ecat == 13:
        return 'UNMANNED'  # ?

    # 14 = space / transatmospheric vehicle
    elif ecat == 14:
        return 'SPACE_VEHICLE'  # ?

    # 15 = ultralight / handglider / paraglider
    elif ecat == 15:
        return 'ULTRALIGHT'  # ?

    # 16 = parachutist / skydiver
    elif ecat == 16:
        return 'SKYDIVER'  # ?

    # 17 to 19 = reserved
    elif ecat in (17, 18, 19):
        return None  # ?

    # 20 = surface emergency vehicle
    # value record '20' db 'SURF_EMERGENCY': 286
    elif ecat == 20:
        return 'SURF_EMERGENCY'

    # 21 = surface service vehicle
    # value record '21' db 'SURF_SERVICE': 12773
    elif ecat == 21:
        return 'SURF_SERVICE'

    # 22 = fixed ground or tethered obstruction
    elif ecat == 22:
        return 'GROUND_OBSTRUCTION'  # ?
    # 23 = cluster obstacle

    elif ecat == 23:
        return 'CLUSTER_OBSTACLE'  # ?

    # 24 = line obstacle
    elif ecat == 24:
        return 'LINE_OBSTACLE'  # ?

    return None


def get_surveillance_status_str(record):
    if find_value("200.SS", record) is None:
        return None

    ss = find_value("200.SS", record)

    # 0 No condition reported
    if ss == 0:
        return 'NO_CONDITION'
    # = 1 Permanent Alert (Emergency condition)
    if ss == 1:
        return 'PERMANENT_ALERT'
    # = 2 Temporary Alert (change in Mode 3/A Code other than emergency)
    if ss == 2:
        return 'TEMPORARY_ALERT'
    # = 3 SPI set
    if ss == 3:
        return 'SPI_SET'  # ?




