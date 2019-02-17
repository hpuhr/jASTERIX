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

#include <iostream>
#include <iomanip>

using namespace std;
using namespace nlohmann;

namespace jASTERIX {

ASTERIXParser::ASTERIXParser(const nlohmann::json& data_block_definition,
                             const std::map<unsigned int, nlohmann::json>& asterix_category_definitions, bool debug)
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
    ItemParser* item {nullptr};

    for (const json& data_item_it : data_block_definition.at("items"))
    {
        item_name = data_item_it.at("name");

        if (debug)
            loginf << "frame parser constructing data block item '" << item_name << "'";

        item = ItemParser::createItemParser(data_item_it);
        assert (item);
        data_block_items_.push_back(std::unique_ptr<ItemParser>{item});
    }

    // asterix definitions

    for (auto& ast_cat_def_it : asterix_category_definitions)
    {

        if (debug)
            loginf << "frame parser constructing cat '" << setfill('0') << setw(3) << ast_cat_def_it.first << "'";

        item = ItemParser::createItemParser(ast_cat_def_it.second);
        assert (item);
        asterix_category_definitions_.insert(
                    std::pair<unsigned int, std::unique_ptr<ItemParser>>
                    (ast_cat_def_it.first, std::unique_ptr<ItemParser>{item}));
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
            throw runtime_error("frame parser record does not contain category information");

        unsigned int cat = record.at("category");

        if (debug && record.find ("content") == record.end())
            throw runtime_error("frame parser record does not contain content information");

        json& record_content = record.at("content");

        if (debug && record_content.find ("index") == record_content.end())
            throw runtime_error("frame parser record content does not contain index information");

        size_t record_index = record_content.at("index");

        if (debug && record_content.find ("length") == record_content.end())
            throw runtime_error("frame parser record content does not contain length information");

        size_t record_length = record_content.at("length");

        // try to decode
        if (asterix_category_definitions_.count(cat) != 0)
        {
            // decode
            if (debug)
                loginf << "frame parser decoding record with cat " << cat << " index " << record_index
                     << " length " << record_length;

            //const json& asterix_category_definition = asterix_category_definitions_.at(cat);

            //std::string record_content_name = asterix_category_definition.at("name");

            // TODO
            parsed_bytes = asterix_category_definitions_.at(cat)->parseItem(
                        data, record_index, record_length, parsed_bytes,
                        record_content[asterix_category_definitions_.at(cat)->name()], debug);

            if (debug)
                loginf << "frame parser decoding record with cat " << cat << " index " << record_index
                         << ": " << record_content.at(asterix_category_definitions_.at(cat)->name()).dump(4) << "'";
            ++num_records;
        }
        else if (debug)
            loginf << "frame parser decoding record with cat " << cat << " index " << record_index
                 << " length " << record_length << " skipped since cat definition is missing ";

        return num_records;
}

}
