/*
 * This file is part of ATSDB.
 *
 * ATSDB is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ATSDB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with ATSDB.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "asterixparser.h"
#include "logger.h"
#include "itemparserbase.h"
#include "record.h"
#include "jasterix.h"
#include "string_conv.h"

#include <iostream>
#include <iomanip>

#include <tbb/tbb.h>

using namespace std;
using namespace nlohmann;

namespace jASTERIX {

ASTERIXParser::ASTERIXParser(const nlohmann::json& data_block_definition,
                             std::map<unsigned int, std::shared_ptr<Category>>& category_definitions,
                             bool debug)
{
    // data block

    if (data_block_definition.find("name") == data_block_definition.end())
        throw runtime_error ("data block construction without JSON name definition");

    data_block_name_ = data_block_definition.at("name");

    if (data_block_definition.find("items") == data_block_definition.end())
        throw runtime_error ("data block construction without header items");

    if (!data_block_definition.at("items").is_array())
        throw runtime_error ("data block construction with items non-array");

    std::string item_name;
    ItemParserBase* item {nullptr};

    for (const json& data_item_it : data_block_definition.at("items"))
    {
        item_name = data_item_it.at("name");

        if (debug)
            loginf << "asterix parser constructing data block item '" << item_name << "'" << logendl;

        item = ItemParserBase::createItemParser(data_item_it);
        assert (item);
        data_block_items_.push_back(std::unique_ptr<ItemParserBase>{item});
    }

    // asterix record definitions

    for (auto& cat_it : category_definitions)
    {
        if (debug)
            loginf << "asterix parser constructing cat '" << setfill('0') << setw(3) << cat_it.first << "'"
                   << logendl;

        if (!cat_it.second->decode()) // if not decode, do not store record
        {
            loginf << "asterix parser not decoding cat '" << setfill('0') << setw(3) << cat_it.first << "'"
                   << logendl;
            continue;
        }
        loginf << "asterix parser decoding cat '" << setfill('0') << setw(3) << cat_it.first << "'"
               << logendl;

        records_.insert(std::pair<unsigned int, std::shared_ptr<Record>>
                        (cat_it.first, std::shared_ptr<Record>{cat_it.second->getCurrentEdition()->record()}));

        if (cat_it.second->hasCurrentREFEdition())
            records_.at(cat_it.first)->setRef(cat_it.second->getCurrentREFEdition()->reservedExpansionField());

        if (cat_it.second->hasCurrentMapping())
        {
            mappings_.insert(
                        std::pair<unsigned int, std::shared_ptr<Mapping>>
                        (cat_it.first, std::shared_ptr<Mapping>{cat_it.second->getCurrentMapping()}));
        }
    }
}

std::tuple<size_t, size_t, bool, bool> ASTERIXParser::findDataBlocks (const char* data, size_t index, size_t length,
                                                                      nlohmann::json& target, bool debug)
{
    if (debug)
        loginf << "ASTERIXParser: findDataBlocks index " << index << " length " << length << logendl;

    size_t parsed_bytes {0};
    size_t parsed_data_block_bytes {0};
    size_t parsed_bytes_sum {0};
    size_t num_blocks {0};
    bool error {false};

    //    loginf << "UGA AP index " << index << " length " << length << logendl;

    bool record_limit_hit {false};
    size_t current_index {index};

    try
    {


        while (parsed_bytes_sum < length)
        {
            //        loginf << "UGA AP1 parsed sum " << parsed_bytes_sum << " length " << length << " block " << num_blocks << logendl;

            if (record_chunk_size > 0 && num_blocks >= static_cast<size_t> (record_chunk_size))
            {
                record_limit_hit = true;
                break;
            }

            parsed_data_block_bytes = 0;


            for (auto& r_item : data_block_items_)
            {
                parsed_bytes = r_item->parseItem(data, current_index, length, parsed_data_block_bytes,
                                                 target[data_block_name_][num_blocks], debug);
                //            loginf << "UGA FP2 parsed " << parsed_bytes << " target '" << target[data_block_name_][num_blocks]
                //                      << "'" << logendl;

                parsed_data_block_bytes += parsed_bytes;
                parsed_bytes_sum += parsed_bytes;
                current_index += parsed_bytes;
            }

            //loginf << "UGA2 target block '" << target[data_block_name_][num_blocks].dump(4) << "'" << logendl;

            //        loginf << "UGA AP3 parsed " << parsed_bytes_sum << " length " << length << " block " << num_blocks
            //               << " target '" << target[data_block_name_][num_blocks] << "'" << logendl;

            ++num_blocks;
        }

    }
    catch (std::exception& e)
    {
        loginf << "asterix parser finding data blocks caught exception '" << e.what() << "' at index " << index
               << " length " << length << ", current index " << current_index << ", breaking" << logendl;
        error = true;
    }

    //loginf << "UGA3 target '" << target.dump(4) << "' parsed " << parsed_bytes_sum << " size " << length << logendl;

    //    {
    //        "cnt": 0,
    //        "content": {
    //            "index": 134,
    //            "length": 56,
    //            "record": {
    //                "category": 1,
    //                "content": {
    //                    "index": 137,
    //                    "length": 42
    //                },
    //                "length": 45
    //            }
    //        },
    //        "frame_length": 56,
    //        "frame_relative_time_ms": 2117
    //    }

    if (debug)
        loginf << "ASTERIXParser: findDataBlocks done parsed bytes " << parsed_bytes_sum
               << " num blocks " << num_blocks << " error " << error << " limit hit " << record_limit_hit << logendl;

    if (error)
        return std::make_tuple (parsed_bytes_sum, num_blocks, true, true); // error and done
    else
        return std::make_tuple (parsed_bytes_sum, num_blocks, false, !record_limit_hit);
}

std::pair<size_t, size_t> ASTERIXParser::decodeDataBlocks (const char* data, nlohmann::json& data_blocks, bool debug)
{
    if (debug)
        loginf << "ASTERIXParser: decodeDataBlocks" << logendl;

    std::pair<size_t, size_t> ret {0,0}; // num records, num errors

    if (!data_blocks.is_array())
        throw runtime_error("asterix parser data blocks is not an array");

    size_t num_data_blocks = data_blocks.size();

    std::vector<std::pair<size_t, size_t>> num_records;
    num_records.resize(num_data_blocks, {0,0});

    if (debug) // switch to single thread in debug
    {
        for (size_t cnt=0; cnt < num_data_blocks; ++cnt)
            num_records.at(cnt) = decodeDataBlock (data, data_blocks[cnt], debug);
    }
    else
    {
        tbb::parallel_for( size_t(0), num_data_blocks, [&]( size_t cnt )
        {
            num_records.at(cnt) = decodeDataBlock (data, data_blocks[cnt], debug);
        });
    }

    for (auto num_record_it : num_records)
    {
        ret.first += num_record_it.first;
        ret.second += num_record_it.second;
    }

    if (debug)
        loginf << "ASTERIXParser: decodeDataBlocks: done" << logendl;

    return ret;
}

std::pair<size_t, size_t> ASTERIXParser::decodeDataBlock (const char* data, nlohmann::json& data_block, bool debug)
{
    if (debug)
        loginf << "ASTERIXParser: decodeDataBlock" << logendl;

    size_t parsed_bytes {0}; // TODO unsure if used
    std::pair<size_t, size_t> ret {0,0}; // num records, num errors

    // check record information
    //json& record = target.at(data_block_name_);

    //        {
    //            "category": 1,
    //            "content": {
    //                "index": 3265,
    //                "length": 52
    //            },
    //            "length": 55
    //        }

    if (data_block.find ("category") == data_block.end())
    {
        loginf << "asterix parser data block '" << data_block.dump(4)
               << "' does not contain category information" << logendl;
        return {0, 1};
    }

    unsigned int cat = data_block.at("category");

    if (data_block.find ("content") == data_block.end())
    {
        loginf << "asterix parser data block '" << data_block.dump(4)
               << "' does not contain content information" << logendl;
        return {0, 1};
    }

    json& data_block_content = data_block.at("content");

    if (data_block_content.find ("index") == data_block_content.end())
    {
        loginf << "asterix parser data block '" << data_block.dump(4)
               << "' does not contain content index information" << logendl;
        return {0, 1};
    }

    size_t record_index = data_block_content.at("index");

    if (data_block_content.find ("length") == data_block_content.end())
    {
        loginf << "asterix parser data block '" << data_block.dump(4)
               << "' does not contain content length information" << logendl;
        return {0, 1};
    }

    size_t record_length = data_block_content.at("length");

    if (debug)
        loginf << "ASTERIXParser: decodeDataBlock: index " << record_index << " length " << record_length
               << " data '" << binary2hex((const unsigned char*)&data[record_index], record_length) << "'" << logendl;

    // try to decode
    if (records_.count(cat) != 0)
    {
        size_t parsed_bytes_record {0};

        try
        {
            // decode
            if (debug)
                loginf << "asterix parser decoding record with cat " << cat << " index " << record_index
                       << " length " << record_length << logendl;

            data_block_content["records"] = json::array();

            // create records until end of content
            while (parsed_bytes_record < record_length)
            {
                //loginf << "asterix parser decoding record " << cnt << " parsed bytes " << parsed_bytes_record << " length " << record_length;

                parsed_bytes_record += records_.at(cat)->parseItem(
                            data, record_index+parsed_bytes_record, record_length-parsed_bytes_record,
                            parsed_bytes+parsed_bytes_record,
                            data_block_content.at("records")[ret.first], debug);

                if (debug)
                    loginf << "asterix parser decoding record with cat " << cat << " index " << record_index
                           << ": " << data_block_content.at("records")[ret.first].dump(4) << "'" << logendl;
                ++ret.first;
            }
        }
        catch (std::exception& e)
        {
            loginf << "asterix parser decoding of cat " << cat << " failed with exception: '"
                   << e.what() << "'"  << "' after index " << record_index+parsed_bytes_record << logendl;
            ++ret.second;
        }
    }
    else if (debug)
        loginf << "asterix parser decoding record with cat " << cat << " index " << record_index
               << " length " << record_length << " skipped since cat definition is missing " << logendl;

    if (ret.first && mappings_.count(cat))
    {
        if (debug)
            loginf << "asterix parser decoding mapping cat " << cat << ", num records " << ret.first << logendl;

        std::shared_ptr<Mapping> current_mapping = mappings_.at(cat);
        json& mapping_src = data_block_content.at("records");
        json mapping_dest = json::array();

        for (size_t cnt=0; cnt < ret.first; ++cnt)
            current_mapping->map(mapping_src[cnt], mapping_dest[cnt]);

        data_block_content["records"] = std::move(mapping_dest);

    }

    if (debug)
        loginf << "ASTERIXParser: decodeDataBlock: done num records " << ret.first
               << " errors " << ret.second << logendl;

    return ret;
}

}
