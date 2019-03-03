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

#ifndef JASTERIX_H
#define JASTERIX_H

#include <string>
#include <map>
#include <boost/iostreams/device/mapped_file.hpp>
#include "tbb/concurrent_queue.h"

#include "json.hpp"
#include "frameparser.h"
#include "edition.h"
#include "mapping.h"

namespace jASTERIX {

class Category;

class jASTERIX
{
public:
    jASTERIX( const std::string& definition_path, bool print, bool debug);
    virtual ~jASTERIX();

    void decodeFile (const std::string& filename, const std::string& framing_str,
                     std::function<void(nlohmann::json&&, size_t, size_t)> callback);
    // callback gets moved chunk, accumulated number of frames, number of records

    void decodeASTERIX (const char* data, size_t size,
                 std::function<void(nlohmann::json&&, size_t, size_t)> callback);

    size_t numFrames() const;
    size_t numRecords() const;

    void addDataChunk (nlohmann::json& data_chunk);

private:
    std::string definition_path_;
    bool print_ {false};
    bool debug_ {false};

    nlohmann::json data_block_definition_;
    nlohmann::json categories_definition_;
    std::map<std::string, Category> category_definitions_;
    std::map<unsigned int, std::shared_ptr<Edition>> current_category_editions_; // cat -> edition
    std::map<unsigned int, std::shared_ptr<Mapping>> current_category_mappings_; // cat -> edition

    boost::iostreams::mapped_file_source file_;

    tbb::concurrent_queue<nlohmann::json> data_chunks_;

    size_t num_frames_{0};
    size_t num_records_{0};
};
}

#endif // JASTERIX_H
