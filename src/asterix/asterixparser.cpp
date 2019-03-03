/*
 * This file is part of jASTERIX.
 *
 * jASTERIX is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * jASTERIX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with jASTERIX.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "asterixparser.h"
#include "logger.h"
#include "itemparserbase.h"
#include "record.h"

#include <iostream>
#include <iomanip>

using namespace std;
using namespace nlohmann;

namespace jASTERIX {

ASTERIXParser::ASTERIXParser(const nlohmann::json& data_block_definition,
                             const std::map<unsigned int, std::shared_ptr<Edition>>& asterix_category_definitions,
                             const std::map<unsigned int, std::shared_ptr<Mapping>>& mappings,
                             bool debug)
    : mappings_(mappings)
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
            loginf << "asterix parser constructing data block item '" << item_name << "'";

        item = ItemParserBase::createItemParser(data_item_it);
        assert (item);
        data_block_items_.push_back(std::unique_ptr<ItemParserBase>{item});
    }

    // asterix recird definitions

    for (auto& ast_cat_def_it : asterix_category_definitions)
    {
        if (debug)
            loginf << "asterix parser constructing cat '" << setfill('0') << setw(3) << ast_cat_def_it.first << "'";

        records_.insert(
                    std::pair<unsigned int, std::shared_ptr<Record>>
                    (ast_cat_def_it.first, std::shared_ptr<Record>{ast_cat_def_it.second->record()}));

//        if (ast_cat_def_it.second->hasCurrentMapping())
//        {
//            mappings_.insert(
//                        std::pair<unsigned int, std::shared_ptr<Mapping>>
//                        (ast_cat_def_it.first, std::shared_ptr<Mapping>{ast_cat_def_it.second->record()}));
//        }
    }
}

size_t ASTERIXParser::decodeDataBlock (const char* data, size_t index, size_t length, nlohmann::json& target,
                                       bool debug)
{
    size_t parsed_bytes {0};
    size_t num_records {0};

    for (auto& r_item : data_block_items_)
    {
        parsed_bytes += r_item->parseItem(data, index+parsed_bytes, length, parsed_bytes,
                                          target[data_block_name_], debug);
        //++record_cnt;
    }

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
        // check record information
        json& record = target.at(data_block_name_);

        if (debug && record.find ("category") == record.end())
            throw runtime_error("asterix parser record does not contain category information");

        unsigned int cat = record.at("category");

        if (debug && record.find ("content") == record.end())
            throw runtime_error("asterix parser record does not contain content information");

        json& data_block_content = record.at("content");

        if (debug && data_block_content.find ("index") == data_block_content.end())
            throw runtime_error("asterix parser record content does not contain index information");

        size_t record_index = data_block_content.at("index");

        if (debug && data_block_content.find ("length") == data_block_content.end())
            throw runtime_error("asterix parser record content does not contain length information");

        size_t record_length = data_block_content.at("length");

        // try to decode
        if (records_.count(cat) != 0)
        {
            // decode
            if (debug)
                loginf << "asterix parser decoding record with cat " << cat << " index " << record_index
                     << " length " << record_length;

            //const json& asterix_category_definition = asterix_category_definitions_.at(cat);

            //std::string record_content_name = asterix_category_definition.at("name");

            data_block_content["records"] = json::array();
            size_t cnt = 0;

            // TODO
            parsed_bytes = records_.at(cat)->parseItem(
                        data, record_index, record_length, parsed_bytes,
                        data_block_content.at("records")[cnt], debug);

            if (debug)
                loginf << "asterix parser decoding record with cat " << cat << " index " << record_index
                         << ": " << data_block_content.at("records")[cnt].dump(4) << "'";
            ++num_records;
        }
        else if (debug)
            loginf << "asterix parser decoding record with cat " << cat << " index " << record_index
                 << " length " << record_length << " skipped since cat definition is missing ";

        if (num_records && mappings_.count(cat))
        {
            if (debug)
                loginf << "asterix parser decoding mapping cat " << cat << ", num records " << num_records;

            std::shared_ptr<Mapping> current_mapping = mappings_.at(cat);
            json& mapping_src = data_block_content.at("records");
            json mapping_dest = json::array();

            for (size_t cnt=0; cnt < num_records; ++cnt)
                current_mapping->map(mapping_src[cnt], mapping_dest[cnt]);

            data_block_content["records"] = std::move(mapping_dest);

        }

        return num_records;
}

}
