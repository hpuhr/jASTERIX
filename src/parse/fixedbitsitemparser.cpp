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

#include "fixedbitsitemparser.h"

using namespace std;
using namespace nlohmann;

namespace jASTERIX
{

FixedBitsItemParser::FixedBitsItemParser (const nlohmann::json& item_definition, unsigned int byte_length)
 : ItemParser (item_definition), byte_length_(byte_length)
{
    assert (type_ == "fixed_bits");

    if (item_definition.find("start_bit") == item_definition.end())
        throw runtime_error ("fixed byte bitfield item '"+name_+"' without start bit");

    start_bit_ = item_definition.at("start_bit");

    if (item_definition.find("bit_length") == item_definition.end())
        throw runtime_error ("fixed byte bitfield item '"+name_+"' without bit length");

    bit_length_ = item_definition.at("bit_length");

    if (bit_length_ == 0)
        throw runtime_error ("fixed byte bitfield item '"+name_+"' with 0 bit length");

    if (start_bit_+bit_length_ > byte_length*8)
        throw runtime_error ("fixed byte bitfield item '"+name_+"' wrong length "+to_string(byte_length*8)
                             +" for bitsize "+to_string(start_bit_+bit_length_));


    if (item_definition.find("data_type") != item_definition.end())
        data_type_ = item_definition.at("data_type");

    negative_bit_pos_ = start_bit_+bit_length_;

    if (data_type_ == "uint" || data_type_ == "int")
    {
        if (byte_length_ == 1)
        {
            bitmask1 = 1;
            for (unsigned cnt=0; cnt < bit_length_-1; ++cnt)
            {
                bitmask1 <<= 1;
                bitmask1 += 1;
            }
            bitmask1 <<= start_bit_;
        }
        else if (byte_length_ <= 4)
        {
            bitmask4 = 1;
            for (unsigned cnt=0; cnt < bit_length_-1; ++cnt)
            {
                bitmask4 <<= 1;
                bitmask4 += 1;
            }
            bitmask4 <<= start_bit_;
        }
        else if (byte_length_ <= 8)
        {
            bitmask8 = 1;
            for (unsigned cnt=0; cnt < bit_length_-1; ++cnt)
            {
                bitmask8 <<= 1;
                bitmask8 += 1;
            }
            bitmask8 <<= start_bit_;
        }
        else
            throw runtime_error ("fixed byte bitfield item '"+name_+"' with length"+to_string(byte_length_));
    }
    else if (data_type_ == "digits")
    {
        if (start_bit_+num_digits_*digit_bit_length_ > byte_length*8)
            throw runtime_error ("fixed byte bitfield item '"+name_+"' wrong length "+to_string(byte_length*8)
                                 +" for digits bitsize "+to_string(start_bit_+num_digits_*digit_bit_length_));

        if (item_definition.find("num_digits") == item_definition.end())
            throw runtime_error ("fixed byte bitfield item '"+name_+"' data type digits without number of digits");

        num_digits_ = item_definition.at("num_digits");

        if (item_definition.find("digit_bit_length") == item_definition.end())
            throw runtime_error ("fixed byte bitfield item '"+name_+"' data type digits without digit bit length");

        digit_bit_length_ = item_definition.at("digit_bit_length");

        if (byte_length_ == 1)
        {
            bitmask1 = 1;
            for (unsigned cnt=0; cnt < digit_bit_length_-1; ++cnt)
            {
                bitmask1 <<= 1;
                bitmask1 += 1;
            }
            bitmask1 <<= start_bit_;

            for (unsigned cnt=0; cnt < num_digits_; ++cnt)
            {
                digits_bitmasks1.push_back(bitmask1);
                bitmask1 <<= digit_bit_length_;
            }

            bitmask1 = 0;
            assert (digits_bitmasks1.size() == num_digits_);
        }
        else if (byte_length_ <= 4)
        {
            bitmask4 = 1;
            for (unsigned cnt=0; cnt < digit_bit_length_-1; ++cnt)
            {
                bitmask4 <<= 1;
                bitmask4 += 1;
            }

            bitmask4 <<= start_bit_;

            for (unsigned cnt=0; cnt < num_digits_; ++cnt)
            {
                digits_bitmasks4.push_back(bitmask4);
                bitmask4 <<= digit_bit_length_;
            }

            bitmask4 = 0;
            assert (digits_bitmasks4.size() == num_digits_);
        }
        else if (byte_length_ <= 8)
        {
            bitmask8 = 1;
            for (unsigned cnt=0; cnt < digit_bit_length_-1; ++cnt)
            {
                bitmask8 <<= 1;
                bitmask8 += 1;
            }

            bitmask8 <<= start_bit_;

            for (unsigned cnt=0; cnt < num_digits_; ++cnt)
            {
                digits_bitmasks8.push_back(bitmask8);
                bitmask8 <<= digit_bit_length_;
            }

            bitmask8 = 0;
            assert (digits_bitmasks8.size() == num_digits_);
        }
        else
            throw runtime_error ("fixed byte bitfield item '"+name_+"' with length"+to_string(byte_length_));
    }
    else
        throw runtime_error ("fixed byte bitfield item '"+name_+"' with unknown data type '"+data_type_+"'");
}

size_t FixedBitsItemParser::parseItem (const char* data, size_t index, size_t size, size_t current_parsed_bytes,
                              nlohmann::json& target, bool debug)
{
    if (debug)
        loginf << "parsing fixed bits item '" << name_ << "' byte length " << byte_length_;

    if (byte_length_ == 1)
    {
        unsigned char tmp1 = *reinterpret_cast<const unsigned char*> (&data[index]);

        if (data_type_ == "uint")
        {
            tmp1 &= bitmask1;
            tmp1 >>= start_bit_;

            if (debug)
                loginf << "parsing fixed bits item '" << name_ << "' with start bit " << start_bit_
                       << " length " << bit_length_ << " value " << (size_t) tmp1;

            target.emplace(name_, tmp1);
        }
        else if (data_type_ == "int")
        {
            tmp1 &= bitmask1;
            tmp1 >>= start_bit_;

            if (debug)
                loginf << "parsing fixed bits item '" << name_ << "' with start bit " << start_bit_
                       << " length " << bit_length_ << " value " << (size_t) tmp1;

            int data_int;

            if ((tmp1 & (1 << negative_bit_pos_)) != 0)
                data_int = tmp1 | ~((1 << negative_bit_pos_) - 1);
            else
                data_int = tmp1;

            target.emplace(name_, data_int);
        }
        else if (data_type_ == "digits")
        {
            size_t digits_tmp {0};

            for (unsigned int cnt=0; cnt < num_digits_; ++cnt)
            {
                digits_tmp *= 10;
                digits_tmp += tmp1 & digits_bitmasks1[cnt];
            }
            target.emplace(name_, digits_tmp);
        }
        else
            throw runtime_error ("fixed bits item '"+name_+"' parsing with unknown data type '"+data_type_+"'");
    }
    else if (byte_length_ <= 4)
    {
        unsigned int tmp4 = *reinterpret_cast<const unsigned int*> (&data[index]);

        if (data_type_ == "uint")
        {
            tmp4 &= bitmask4;
            tmp4 >>= start_bit_;

            if (debug)
                loginf << "parsing fixed bits item '" << name_ << "' with start bit " << start_bit_
                       << " length " << bit_length_ << " value " << (size_t) tmp4;

            target.emplace(name_, tmp4);
        }
        else if (data_type_ == "int")
        {
            tmp4 &= bitmask4;
            tmp4 >>= start_bit_;

            if (debug)
                loginf << "parsing fixed bits item '" << name_ << "' with start bit " << start_bit_
                       << " length " << bit_length_ << " value " << (size_t) tmp4;

            int data_int;

            if ((tmp4 & (1 << negative_bit_pos_)) != 0)
                data_int = tmp4 | ~((1 << negative_bit_pos_) - 1);
            else
                data_int = tmp4;

            target.emplace(name_, data_int);
        }
        else if (data_type_ == "digits")
        {
            size_t digits_tmp {0};

            for (unsigned int cnt=0; cnt < num_digits_; ++cnt)
            {
                digits_tmp *= 10;
                digits_tmp += tmp4 & digits_bitmasks4[cnt];
            }
            target.emplace(name_, digits_tmp);
        }
        else
            throw runtime_error ("fixed bits item '"+name_+"' parsing with unknown data type '"+data_type_+"'");

    }
    else if (byte_length_ <= 8)
    {
        size_t tmp8 = *reinterpret_cast<const size_t*> (&data[index]);

        if (data_type_ == "uint")
        {
            tmp8 &= bitmask8;
            tmp8 >>= start_bit_;

            if (debug)
                loginf << "parsing fixed bits item '" << name_ << "' with start bit " << start_bit_
                       << " length " << bit_length_ << " value " << (size_t) tmp8;

            target.emplace(name_, tmp8);
        }
        else if (data_type_ == "int")
        {
            tmp8 &= bitmask8;
            tmp8 >>= start_bit_;

            if (debug)
                loginf << "parsing fixed bits item '" << name_ << "' with start bit " << start_bit_
                       << " length " << bit_length_ << " value " << (size_t) tmp8;

            long int data_lint;

            if ((tmp8 & (1 << negative_bit_pos_)) != 0)
                data_lint = tmp8 | ~((1 << negative_bit_pos_) - 1);
            else
                data_lint = tmp8;

            target.emplace(name_, data_lint);
        }
        else if (data_type_ == "digits")
        {
            size_t digits_tmp {0};

            for (unsigned int cnt=0; cnt < num_digits_; ++cnt)
            {
                digits_tmp *= 10;
                digits_tmp += tmp8 & digits_bitmasks8[cnt];
            }
            target.emplace(name_, digits_tmp);
        }
        else
            throw runtime_error ("fixed bits item '"+name_+"' parsing with unknown data type '"+data_type_+"'");
    }

    return 0;
}

}
