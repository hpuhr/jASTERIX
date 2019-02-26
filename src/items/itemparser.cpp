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

#include "itemparser.h"

using namespace std;
using namespace nlohmann;

namespace jASTERIX
{

ItemParser::ItemParser (const nlohmann::json& item_definition)
 : ItemParserBase (item_definition)
{
    assert (type_ == "item");

    const json& data_fields = item_definition.at("data_fields");

    if (!data_fields.is_array())
        throw runtime_error ("parsing item '"+name_+"' data fields container is not an array");

    std::string item_name;
    ItemParserBase* item {nullptr};

    for (const json& data_item_it : data_fields)
    {
        item_name = data_item_it.at("name");
        item = ItemParserBase::createItemParser(data_item_it);
        assert (item);
        data_fields_.push_back(std::unique_ptr<ItemParserBase>{item});
    }
}

size_t ItemParser::parseItem (const char* data, size_t index, size_t size, size_t current_parsed_bytes,
                              nlohmann::json& target, bool debug)
{
    if (debug)
        loginf << "parsing item '" << name_ << "'";

    size_t parsed_bytes {0};

    for (auto& df_item : data_fields_)
    {
        parsed_bytes += df_item->parseItem(data, index+parsed_bytes, size, current_parsed_bytes, target[name_], debug);
    }

    if (debug)
        loginf << "parsing item '"+name_+"' done, " << parsed_bytes << " bytes parsed";

    return parsed_bytes;
}

}
