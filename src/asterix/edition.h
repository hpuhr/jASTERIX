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


#ifndef EDITION_H
#define EDITION_H

#include "json.hpp"
#include "record.h"

#include <string>

namespace jASTERIX
{

class Record;

class Edition
{
public:
    Edition(const std::string& number, const nlohmann::json& definition, const std::string& definition_path);
    virtual ~Edition() {}

    std::string number() const;
    std::string document() const;
    std::string date() const;
    std::string file() const;

    std::shared_ptr<Record> record() const;

protected:
    std::string number_;
    std::string document_;
    std::string date_;
    std::string file_;

    nlohmann::json definition_; // from file
    std::shared_ptr<Record> record_;
};

}
#endif // EDITION_H
