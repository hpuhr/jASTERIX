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

#include "frameparser.h"
#include "logger.h"
#include "asterixparser.h"
#include "itemparserbase.h"

#include <tbb/tbb.h>

using namespace std;
using namespace nlohmann;

namespace jASTERIX {

FrameParser::FrameParser(const json& framing_definition, ASTERIXParser& asterix_parser, bool debug)
    : asterix_parser_(asterix_parser)
{
    if (framing_definition.find("name") == framing_definition.end())
        throw runtime_error ("frame parser construction without JSON name definition");

    if (framing_definition.find("header_items") == framing_definition.end())
        throw runtime_error ("frame parser construction without header items");

    if (!framing_definition.at("header_items").is_array())
        throw runtime_error ("frame parser construction with header items non-array");

    std::string item_name;
    ItemParserBase* item {nullptr};

    for (const json& data_item_it : framing_definition.at("header_items"))
    {
        item_name = data_item_it.at("name");

        if (debug)
            loginf << "frame parser constructing header item '" << item_name << "'" << logendl;

        item = ItemParserBase::createItemParser(data_item_it);
        assert (item);
        header_items_.push_back(std::unique_ptr<ItemParserBase>{item});
    }

    if (framing_definition.find("frame_items") == framing_definition.end())
        throw runtime_error ("frame parser construction without frame items");

    if (!framing_definition.at("frame_items").is_array())
        throw runtime_error ("frame parser construction with frame items non-array");

    for (const json& data_item_it : framing_definition.at("frame_items"))
    {
        item_name = data_item_it.at("name");

        if (debug)
            loginf << "frame parser constructing frame item '" << item_name << "'" << logendl;

        item = ItemParserBase::createItemParser(data_item_it);
        assert (item);
        frame_items_.push_back(std::unique_ptr<ItemParserBase>{item});
    }
}

size_t FrameParser::parseHeader (const char* data, size_t index, size_t size, json& target, bool debug)
{
    assert (data);
    assert (size);
    assert (index < size);
    //assert (target != nullptr);

    size_t parsed_bytes {0};

    for (auto& j_item : header_items_)
    {
        parsed_bytes += j_item->parseItem(data, index+parsed_bytes, size, parsed_bytes, target, debug);
    }

    return parsed_bytes;
}


size_t FrameParser::parseFrames (const char* data, size_t index, size_t size, nlohmann::json& target, size_t num_frames,
                 bool debug)
{
    assert (data);
    assert (size);
    assert (index < size);
    assert (target != nullptr);
    assert (!done_);

    size_t parsed_bytes {0};
    size_t current_parsed_bytes {0};
    size_t frames_cnt {0};

    if (debug)
        loginf << "parsing frames index " << index << " num_frames " << num_frames;

    nlohmann::json& j_frames = target["frames"];

    while (index+parsed_bytes < size && frames_cnt < num_frames)
    {
        current_parsed_bytes = 0;
        for (auto& j_item : frame_items_)
        {
            if (debug)
                loginf << "parsing frame at index " << index+parsed_bytes << " cnt " << frames_cnt << logendl;

            parsed_bytes += j_item->parseItem(data, index+parsed_bytes, size, current_parsed_bytes,
                                              j_frames[frames_cnt], debug);
            j_frames[frames_cnt]["cnt"] = frames_cnt;
        }
        ++frames_cnt;
    }

    if (index+parsed_bytes == size)
        done_ = true;

    return parsed_bytes;
}

size_t FrameParser::decodeFrames (const char* data, json& target, bool debug)
{
    assert (data);
    assert (target != nullptr);

    size_t num_records_sum {0};
    nlohmann::json& j_frames = target.at("frames");

    if (debug) // switch to single thread in debug
    {
        for (json& frame_it : j_frames)
        {
            num_records_sum += decodeFrame (data, frame_it, debug);
        }
    }
    else
    {
        size_t num_frames = j_frames.size();
        std::vector<size_t> num_records;
        num_records.resize(num_frames, 0);

        tbb::parallel_for( size_t(0), num_frames, [&]( size_t cnt )
        {
            num_records.at(cnt) = decodeFrame (data, j_frames.at(cnt), debug);
        });

        for (auto num_record_it : num_records)
            num_records_sum += num_record_it;
    }

    if (debug)
        loginf << "frames decoded, num frames " << num_records_sum << logendl;

    return num_records_sum;
}

bool FrameParser::done() const
{
    return done_;
}

size_t FrameParser::decodeFrame (const char* data, json& json_frame, bool debug)
{
    if (debug && json_frame.find("content") == json_frame.end())
        throw runtime_error("frame parser scoped frames does not contain correct content");

    json& frame_content = json_frame.at("content");

    size_t index {frame_content.at("index")};
    size_t length {frame_content.at("length")};

    if (debug)
        loginf << "frame parser decoding frame at index " << index << " length " << length << logendl;

    return asterix_parser_.decodeDataBlock(data, index, length, frame_content, debug);
}

}
