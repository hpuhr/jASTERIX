

class RecordExtractor:
    def __init__(self, framing, callback):
        self.__framing = framing
        self.__callback = callback

    def find_records(self, json_data):

        if self.__framing:
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
                        self.__callback(cat, record)
        else:
            if 'data_blocks' not in json_data:
                raise ValueError("no data_blocks in frame_content")

            data_blocks = json_data['data_blocks']

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
                    self.__callback(cat, record)