#include "editionbase.h"
#include "files.h"

#include <fstream>

namespace jASTERIX {

using namespace Files;
using namespace std;
using namespace nlohmann;

EditionBase::EditionBase(const std::string& number, const nlohmann::json& definition,
                         const std::string& definition_path)
    : number_(number)
{
    //    "document": "SUR.ET1.ST05.2000-STD-04-01",
    if (!definition.contains("document"))
        throw runtime_error("edition '" + number_ + "' has no document reference");

    document_ = definition.at("document");

    //    "date": "April 2007",
    if (!definition.contains("date"))
        throw runtime_error("edition '" + number_ + "' has no date");

    date_ = definition.at("date");

    //    "file": "048/cat048_1.15.json"
    if (!definition.contains("file"))
        throw runtime_error("edition '" + number_ + "' has no file");

    file_ = definition.at("file");

    edition_definition_path_ = definition_path + "/categories/" + file_;

    if (!fileExists(edition_definition_path_))
        throw invalid_argument("edition " + number_ + " file '" + edition_definition_path_ +
                               "' not found");

    definition_ = json::parse(ifstream(edition_definition_path_));
}

EditionBase::~EditionBase() {}

std::string EditionBase::document() const { return document_; }

std::string EditionBase::date() const { return date_; }

std::string EditionBase::file() const { return file_; }

std::string EditionBase::definitionPath() const { return edition_definition_path_; }

std::string EditionBase::number() const { return number_; }

} // namespace jASTERIX
