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


#ifndef CATEGORY_H
#define CATEGORY_H

#include "json.hpp"
#include <jasterix/edition.h>
#include <jasterix/mapping.h>

#include <string>

namespace jASTERIX
{

class Category
{
public:
    Category(const std::string& number, const nlohmann::json& definition, const std::string& definition_path);
    virtual ~Category() {}

    std::string number() const;
    std::string comment() const;

    bool hasEdition (const std::string& edition_str) const;
    std::shared_ptr<Edition> edition (const std::string& edition_str);
    std::string editionPath (const std::string& edition_str) const;

    std::string defaultEdition() const;
    void setCurrentEdition (const std::string& edition_str);
    std::shared_ptr<Edition> getCurrentEdition();

    bool hasMapping (const std::string& mapping_str);
    std::shared_ptr<Mapping> mapping (const std::string& mapping_str);


    std::string defaultMapping() const;
    bool hasCurrentMapping();
    std::shared_ptr<Mapping> getCurrentMapping();

    const std::map<std::string, std::shared_ptr<Edition>>& editions() const;
    const std::map<std::string, std::shared_ptr<Mapping>>& mappings() const;

    bool decode() const;
    void decode (bool value);

protected:
    std::string number_;
    std::string comment_;
    std::string default_edition_;
    std::string current_edition_;
    std::string default_mapping_;
    bool decode_{true};

    std::map<std::string, std::shared_ptr<Edition>> editions_;
    std::map<std::string, std::shared_ptr<Mapping>> mappings_;
};

}
#endif // CATEGORY_H
