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

    if (item_definition.find("additative_factor") != item_definition.end())
    {
        has_additative_factor_ = true;
        additative_factor_ = item_definition.at("additative_factor");
    }
}

size_t DynamicBytesItemParser::parseItem (const char* data, size_t index, size_t size, size_t current_parsed_bytes,
                              nlohmann::json& target, bool debug)
{
    if (debug)
        loginf << "parsing dynamic bytes item '" << name_ << "' substract " << substract_previous_
               << " additative " << has_additative_factor_ << logendl;

    if (debug && target.find(length_variable_name_) == target.end())
        throw runtime_error ("dynamic bytes item '"+name_+"' parsing without given length");

    size_t length = target.at(length_variable_name_);

//    loginf << "UGA dynamic bytes item '"+name_+"' index " << index << " size " << size << " length " << length
//           << " current pb " << current_parsed_bytes << logendl;

    if (substract_previous_)
    {
        assert (length >= current_parsed_bytes);

        if (debug)
            loginf << "parsing dynamic bytes item '" << name_ << "' substracting previous " << current_parsed_bytes
                   << logendl;

        length -= current_parsed_bytes;
    }

    if (has_additative_factor_)
    {
        if (debug)
            loginf << "parsing dynamic bytes item '" << name_ << "' substracting addtative " << additative_factor_
                   << logendl;


        length += additative_factor_;
    }

    if (debug)
        loginf << "parsing dynamic bytes item '"+name_+"' index " << index << " length " << length << logendl;

    assert (target.find(name_) == target.end());

    target.emplace(name_, json::object({ {"index", index}, {"length", length} }));

    if (debug)
        loginf << "parsed dynamic bytes item '"+name_+"' index " << index << " length " << length << logendl;

    return length;
}

}
