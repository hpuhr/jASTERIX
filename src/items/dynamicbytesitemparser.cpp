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

#include "dynamicbytesitemparser.h"

using namespace std;
using namespace nlohmann;

namespace jASTERIX
{

DynamicBytesItemParser::DynamicBytesItemParser (const nlohmann::json& item_definition)
 : ItemParserBase (item_definition)
{
    assert (type_ == "dynamic_bytes");

    if (item_definition.find("length_variable") == item_definition.end())
        throw runtime_error ("dynamic bytes item '"+name_+"' parsing without length variable");

    length_variable_name_ = item_definition.at("length_variable");

    substract_previous_ = item_definition.find("substract_previous") != item_definition.end()
            && item_definition.at("substract_previous") == true;
}

size_t DynamicBytesItemParser::parseItem (const char* data, size_t index, size_t size, size_t current_parsed_bytes,
                              nlohmann::json& target, bool debug)
{
    if (debug)
        loginf << "parsing dynamic bytes item '" << name_ << "'" << logendl;

    if (debug && target.find(length_variable_name_) == target.end())
        throw runtime_error ("dynamic bytes item '"+name_+"' parsing without given length");

    size_t length = target.at(length_variable_name_);

    if (substract_previous_)
    {
        length -= current_parsed_bytes;
        assert (length - current_parsed_bytes >= 0);
    }

    if (debug)
        loginf << "parsing dynamic bytes item '"+name_+"' index " << index << " length " << length << logendl;

    assert (target.find(name_) == target.end());

    //target[name_] = { {"index", index}, {"length", length} };

    target.emplace(name_, json::object({ {"index", index}, {"length", length} }));
//    target[name_]["index"] = index;
//    target[name_]["length"] = length;

    return length;
}

}
