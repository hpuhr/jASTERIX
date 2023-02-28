#ifndef HASHCHECKER_H
#define HASHCHECKER_H

#include <map>
#include <memory>

#include "json.hpp"

struct RecordInfo
{
    RecordInfo(unsigned int category, unsigned int index, unsigned int length,
               const std::string& record_data)
        : category_(category), index_(index), length_(length), record_data_(record_data)
    {
    }
    unsigned int category_;
    unsigned int index_;       // data block index
    unsigned int length_;      // data block length
    std::string record_data_;  // hex_dump, optional
};

class HashChecker
{
  public:
    HashChecker(bool framing_used);

    void process(std::unique_ptr<nlohmann::json> data);
    void printCollisions();

  private:
    bool framing_used_{false};

    std::map<std::string, std::vector<RecordInfo>> hash_map_;  // hash -> [RecordInfo]

    // data block index, length
    void processRecord(unsigned int category, nlohmann::json& record);
};

extern std::string check_artas_md5_hash;
extern std::vector<int> check_artas_md5_categories;
extern std::unique_ptr<HashChecker> hash_checker;

extern void check_callback(std::unique_ptr<nlohmann::json> data_chunk, size_t num_frames,
                           size_t num_records, size_t num_errors);

#endif  // HASHCHECKER_H
