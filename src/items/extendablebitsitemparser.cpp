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

#include "extendablebitsitemparser.h"

#include <algorithm>

#include "logger.h"

using namespace std;
using namespace nlohmann;

namespace jASTERIX
{
ExtendableBitsItemParser::ExtendableBitsItemParser(const nlohmann::json& item_definition,
                                                   const std::string& long_name_prefix)
    : ItemParserBase(item_definition, long_name_prefix)
{
    assert(type_ == "extendable_bits");

    if (!item_definition.contains("data_type"))
        throw runtime_error("extendable bits item '" + name_ + "' parsing without data type");

    data_type_ = item_definition.at("data_type");

    reverse_bits_ =
        (item_definition.contains("reverse_bits") && item_definition.at("reverse_bits") == true);

    reverse_order_ =
        (item_definition.contains("reverse_order") && item_definition.at("reverse_order") == true);
}

size_t ExtendableBitsItemParser::parseItem(const char* data, size_t index, size_t size,
                                           size_t current_parsed_bytes, size_t total_size,
                                           nlohmann::json& target, bool debug)
{
    if (debug)
        loginf << "parsing extendable bits item '" << name_ << "' index " << index << " size "
               << size << " current parsed bytes " << current_parsed_bytes << logendl;

    const char* current_data = &data[index];

    if (data_type_ == "bitfield")
    {
        unsigned int bitmask;
        std::vector<bool> bitfield;
        bool value = true;
        size_t parsed_bytes{0};

        while (value != false)  // last value is extension bit
        {
            const unsigned char current_byte =
                *reinterpret_cast<const unsigned char*>(&current_data[parsed_bytes]);

            if (reverse_bits_)
            {
                bitmask = 1;
                bitmask <<= 7;

                for (size_t cnt{0}; cnt < 8; ++cnt)
                {
                    value = current_byte & bitmask;
                    bitfield.push_back(value);

                    if (debug)
                        loginf << "extendable bits item '" << name_ << "' index "
                               << index + parsed_bytes << " current byte "
                               << static_cast<unsigned int>(current_byte) << " reverse true "
                               << " bit field index " << cnt << " bitmask " << bitmask << " value "
                               << value << logendl;

                    bitmask >>= 1;
                }
            }
            else
            {
                bitmask = 1;

                for (size_t cnt{0}; cnt < 8; ++cnt)
                {
                    value = current_byte & bitmask;
                    bitfield.push_back(value);

                    if (debug)
                        loginf << "extendable bits item '" << name_ << "' index "
                               << index + parsed_bytes << " current byte "
                               << static_cast<unsigned int>(current_byte) << " reverse false "
                               << " bit field index " << cnt << " bitmask " << bitmask << " value "
                               << value << logendl;

                    bitmask = bitmask << 1;
                }
            }
            ++parsed_bytes;
        }

        if (reverse_order_)
            std::reverse(bitfield.begin(), bitfield.end());

        target.emplace(name_, bitfield);

        if (debug)
            loginf << "extendable bits item '" + name_ + "'"
                   << " index " << index << " parsed " << parsed_bytes << " bytes" << logendl;

        return parsed_bytes;
    }
    else
        throw runtime_error("extentable bits item '" + name_ +
                            "' parsing with unknown data type '" + data_type_ + "'");
}

}  // namespace jASTERIX
