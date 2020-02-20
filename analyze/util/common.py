from typing import List


def find_value(key_location, json_data):
    assert isinstance(key_location, List)

    current_json = json_data
    for key in key_location:
        if key not in current_json:
            return None
        else:
            current_json = current_json[key]
            continue

    return current_json
