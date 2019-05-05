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

#ifndef JASTERIX_H
#define JASTERIX_H

#include <string>
#include <map>
#include <jasterix/global.h>

#if USE_BOOST
#include <boost/iostreams/device/mapped_file.hpp>
#endif

#include "tbb/concurrent_queue.h"

#include "json.hpp"
#include <jasterix/frameparser.h>
#include <jasterix/edition.h>
#include <jasterix/mapping.h>

namespace jASTERIX {

extern int print_dump_indent;
extern int frame_limit;
extern int frame_chunk_size;
extern int record_chunk_size;
extern int data_write_size;

class Category;

class jASTERIX
{
public:
    jASTERIX( const std::string& definition_path, bool print, bool debug);
    virtual ~jASTERIX();

    bool hasCategory(const std::string& cat_str);

    bool hasEdition (const std::string& cat_str, const std::string& edition_str);
    void setEdition (const std::string& cat_str, const std::string& edition_str);

    bool hasMapping (const std::string& cat_str, const std::string& mapping_str);
    void setMapping (const std::string& cat_str, const std::string& mapping_str);

    void decodeFile (const std::string& filename, const std::string& framing_str,
                     std::function<void(nlohmann::json&, size_t, size_t)> callback=nullptr);
    // callback gets moved chunk, accumulated number of frames, number of records
    void decodeFile (const std::string& filename,
                     std::function<void(nlohmann::json&, size_t, size_t)> callback=nullptr);

    void decodeASTERIX (const char* data, size_t size,
                 std::function<void(nlohmann::json&, size_t, size_t)> callback=nullptr);

    size_t numFrames() const;
    size_t numRecords() const;

    void addDataBlockChunk (nlohmann::json& data_block_chunk, bool done);
    void addDataChunk (nlohmann::json& data_chunk, bool done);

private:
    std::string definition_path_;
    bool print_ {false};
    bool debug_ {false};

    nlohmann::json data_block_definition_;
    nlohmann::json categories_definition_;
    std::map<std::string, Category> category_definitions_;
    std::map<unsigned int, std::shared_ptr<Edition>> current_category_editions_; // cat -> edition
    std::map<unsigned int, std::shared_ptr<Mapping>> current_category_mappings_; // cat -> edition

#if USE_BOOST
    boost::iostreams::mapped_file_source file_;
#else
    char* file_buffer_{nullptr};
#endif

    tbb::concurrent_queue<nlohmann::json> data_block_chunks_;
    bool data_block_processing_done_ {false};

    tbb::concurrent_queue<nlohmann::json> data_chunks_;
    bool data_processing_done_ {false};

    size_t num_frames_{0};
    size_t num_records_{0};
};
}

#endif // JASTERIX_H
