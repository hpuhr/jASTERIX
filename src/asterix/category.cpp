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


#include "category.h"
#include "edition.h"
#include "logger.h"

using namespace std;
using namespace nlohmann;

namespace jASTERIX
{

Category::Category(const std::string& number, const nlohmann::json& definition, const std::string& definition_path)
    : number_(number)
{
    //    "comment": "Category 048 Transmission of Monoradar Target Reports",

    if (definition.find("comment") == definition.end())
        throw runtime_error ("category '"+number_+"' has no comment");

    comment_ = definition.at("comment");

    //    "default_edition" : "1.15",

    if (definition.find("default_edition") == definition.end())
        throw runtime_error ("category '"+number_+"' has no default edition");

    default_edition_ = definition.at("default_edition");

    //    "editions":

    if (definition.find("editions") == definition.end())
        throw runtime_error ("category '"+number_+"' has no editions");

    const json& edition_definitions = definition.at("editions");

    if (!edition_definitions.is_object())
        throw invalid_argument ("category '"+number_+"' with non-object edition definition");

    for (auto ed_def_it = edition_definitions.begin(); ed_def_it != edition_definitions.end();
         ++ed_def_it)
    {
        editions_[ed_def_it.key()] = std::shared_ptr<Edition> (
                    new Edition(ed_def_it.key(), ed_def_it.value(), definition_path));
    }

    if (editions_.count(default_edition_) != 1)
        throw invalid_argument ("category '"+number_+"' default edition '"+default_edition_+"' not defined");

    //    "default_mapping" : "1.0",

    if (definition.find("default_mapping") == definition.end())
        throw runtime_error ("category '"+number_+"' has no default mapping");

    default_mapping_ = definition.at("default_mapping");

    //    "mappings":

    if (definition.find("mappings") == definition.end())
        throw runtime_error ("category '"+number_+"' has no mappings");

    const json& mapping_definitions = definition.at("mappings");

    if (!mapping_definitions.is_object())
        throw invalid_argument ("category '"+number_+"' with non-object mapping definition");

    for (auto map_def_it = mapping_definitions.begin(); map_def_it != mapping_definitions.end();
         ++map_def_it)
    {
        mappings_[map_def_it.key()] = std::shared_ptr<Mapping> (
                    new Mapping(map_def_it.key(), map_def_it.value(), definition_path));
    }

    if (default_mapping_.size() && mappings_.count(default_mapping_) != 1)
        throw invalid_argument ("category '"+number_+"' default mapping '"+default_mapping_+"' not defined");
}

std::string Category::number() const
{
    return number_;
}

std::string Category::comment() const
{
    return comment_;
}

std::string Category::defaultEdition() const
{
    return default_edition_;
}

std::shared_ptr<Edition> Category::getCurrentEdition()
{
    assert (editions_.count(default_edition_) == 1);
    return editions_.at(default_edition_);
}

std::string Category::defaultMapping() const
{
    return default_mapping_;
}

bool Category::hasCurrentMapping()
{
    if (default_mapping_.size() == 0)
        return false;

    return mappings_.count(default_mapping_) == 1;
}

std::shared_ptr<Mapping> Category::getCurrentMapping()
{
    assert (hasCurrentMapping());
    return mappings_.at(default_mapping_);
}

}
