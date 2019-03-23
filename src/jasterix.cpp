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

#include "jasterix.h"
#include "files.h"
#include "logger.h"
#include "asterixparser.h"
#include "frameparser.h"
#include "frameparsertask.h"
#include "category.h"
#include "edition.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>

using namespace nlohmann;

namespace jASTERIX {

int print_dump_indent=4;
int frame_limit=-1;
int frame_chunk_size=1000;
int data_write_size=100;

using namespace Files;
using namespace std;

jASTERIX::jASTERIX(const std::string& definition_path, bool print, bool debug)
    : definition_path_(definition_path), print_(print), debug_(debug)
{

    // check framing definitions
    if (!directoryExists(definition_path_))
        throw invalid_argument ("jASTERIX called with non-existing definition path '"+definition_path_+"'");

    if (!directoryExists(definition_path_+"/framings"))
        throw invalid_argument ("jASTERIX called with incorrect definition path '"+definition_path_
                                     +"', framings are missing");

    if (!fileExists(definition_path_+"/data_block_definition.json"))
        throw invalid_argument ("jASTERIX called without asterix data block definition");

    // check asterix definitions

    if (!directoryExists(definition_path_+"/categories"))
        throw invalid_argument ("jASTERIX called with incorrect definition path '"+definition_path_
                                     +"', categories are missing");

    if (!fileExists(definition_path_+"/categories/categories.json"))
        throw invalid_argument ("jASTERIX called without asterix categories list definition");

    try // asterix record definition
    {
        data_block_definition_ = json::parse(ifstream(definition_path_+"/data_block_definition.json"));
    }
    catch (json::exception& e)
    {
        throw runtime_error (string{"jASTERIX parsing error in asterix data block definition: "}+e.what());
    }

    try // asterix categories list definition
    {
        categories_definition_ = json::parse(ifstream(definition_path_+"/categories/categories.json"));
    }
    catch (json::exception& e)
    {
        throw runtime_error (string{"jASTERIX parsing error in asterix categories definition: "}+e.what());
    }

    if (!categories_definition_.is_object())
        throw invalid_argument ("jASTERIX called with non-object asterix categories list definition");

    try // asterix category definitions
    {
        std::string cat_str;
        int cat;

        for (auto cat_def_it = categories_definition_.begin(); cat_def_it != categories_definition_.end();
             ++cat_def_it)
        {
            cat = -1;
            cat_str = cat_def_it.key();
            cat = stoi(cat_str);

            if (cat < 0 || cat > 255 || current_category_editions_.count(cat) != 0)
                throw invalid_argument ("jASTERIX called with wrong asterix category '"+cat_str+"' in list definition");

            if (debug)
                loginf << "jASTERIX found asterix category " << cat_str << logendl;


            try
            {
                category_definitions_.emplace(std::piecewise_construct,
                                  std::forward_as_tuple(cat_str),
                                  std::forward_as_tuple(cat_str, cat_def_it.value(), definition_path_));

                assert (category_definitions_.count(cat_str) == 1);

                current_category_editions_[cat] = category_definitions_.at(cat_str).getCurrentEdition();

                if (category_definitions_.at(cat_str).hasCurrentMapping())
                    current_category_mappings_[cat] = category_definitions_.at(cat_str).getCurrentMapping();
            }
            catch (json::exception& e)
            {
                throw runtime_error ("jASTERIX parsing error in asterix category "+cat_str+": "+e.what());
            }
        }
    }
    catch (json::exception& e)
    {
        throw runtime_error (string{"jASTERIX parsing error in asterix category definitions: "}+e.what());
    }

}

jASTERIX::~jASTERIX()
{
#if USE_BOOST
    if(file_.is_open())
        file_.close();
#else
    if (file_buffer_)
    {
        delete file_buffer_;
        file_buffer_ = nullptr;
    }
#endif
}

bool jASTERIX::hasCategory(const std::string& cat_str)
{
    return category_definitions_.count(cat_str) == 1;
}

bool jASTERIX::hasEdition (const std::string& cat_str, const std::string& edition_str)
{
    if (!hasCategory(cat_str))
        return false;

    return category_definitions_.at(cat_str).hasEdition(edition_str);
}

void jASTERIX::setEdition (const std::string& cat_str, const std::string& edition_str)
{
    assert (hasEdition(cat_str, edition_str));

    int cat = -1;
    cat = stoi(cat_str);
    assert (cat > 0);

    current_category_editions_[cat] = category_definitions_.at(cat_str).edition(edition_str);
}

bool jASTERIX::hasMapping (const std::string& cat_str, const std::string& mapping_str)
{
    if (!hasCategory(cat_str))
        return false;

    return category_definitions_.at(cat_str).hasMapping(mapping_str);
}

void jASTERIX::setMapping (const std::string& cat_str, const std::string& mapping_str)
{
    int cat = -1;
    cat = stoi(cat_str);
    assert (cat > 0);

    if (!mapping_str.size()) // "" for no mapping
    {
        if (current_category_mappings_.count(cat))
            current_category_mappings_.erase(cat);
        return;
    }

    assert (hasMapping(cat_str, mapping_str));

    current_category_mappings_[cat] = category_definitions_.at(cat_str).mapping(mapping_str);
}


void jASTERIX::decodeFile (const std::string& filename, const std::string& framing,
                           std::function<void(nlohmann::json&, size_t, size_t)> callback)
{
    // check and open file
    if (!fileExists(filename))
        throw invalid_argument ("jASTERIX called with non-existing file '"+filename+"'");

    size_t file_size = fileSize (filename);

    if (!file_size)
        throw invalid_argument ("jASTERIX called with empty file '"+filename+"'");

    if (debug_)
        loginf << "jASTERIX: file " << filename << " size " << file_size << logendl;

#if USE_BOOST
    file_.open(filename, file_size);

    if(!file_.is_open())
        throw runtime_error ("jASTERIX unable to map file '"+filename+"'");

    const char* data = file_.data();
#else
    ifstream file (filename, ios::in | ios::binary);
    file_buffer_ = new char[file_size];
    file.read (file_buffer_, file_size);
    if (!file)
    {
        // An error occurred!
        throw runtime_error ("jASTERIX unable to read file '"+filename+"'");
    }
    file.close();

    char* data = file_buffer_;
#endif

    // check framing
    if (!fileExists(definition_path_+"/framings/"+framing+".json"))
        throw invalid_argument ("jASTERIX called with unknown framing '"+framing+"'");

    nlohmann::json framing_definition;

    try // create framing definition
    {
        framing_definition = json::parse(ifstream(definition_path_+"/framings/"+framing+".json"));
    }
    catch (json::exception& e)
    {
        throw runtime_error ("jASTERIX parsing error in framing definition '"+framing+"': "+e.what());
    }

    // create ASTERIX parser
    ASTERIXParser asterix_parser (data_block_definition_, current_category_editions_,
                                  current_category_mappings_, debug_);

    // create frame parser
    FrameParser frame_parser (framing_definition, asterix_parser, debug_);

    nlohmann::json json_header;

    size_t index;

    // parsing header
    index = frame_parser.parseHeader(data, 0, file_size, json_header, debug_);

    FrameParserTask* task = new (tbb::task::allocate_root()) FrameParserTask (
                *this, frame_parser, json_header, data, index, file_size, debug_);
    tbb::task::enqueue(*task);

    nlohmann::json data_chunk;

    while (1)
    {
        if (frame_parser.done() && data_chunks_.empty())
            break;

        if (data_chunks_.try_pop(data_chunk))
        {
            num_frames_ += data_chunk.at("frames").size();
            num_records_ += frame_parser.decodeFrames(data, data_chunk, debug_);

            if (debug_)
                loginf << "jASTERIX processing " << num_frames_ << " frames, " << num_records_ << " records" << logendl;

            if (print_)
                loginf << data_chunk.dump(print_dump_indent) << logendl;

            callback(data_chunk, num_frames_, num_records_);

            if (frame_limit > 0 && num_frames_ >= static_cast<unsigned>(frame_limit))
            {
                loginf << "jASTERIX processing hit framelimit" << logendl;
                break;
            }
        }
        else
        {
            if (debug_)
                loginf << "jASTERIX waiting" << logendl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

void jASTERIX::decodeASTERIX (const char* data, size_t size,
             std::function<void(nlohmann::json&, size_t, size_t)> callback)
{
    // create ASTERIX parser
    ASTERIXParser asterix_parser (data_block_definition_, current_category_editions_,
                                  current_category_mappings_, debug_);

    nlohmann::json data_chunk;

    asterix_parser.decodeDataBlock(data, 0, size, data_chunk, debug_);

    callback(data_chunk, 0, 0);
}


size_t jASTERIX::numFrames() const
{
    return num_frames_;
}

size_t jASTERIX::numRecords() const
{
    return num_records_;
}

void jASTERIX::addDataChunk (nlohmann::json& data_chunk)
{
    if (debug_)
    {
        loginf << "jASTERIX adding data chunk" << logendl;

        if (data_chunk.find("frames") == data_chunk.end())
            throw std::runtime_error ("jASTERIX scoped frames information contains no frames");

        if (!data_chunk.at("frames").is_array())
            throw std::runtime_error ("jASTERIX scoped frames information is not array");
    }

    data_chunks_.push(std::move(data_chunk));
}

}
