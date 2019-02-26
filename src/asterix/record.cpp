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

#include "record.h"

using namespace std;
using namespace nlohmann;

namespace jASTERIX
{

Record::Record (const nlohmann::json& item_definition)
 : ItemParserBase (item_definition)
{
    assert (type_ == "record");

    // fspec

    if (item_definition.find("field_specification") == item_definition.end())
        throw runtime_error ("record item '"+name_+"' parsing without field specification");

    const json& field_specification = item_definition.at("field_specification");

    if (!field_specification.is_object())
        throw runtime_error ("record item '"+name_+"' field specification is not an object");

    field_specification_.reset(ItemParserBase::createItemParser(field_specification));
    assert (field_specification_);

    // uap

    if (item_definition.find("uap") == item_definition.end())
        throw runtime_error ("record item '"+name_+"' without uap");

    const json& uap = item_definition.at("uap");

    if (!uap.is_array())
        throw runtime_error ("record item '"+name_+"' uap is not an array");

    for (const auto& uap_item : uap)
        uap_names_.push_back(uap_item);

    // items

    if (item_definition.find("items") == item_definition.end())
        throw runtime_error ("record item '"+name_+"' without items");

    const json& items = item_definition.at("items");

    if (!items.is_array())
        throw runtime_error ("record item '"+name_+"' field specification is not an array");

    std::string item_name;
    ItemParserBase* item {nullptr};

    for (const json& data_item_it : items)
    {
        item_name = data_item_it.at("name");
        item = ItemParserBase::createItemParser(data_item_it);
        assert (item);

        if (items_.count(item_name) != 0)
            throw runtime_error ("record item '"+name_+"' item name '"+item_name+"' used multiple times");

        items_[item_name] = std::unique_ptr<ItemParserBase>{item};
    }
}

size_t Record::parseItem (const char* data, size_t index, size_t size, size_t current_parsed_bytes,
                              nlohmann::json& target, bool debug)
{
    if (debug)
        loginf << "parsing record item '" << name_ << "' with " << items_.size() << " items";

    size_t parsed_bytes {0};

    if (debug)
        loginf << "parsing record item '"+name_+"' field specification";

    parsed_bytes = field_specification_->parseItem(data, index+parsed_bytes, size, parsed_bytes, target, debug);

    if (target.find("fspec") == target.end())
        throw runtime_error ("record item '"+name_+"' fspec not found");

    std::vector<bool> fspec_bits = target.at("fspec");

    if (fspec_bits.size() > uap_names_.size())
        throw runtime_error ("record item '"+name_+"' has more fspec bits than define uap items");

    if (debug)
        loginf << "parsing record item '"+name_+"' data items";

    std::string item_name;

    for (size_t cnt=0; cnt < fspec_bits.size(); ++cnt)
    {
        if (fspec_bits.at(cnt))
        {
            item_name = uap_names_.at(cnt);

            if (item_name == "FX" || item_name == "SP" || item_name == "RE")
                continue;

            if (debug)
                loginf << "parsing record item '" << name_ << "' data item '" << item_name << "' index "
                       << index+parsed_bytes;

            if (items_.count(item_name) != 1)
                throw runtime_error ("record item '"+name_+"' references undefined item '"+item_name+"'");

            parsed_bytes += items_.at(item_name)->parseItem(
                        data, index+parsed_bytes, size, parsed_bytes, target, debug);
        }
    }

    return parsed_bytes;
}

}
