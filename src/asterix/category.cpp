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

//        editions_.emplace(std::piecewise_construct,
//                          std::forward_as_tuple(ed_def_it.key()),
//                          std::forward_as_tuple(ed_def_it.key(), ed_def_it.value(), definition_path));

        //editions_.emplace(ed_def_it.key(), ed_def_it.value(), definition_path);
    }

    //        {
    //            "1.15":
    //            {
    //                "document": "SUR.ET1.ST05.2000-STD-04-01",
    //                "date": "April 2007",
    //                "file": "048/cat048_1.15.json"
    //            }
    //        }

    if (editions_.count(default_edition_) != 1)
        throw invalid_argument ("category '"+number_+"' default edition '"+default_edition_+"' not defined");

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

}
