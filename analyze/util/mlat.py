from util.common import find_value


def get_alt_cap_as_double(record):
    if find_value("230.ARC", record) == 0:
        return 100.0
    elif find_value("230.ARC", record) == 1:
        return 25.0
    else:
        return None


