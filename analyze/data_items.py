import sys
import json
import time

def find_records (json_data, use_framing, callback):

    if use_framing:
        if 'frames' not in json_data:
            raise ValueError("no frames in given data")

        frames = json_data['frames']

        for frame in frames:

            if 'content' not in frame:
                raise ValueError("no content in data_block")

            frame_content = frame['content']

            if 'data_blocks' not in frame_content:
                raise ValueError("no data_blocks in frame_content")

            data_blocks = frame_content['data_blocks']

            for data_block in data_blocks:
                if 'category' not in data_block:
                    raise ValueError("no category in data_block")

                cat = data_block['category']

                if 'content' not in data_block:
                    raise ValueError("no content in data_block")

                content = data_block['content']

                if 'records' not in content:
                    raise ValueError("no records in content")

                records = content['records']

                for record in records:
                    callback (cat, record)
    else:
        if 'data_blocks' not in json_data:
            raise ValueError("no data_blocks in frame_content")

        data_blocks = frame_content['data_blocks']

        for data_block in data_blocks:
            if 'category' not in data_block:
                raise ValueError("no category in data_block")

            cat = data_block['category']

            if 'content' not in data_block:
                raise ValueError("no content in data_block")

            content = data_block['content']

            if 'records' not in content:
                raise ValueError("no records in content")

            records = content['records']

            for record in records:
                callback (cat, record)

num_lines=0
framing = True
num_records = 0
num_records_cat = {}

data_items = {}

def pretty_print(d, indent=0):
    for key, value in sorted(d.items()):
        print('\t' * indent + str(key))

        if isinstance(value, dict):
            pretty_print(value, indent+1)
        else:
            print('\t' * (indent+1) + str(value))

def add_data_items (record_object, counter_object):

    assert isinstance(record_object, dict)

    for key, value in record_object.items():

        if key not in counter_object:
            counter_object[key] = {}
            counter_object[key]['count'] = 1
        else:
            counter_object[key]['count'] += 1

        if isinstance(value, dict):
            add_data_items(value, counter_object[key])
        elif isinstance(value, (bool, int, float, str)):
            if 'min' not in counter_object[key]:
                counter_object[key]['min'] = value
                counter_object[key]['max'] = value
            else:
                counter_object[key]['min'] = min(counter_object[key]['min'], value)
                counter_object[key]['max'] = max(counter_object[key]['max'], value)

def process_record (cat, record):
    global num_records
    global data_items

    num_records += 1

    if cat not in num_records_cat:
        num_records_cat[cat]=1
    else:
        num_records_cat[cat] += 1

    if cat not in data_items:
        data_items[cat] = {}

    add_data_items (record, data_items[cat])

start_time = time.time()

for line in sys.stdin:
    #print(line)

    json_data = json.loads(line)

    find_records (json_data, framing, process_record)

    #print (json.dumps(loaded_json))
    num_lines += 1

    end_time = time.time()
    print('lines {0} time {1:.2f}s rate {2} rec/s'.format(num_lines, end_time-start_time, int(num_records/(end_time-start_time))))

print ('num records {}'.format(num_records))
print ('num records per cat')
pretty_print(num_records_cat)
print ('data items')
pretty_print(data_items)