#include "hashchecker.h"
#include "jasterix.h"
#include "logger.h"

bool check_artas_md5_hash {false};
HashChecker* hash_checker {nullptr};

using namespace nlohmann;
//using namespace Utils;

void check_callback (std::unique_ptr<nlohmann::json> data_chunk, size_t num_frames, size_t num_records,
                     size_t num_errors)
{
    assert (hash_checker);
    hash_checker->process(std::move(data_chunk));
}

HashChecker::HashChecker(bool framing_used)
    : framing_used_(framing_used)
{
    record_data_present_ = jASTERIX::add_record_data;
}

void HashChecker::process (std::unique_ptr<nlohmann::json> data)
{
    //loginf << "UGA '" << data->dump(4) << "'" << logendl;

    unsigned int category;
    unsigned int index;
    unsigned int length;

    if (!framing_used_)
    {
        assert (data->find("data_blocks") != data->end());

        for (json& data_block : data->at("data_blocks"))
        {
            category = data_block.at("category");

            if (!data_block.contains("content")) // data blocks with errors
                continue;

            json& data_block_content = data_block.at("content");

            index = data_block_content.at("index");
            length = data_block_content.at("length");

            if (data_block_content.contains("records"))
            {
                for (json& record : data_block_content.at("records"))
                    processRecord (category, index, length, record);
            }
        }
    }
    else
    {
        assert (data->find("frames") != data->end());
        assert (data->at("frames").is_array());

        for (json& frame : data->at("frames"))
        {
            if (!frame.contains("content")) // frame with errors
                continue;

            assert (frame.at("content").is_object());

            if (!frame.at("content").contains("data_blocks")) // frame with errors
                continue;

            assert (frame.at("content").at("data_blocks").is_array());

            for (json& data_block : frame.at("content").at("data_blocks"))
            {
                if (!data_block.contains("category")) // data block with errors
                    continue;

                category = data_block.at("category");

                if (!data_block.contains("content")) // data block with errors
                    continue;

                json& data_block_content = data_block.at("content");

                index = data_block_content.at("index");
                length = data_block_content.at("length");

                if (data_block_content.contains("records"))
                {
                    for (json& record : data_block_content.at("records"))
                        processRecord (category, index, length, record);
                }
            }
        }
    }
}

void HashChecker::processRecord (unsigned int category, unsigned int index, unsigned int length, nlohmann::json& record)
{
//    loginf << "UGA  cat " << category << " index " << index << " length " << length
//           << " '" << record.dump(4) << "'" << logendl;

    assert (record.contains("artas_md5"));

    if (record_data_present_)
        hash_map_[record.at("artas_md5")].emplace_back(category, index, length, record.at("record_data"));
    else {
        hash_map_[record.at("artas_md5")].emplace_back(category, index, length, "");
    }

//    "artas_md5": "5073ed0f",
//    "record_data": "ff02620f221f645b9c0fff08bcc175c06ddf82"

}

void HashChecker::printCollisions ()
{
    unsigned int record_cnt {0};
    unsigned int collision_cnt {0};

    for (auto& hash_it : hash_map_)
    {
        if (hash_it.second.size() > 1)
        {
            loginf << "found  " << hash_it.second.size() << " collision in hash '" << hash_it.first << "':" << logendl;

            for (auto& rec_it : hash_it.second)
            {
                if (record_data_present_)
                    loginf << "\t cat " << rec_it.category_ << " data block index " << rec_it.index_ << " length "
                           << rec_it.length_ << " data '" << rec_it.record_data_ << "'" << logendl;
                else
                    loginf << "\t cat " << rec_it.category_ << " data block index " << rec_it.index_ << " length "
                           << rec_it.length_ << logendl;
                ++collision_cnt; // one for each
            }
        }

        record_cnt += hash_it.second.size();
    }

    if (record_cnt)
        loginf << "found " << collision_cnt << " collisions in " << record_cnt << " records ("
               << 100.0*collision_cnt/record_cnt << "%)" << logendl;
    else
        loginf << "found " << collision_cnt << " collisions in " << record_cnt << " records" << logendl;
}
