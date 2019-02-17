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

#include <exception>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>

using namespace nlohmann;

namespace jASTERIX {

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
        asterix_list_definition_ = json::parse(ifstream(definition_path_+"/categories/categories.json"));
    }
    catch (json::exception& e)
    {
        throw runtime_error (string{"jASTERIX parsing error in asterix categories list definition: "}+e.what());
    }

    if (!asterix_list_definition_.is_object())
        throw invalid_argument ("jASTERIX called with non-object asterix categories list definition");

    try // asterix category definitions
    {
        std::string cat_str;
        std::string file_str;
        int cat;

        for (auto ast_def_it = asterix_list_definition_.begin(); ast_def_it != asterix_list_definition_.end();
             ++ast_def_it)
        {
            cat = -1;
            cat_str = ast_def_it.key();
            file_str = ast_def_it.value();
            cat = stoi(cat_str);

            if (cat < 0 || cat > 255 || asterix_category_definitions_.count(cat) != 0)
                throw invalid_argument ("jASTERIX called with wrong asterix category '"+cat_str+"' in list definition");

            if (debug)
                loginf << "jASTERIX found asterix category " << cat << " definition in '" << file_str << "'";

            try
            {
                if (debug)
                   loginf << "jASTERIX loading file from path '"+definition_path_+"/categories/"+file_str << "'";

                asterix_category_definitions_[cat] =
                        json::parse(ifstream(definition_path_+"/categories/"+file_str));
            }
            catch (json::exception& e)
            {
                throw runtime_error ("jASTERIX parsing error in asterix category "+cat_str
                                     +" definition '"+file_str+"':"+e.what());
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
    if(file_.is_open())
        file_.close();
}

void jASTERIX::decodeFile (const std::string& filename, const std::string& framing,
                           std::function<void(nlohmann::json&&, size_t, size_t)> callback)
{
    // check and open file
    if (!fileExists(filename))
        throw invalid_argument ("jASTERIX called with non-existing file '"+filename+"'");

    size_t file_size = fileSize (filename);

    if (!file_size)
        throw invalid_argument ("jASTERIX called with empty file '"+filename+"'");

    if (debug_)
        loginf << "jASTERIX: file " << filename << " size " << file_size;

    file_.open(filename, file_size);

    if(!file_.is_open())
        throw runtime_error ("jASTERIX unable to map file '"+filename+"'");

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
    ASTERIXParser asterix_parser (data_block_definition_, asterix_category_definitions_, debug_);

    // create frame parser
    FrameParser frame_parser (framing_definition, asterix_parser, debug_);

    nlohmann::json json_header;

    size_t index;

    // parsing header
    index = frame_parser.parseHeader(file_.data(), 0, file_size, json_header, debug_);

    FrameParserTask* task = new (tbb::task::allocate_root()) FrameParserTask (
                *this, frame_parser, json_header, file_.data(), index, file_size, debug_);
    tbb::task::enqueue(*task);

    nlohmann::json data_chunk;

    while (1)
    {
        if (frame_parser.done() && data_chunks_.empty())
            break;

        if (data_chunks_.try_pop(data_chunk))
        {
            if (debug_)
                loginf << "jASTERIX processing " << num_frames_ << " frames, " << num_records_ << " records";

            num_frames_ += data_chunk.at("frames").size();
            num_records_ += frame_parser.decodeFrames(file_.data(), data_chunk, debug_);

            if (print_)
                loginf << data_chunk.dump(4);

            callback(std::move(data_chunk), num_frames_, num_records_);
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
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
        loginf << "jASTERIX adding data chunk";

        if (data_chunk.find("frames") == data_chunk.end())
            throw std::runtime_error ("jASTERIX scoped frames information contains no frames");

        if (!data_chunk.at("frames").is_array())
            throw std::runtime_error ("jASTERIX scoped frames information is not array");
    }

    data_chunks_.push(std::move(data_chunk));
}

}
