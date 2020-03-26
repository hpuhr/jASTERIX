from util.common import find_value


def get_alt_cap_as_double(record):
    if find_value("230.ARC", record) == 0:
        return 100.0
    elif find_value("230.ARC", record) == 1:
        return 25.0
    else:
        return None


def get_rdp_chain(record):
    if find_value("020.CHN", record) is None:
        return None
    else:
        return find_value("020.CHN", record) + 1




