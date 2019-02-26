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


#ifndef CATEGORY_H
#define CATEGORY_H

#include "json.hpp"

#include <string>

namespace jASTERIX
{

class Edition;

class Category
{
public:
    Category(const std::string& number, const nlohmann::json& definition, const std::string& definition_path);
    virtual ~Category() {}

    std::string number() const;
    std::string comment() const;
    std::string defaultEdition() const;

    std::shared_ptr<Edition> getCurrentEdition();

protected:
    std::string number_;
    std::string comment_;
    std::string default_edition_;

    std::map<std::string, std::shared_ptr<Edition>> editions_;
};

}
#endif // CATEGORY_H
