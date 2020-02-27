

class RecordExtractor:
    def __init__(self, framing, callback, record_filter=None):
        self.__framing = framing
        self.__callback = callback
        self.__record_filter = record_filter
        self.num_records = 0
        self.num_filtered = 0

    def find_records(self, json_data):

        if self.__framing:
            if 'frames' not in json_data:
                print("warn: no frames in given data")
                return

            frames = json_data['frames']

            for frame in frames:

                if 'content' not in frame:
                    print("warn: no content in data_block")
                    return

                frame_content = frame['content']

                if 'data_blocks' not in frame_content:
                    print("warn: no data_blocks in frame_content")
                    return

                data_blocks = frame_content['data_blocks']

                for data_block in data_blocks:
                    if 'category' not in data_block:
                        print("warn: no category in data_block")
                        return

                    cat = data_block['category']

                    if 'content' not in data_block:
                        print("warn: no content in data_block")
                        return

                    content = data_block['content']

                    if 'records' not in content:
                        print("warn: no records in content")
                        return

                    records = content['records']

                    for record in records:
                        self.num_records += 1

                        if self.__record_filter is not None:
                            if not self.__record_filter(cat, record):
                                self.__callback(cat, record)
                            else:
                                self.num_filtered += 1
                        else:
                            self.__callback(cat, record)
        else:
            if 'data_blocks' not in json_data:
                print("warn: no data_blocks in frame_content")
                return

            data_blocks = json_data['data_blocks']

            for data_block in data_blocks:
                if 'category' not in data_block:
                    print("warn: no category in data_block")
                    return

                cat = data_block['category']

                if 'content' not in data_block:
                    print("warn: no content in data_block")
                    return

                content = data_block['content']

                if 'records' not in content:
                    print("warn: no records in content")
                    return

                records = content['records']

                for record in records:
                    self.num_records += 1

                    if self.__record_filter is not None:
                        if not self.__record_filter(cat, record):
                            self.__callback(cat, record)
                        else:
                            self.num_filtered += 1
                    else:
                        self.__callback(cat, record)
