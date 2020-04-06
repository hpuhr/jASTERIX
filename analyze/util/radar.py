from util.common import find_value


def get_alt_cap_as_double(record):
    if find_value("230.ARC", record) == 0:
        return 100.0
    elif find_value("230.ARC", record) == 1:
        return 25.0
    else:
        return None


def get_rdp_chain(record):
    if find_value("020.RDP", record) is None:
        return None
    else:
        return str(find_value("020.RDP", record) + 1)

def get_rdp_chain_rdpc(record):
    if find_value("170.RDPC", record) is None:
        return None
    else:
        return str(find_value("170.RDPC", record) + 1)


def get_rdp_antenna(record):
    if find_value("020.ANT", record) is None:
        return None
    else:
        return str(find_value("020.ANT", record) + 1)


def get_civil_emergency(record):
    if find_value("070.Mode-3/A reply", record) is None:
        return None
    else:
        mode3a_code = find_value("070.Mode-3/A reply", record)

        # 5 = code 7500 (unlawful interference)
        if mode3a_code == 7500:
                return 5
        # 6 = code 7600 (radio comunnication failure)
        if mode3a_code == 7600:
            return 6
        # 7 = code 7700 (emergency) [I001/020 DS1/DS2]
        if mode3a_code == 7700:
            return 7
        # 0 = default is wrong, is None
        return None


def get_ground_bit(record):
    if find_value("230.STAT", record) is None:
        return None
    else:
        stat = find_value("230.STAT", record)

        # = 0 No alert, no SPI, aircraft airborne
        if stat == 0:
            return 'N'
        # = 1 No alert, no SPI, aircraft on ground
        if stat == 1:
            return 'Y'
        # = 2 Alert, no SPI, aircraft airborne
        if stat == 2:
            return 'N'
        # = 3 Alert, no SPI, aircraft on ground
        if stat == 3:
            return 'Y'
        # = 4 Alert, SPI, aircraft airborne or on ground
        # = 5 No alert, SPI, aircraft airborne or on ground
        # 6 - 7 Not assigned
        return None


def get_spi_bit(record):
    if find_value("020.STAT", record) is None:
        return None
    else:
        stat = find_value("020.STAT", record)

        # = 0 No alert, no SPI, aircraft airborne
        if stat == 0:
            return 'N'
        # = 1 No alert, no SPI, aircraft on ground
        if stat == 1:
            return 'N'
        # = 2 Alert, no SPI, aircraft airborne
        if stat == 2:
            return 'N'
        # = 3 Alert, no SPI, aircraft on ground
        if stat == 3:
            return 'N'
        # = 4 Alert, SPI, aircraft airborne or on ground
        if stat == 4:
            return 'Y'
        # = 5 No alert, SPI, aircraft airborne or on ground
        if stat == 5:
            return 'Y'
        # 6 - 7 Not assigned
        return None


def get_alert_bit(record):
    if find_value("020.STAT", record) is None:
        return None
    else:
        stat = find_value("020.STAT", record)

        # = 0 No alert, no SPI, aircraft airborne
        if stat == 0:
            return 'N'
        # = 1 No alert, no SPI, aircraft on ground
        if stat == 1:
            return 'N'
        # = 2 Alert, no SPI, aircraft airborne
        if stat == 2:
            return 'Y'
        # = 3 Alert, no SPI, aircraft on ground
        if stat == 3:
            return 'Y'
        # = 4 Alert, SPI, aircraft airborne or on ground
        if stat == 4:
            return 'Y'
        # = 5 No alert, SPI, aircraft airborne or on ground
        if stat == 5:
            return 'N'
        # 6 - 7 Not assigned
        return None


def get_mode4_friendly(record):
    if find_value("020.FOE/FRI", record) is None:
        return None
    else:
        frifoe = find_value("020.FOE/FRI", record)

    #Mode-4 interrorgation type:
    # - = no interrogation, 0 No Mode 4 interrogation
    if frifoe == 0:
        return '-'
    # F = Friendly target, 1 Friendly target
    if frifoe == 1:
        return 'F'
    # U = Unknown Target, 2 Unknown target
    if frifoe == 2:
        return 'U'
    # N = No Reply, 3 No reply
    if frifoe == 3:
        return 'N'


def get_climb_descend_mode(record):
    if find_value("170.CDM", record) is None:
        return None
    else:
        cdm = find_value("170.CDM", record)

        # value record '0' db 'M': 4117 M = Maintaining
        if cdm == 0:
            return 'M'
        # value record '1' db 'C': 1133 C = Climbing
        if cdm == 1:
            return 'C'
        # value record '2' db 'D': 330 D = Descending
        if cdm == 2:
            return 'D'
        # value record '3' db 'I': 569 I = Invalid
        if cdm == 3:
            return 'I'
        return None


def get_plot_track(record):
    if find_value("161.TRACK NUMBER", record) is None:
        return 0
    else:
        return 1