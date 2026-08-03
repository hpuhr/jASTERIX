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

// Tests the reconstruction of the full Time of Day from the CAT001 truncated
// Time of Day (I001/141) against the last CAT002 Time of Day (I002/030) of the
// same SAC/SIC in flat decoding mode.
//
// I001/141 is the full time modulo 512 s (CAT001 Part 2a ed. 1.3, section
// 5.2.15). Per section 5.2.15 Note 1 the reconstruction must tolerate clock
// offsets below 512 s between record and reference, including the 512 s wrap;
// per Note 2 the time resets to 0 at midnight. The test data interleaves
// CAT002 reference messages and CAT001 records covering:
//   ri 0: record in the same 512 s period as the reference
//   ri 1: record from a SAC/SIC without any CAT002 reference -> null
//   ri 2: record one period after the reference (512 s wrap forward)
//   ri 3: record one period before the reference (512 s wrap backward)
//   ri 4: reference in the truncated last period of the day (base 86016 s),
//         record from the period before - the case that previously failed
//         the 'full_tod < tod_24h' assertion
//   ri 5: same constellation, but record offset >= 256 s from the reference,
//         which is not uniquely resolvable -> null
//   ri 6: reference just before midnight, record just after the reset
//   ri 7: reference just after midnight, record from just before midnight
TEST_CASE("jASTERIX CAT001 1.1 flat ToD reconstruction", "[jASTERIX CAT001]")
{
    loginf << "cat001 tod reconstruction test: start" << logendl;

    jASTERIX::jASTERIX jasterix(definition_path, true, true, false);

    REQUIRE(jasterix.hasCategory(1));
    std::shared_ptr<jASTERIX::Category> cat001 = jasterix.category(1);
    REQUIRE(cat001->hasEdition("1.1"));
    cat001->setCurrentEdition("1.1");

    REQUIRE(jasterix.hasCategory(2));
    std::shared_ptr<jASTERIX::Category> cat002 = jasterix.category(2);
    REQUIRE(cat002->hasEdition("1.0"));
    cat002->setCurrentEdition("1.0");

    const std::string filename = "cat001ed1.1_tod_reconstruction.bin";

    REQUIRE(jASTERIX::Files::fileExists(data_path + filename));
    REQUIRE(jASTERIX::Files::fileSize(data_path + filename) == 142);

    bool callback_called = false;

    jasterix.decodeFile(data_path + filename,
        [&](std::unique_ptr<nlohmann::json> json_data, size_t total_num_bytes,
            size_t num_frames, size_t num_records, size_t num_errors)
        {
            callback_called = true;

            loginf << "cat001 tod reconstruction test: callback with " << num_records
                   << " records, " << num_errors << " errors" << logendl;

            REQUIRE(num_frames == 0);
            REQUIRE(num_records == 15);  // 7 CAT002 + 8 CAT001
            REQUIRE(num_errors == 0);

            REQUIRE(json_data->contains("1"));
            const json& cat1 = json_data->at("1");

            REQUIRE(cat1.contains("141.Truncated Time of Day"));
            REQUIRE(cat1.contains("140.Time-of-Day"));

            const json& tod_col = cat1.at("140.Time-of-Day");
            REQUIRE(tod_col.is_array());
            REQUIRE(tod_col.size() == 8);

            // expected reconstructed full times, NaN marking expected nulls
            const double expected[8] = {1002.0,   // same period as ref 1000
                                        -1.0,     // null: no CAT002 ref for SAC/SIC 0/2
                                        512.109375,  // period after ref 511.99
                                        505.0,    // period before ref 520
                                        85904.0,  // period before ref 86020 (last period of day)
                                        -1.0,     // null: 396 s offset from ref 86300, ambiguous
                                        5.0,      // after midnight reset, ref 86390
                                        86396.0}; // before midnight, ref 5
            const bool expect_null[8] = {false, true, false, false,
                                         false, true, false, false};

            for (size_t i = 0; i < 8; ++i)
            {
                if (expect_null[i])
                    REQUIRE(tod_col[i].is_null());
                else
                {
                    REQUIRE_FALSE(tod_col[i].is_null());
                    REQUIRE(approximatelyEqual(tod_col[i].get<double>(), expected[i], 10e-6));
                }
            }

            loginf << "cat001 tod reconstruction test: all checks passed" << logendl;
        },
        true  // do_flat = true
    );

    REQUIRE(callback_called);

    loginf << "cat001 tod reconstruction test: end" << logendl;
}
