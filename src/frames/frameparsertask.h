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

#ifndef FRAMEPARSERTASK_H
#define FRAMEPARSERTASK_H

#include "jasterix.h"
#include "frameparser.h"

#include <tbb/tbb.h>

#include <exception>

namespace jASTERIX
{

class FrameParserTask : public tbb::task
{
public:
    FrameParserTask (jASTERIX& jasterix, FrameParser& frame_parser, const nlohmann::json& header,
                     const char* data, size_t index, size_t size, bool debug)
        : jasterix_(jasterix), frame_parser_(frame_parser), header_(header), data_(data), index_(index), size_(size),
          debug_(debug)
    {}

    /*override*/ tbb::task* execute()
    {
        while (!frame_parser_.done() || size_-index_ > 0)
        {
            nlohmann::json data_chunk = header_; // copy header

            assert (index_ < size_);

            index_ += frame_parser_.parseFrames(data_, index_, size_, data_chunk, debug_);

            assert (data_chunk != nullptr);

            if (data_chunk.find("frames") == data_chunk.end())
                throw std::runtime_error ("jASTERIX scoped frames information contains no frames");

            if (!data_chunk.at("frames").is_array())
                throw std::runtime_error ("jASTERIX scoped frames information is not array");

            jasterix_.addDataChunk(data_chunk);
        }

        return nullptr; // or a pointer to a new task to be executed immediately
    }

private:
    jASTERIX& jasterix_;
    FrameParser& frame_parser_;
    const nlohmann::json& header_;
    const char* data_;
    size_t index_;
    size_t size_;
    bool debug_;
};

}

#endif // FRAMEPARSERTASK_H
