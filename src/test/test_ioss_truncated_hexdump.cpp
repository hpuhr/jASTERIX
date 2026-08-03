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

#include "catch.hpp"
#include "files.h"
#include "jasterix.h"
#include "logger.h"
#include "test_jasterix.h"

using namespace std;
using namespace nlohmann;

// Regression test for the error-path hexdump reading past the end of the
// memory-mapped file buffer (Skyguide v1.0.0-HF2 crashes 4140321 / 629427).
//
// IOSS framing is pure length-chaining without sync markers, so a truncated
// recording (cut off mid-frame) leaves a final frame whose declared lengths
// no longer match the actual data. The test file contains one valid frame
// (one CAT002 record) followed by a frame truncated before its padding whose
// content holds a data block declaring length 0xFFFF with only 2 record bytes
// present. Parsing that record throws (item read past buffer end), which is
// handled - but the error log message then hexdumps the data block with the
// declared (bogus) length. Unclamped, that read runs up to 65 KB past the
// 35-byte mapped file and crashes on the first unmapped page; clamped, the
// decode must complete and report errors.
//
// The file must be decoded via decodeFile (not decodeData) so the buffer is
// memory-mapped and an out-of-bounds read actually faults.
TEST_CASE("jASTERIX IOSS truncated file error-path hexdump", "[bounds]")
{
    loginf << "ioss truncated hexdump test: start" << logendl;

    jASTERIX::jASTERIX jasterix(definition_path, false, false, false);

    REQUIRE(jasterix.hasCategory(2));
    std::shared_ptr<jASTERIX::Category> cat002 = jasterix.category(2);
    REQUIRE(cat002->hasEdition("1.0"));
    cat002->setCurrentEdition("1.0");

    const std::string filename = "ioss_truncated_hexdump.bin";

    REQUIRE(jASTERIX::Files::fileExists(data_path + filename));
    REQUIRE(jASTERIX::Files::fileSize(data_path + filename) == 35);

    bool callback_called = false;
    size_t sum_records = 0;
    size_t sum_errors = 0;

    jasterix.decodeFile(data_path + filename, "ioss",
        [&](std::unique_ptr<nlohmann::json> json_data, size_t total_num_bytes,
            size_t num_frames, size_t num_records, size_t num_errors)
        {
            callback_called = true;
            sum_records += num_records;
            sum_errors += num_errors;

            loginf << "ioss truncated hexdump test: callback with " << num_frames
                   << " frames, " << num_records << " records, " << num_errors
                   << " errors" << logendl;
        },
        true  // do_flat = true, as used by COMPASS
    );

    // reaching this point at all is the actual test: unclamped, the decode
    // crashes with SIGSEGV inside the error-path hexdump
    REQUIRE(callback_called);
    REQUIRE(sum_records == 1);  // the valid CAT002 record from frame 1
    REQUIRE(sum_errors >= 1);   // truncated frame + failed data block

    loginf << "ioss truncated hexdump test: end" << logendl;
}
