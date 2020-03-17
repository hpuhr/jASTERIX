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

#ifndef FIXEDBYTESITEMPARSER_H
#define FIXEDBYTESITEMPARSER_H

#include "itemparserbase.h"

namespace jASTERIX
{
class FixedBytesItemParser : public ItemParserBase
{
  public:
    FixedBytesItemParser(const nlohmann::json& item_definition);
    virtual ~FixedBytesItemParser() {}

    virtual size_t parseItem(const char* data, size_t index, size_t size,
                             size_t current_parsed_bytes, nlohmann::json& target,
                             bool debug) override;

  protected:
    size_t length_{0};
    std::string data_type_;
    bool reverse_bits_{false};
    bool reverse_bytes_{false};
    unsigned int negative_bit_pos_{0};
    bool has_lsb_{false};
    double lsb_{1.0};
};

}  // namespace jASTERIX

#endif  // FIXEDBYTESITEMPARSER_H
