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

// Data block with 3 CAT062 records extracted from a live ARTAS recording, each
// carrying the ARTAS SPF (Track Report Identifiers) with 1, 2 and 3 TRIs:
//   Record 1: track number 6428, SPF REP 1, TRIs [f052a356]
//   Record 2: track number 5188, SPF REP 2, TRIs [76427f0a, 10c4d792]
//   Record 3: track number 7207, SPF REP 3, TRIs [f91c86be, 07e4cab4, 904cba38]

static const std::vector<unsigned int> track_numbers{6428, 5188, 7207};
static const std::vector<std::vector<std::string>> tris{
    {"f052a356"},
    {"76427f0a", "10c4d792"},
    {"f91c86be", "07e4cab4", "904cba38"}};

void test_cat062_121_spf_callback(std::unique_ptr<nlohmann::json> json_data,
                                  size_t total_num_bytes, size_t num_frames,
                                  size_t num_records, size_t num_errors)
{
    loginf << "cat062 1.21 spf test: decoded " << num_frames << " frames, " << num_records
           << " records, " << num_errors << " errors" << logendl;

    REQUIRE(num_frames == 0);
    REQUIRE(num_records == 3);
    REQUIRE(num_errors == 0);

    REQUIRE(json_data->contains("data_blocks"));
    const json& data_block = json_data->at("data_blocks").at(0);

    REQUIRE(data_block.at("category") == 62);

    const json& records = data_block.at("content").at("records");
    REQUIRE(records.size() == 3);

    // structured output: "SPF": {"REP": n, "Target Report Identifiers": [{"TRI": "..."}, ...]}
    for (size_t rec_cnt = 0; rec_cnt < 3; ++rec_cnt)
    {
        const json& record = records.at(rec_cnt);

        REQUIRE(record.at("040").at("Track Number") == track_numbers.at(rec_cnt));

        REQUIRE(record.contains("SPF"));
        const json& spf = record.at("SPF");

        REQUIRE(spf.at("REP") == tris.at(rec_cnt).size());

        const json& identifiers = spf.at("Target Report Identifiers");
        REQUIRE(identifiers.is_array());
        REQUIRE(identifiers.size() == tris.at(rec_cnt).size());

        for (size_t tri_cnt = 0; tri_cnt < tris.at(rec_cnt).size(); ++tri_cnt)
            REQUIRE(identifiers.at(tri_cnt).at("TRI") == tris.at(rec_cnt).at(tri_cnt));
    }

    loginf << "cat062 1.21 spf test: structured checks passed" << logendl;
}

void test_cat062_121_spf_flat_callback(std::unique_ptr<nlohmann::json> json_data,
                                       size_t total_num_bytes, size_t num_frames,
                                       size_t num_records, size_t num_errors)
{
    loginf << "cat062 1.21 spf flat test: decoded " << num_frames << " frames, " << num_records
           << " records, " << num_errors << " errors" << logendl;

    REQUIRE(num_frames == 0);
    REQUIRE(num_records == 3);
    REQUIRE(num_errors == 0);

    // flat output: {"62": {"SPF.REP": [...], "SPF.Target Report Identifiers.TRI": [[...], ...]}}
    REQUIRE(json_data->contains("62"));
    const json& cat62 = json_data->at("62");

    REQUIRE(cat62.contains("040.Track Number"));
    const json& track_col = cat62.at("040.Track Number");
    REQUIRE(track_col.size() == 3);

    REQUIRE(cat62.contains("SPF.REP"));
    const json& rep_col = cat62.at("SPF.REP");
    REQUIRE(rep_col.size() == 3);

    // repetitive item leaves are struct-of-arrays: one column per leaf path, each
    // per-record cell an array of scalars aligned by repetition index
    REQUIRE(cat62.contains("SPF.Target Report Identifiers.TRI"));
    const json& tri_col = cat62.at("SPF.Target Report Identifiers.TRI");
    REQUIRE(tri_col.is_array());
    REQUIRE(tri_col.size() == 3);

    for (size_t rec_cnt = 0; rec_cnt < 3; ++rec_cnt)
    {
        REQUIRE(track_col.at(rec_cnt) == track_numbers.at(rec_cnt));
        REQUIRE(rep_col.at(rec_cnt) == tris.at(rec_cnt).size());

        const json& cell = tri_col.at(rec_cnt);
        REQUIRE(cell.is_array());
        REQUIRE(cell.size() == tris.at(rec_cnt).size());

        for (size_t tri_cnt = 0; tri_cnt < tris.at(rec_cnt).size(); ++tri_cnt)
        {
            REQUIRE(cell.at(tri_cnt).is_string());
            REQUIRE(cell.at(tri_cnt) == tris.at(rec_cnt).at(tri_cnt));
        }
    }

    loginf << "cat062 1.21 spf flat test: flat checks passed" << logendl;
}

TEST_CASE("jASTERIX CAT062 1.21 SPF", "[jASTERIX CAT062]")
{
    loginf << "cat062 1.21 spf test: start" << logendl;

    const std::string filename = "cat062ed1.21_spf.bin";

    SECTION("structured")
    {
        jASTERIX::jASTERIX jasterix(definition_path, false, false, false);

        REQUIRE(jasterix.hasCategory(62));
        std::shared_ptr<jASTERIX::Category> cat062 = jasterix.category(62);
        REQUIRE(cat062->hasEdition("1.21"));
        cat062->setCurrentEdition("1.21");
        REQUIRE(cat062->hasSPFEdition("ARTAS"));
        cat062->setCurrentSPFEdition("ARTAS");

        REQUIRE(jASTERIX::Files::fileExists(data_path + filename));
        REQUIRE(jASTERIX::Files::fileSize(data_path + filename) == 284);

        jasterix.decodeFile(data_path + filename, test_cat062_121_spf_callback);
    }

    SECTION("flat")
    {
        jASTERIX::jASTERIX jasterix(definition_path, false, false, false);

        REQUIRE(jasterix.hasCategory(62));
        std::shared_ptr<jASTERIX::Category> cat062 = jasterix.category(62);
        REQUIRE(cat062->hasEdition("1.21"));
        cat062->setCurrentEdition("1.21");
        REQUIRE(cat062->hasSPFEdition("ARTAS"));
        cat062->setCurrentSPFEdition("ARTAS");

        REQUIRE(jASTERIX::Files::fileExists(data_path + filename));

        jasterix.decodeFile(data_path + filename, test_cat062_121_spf_flat_callback,
                            true);  // do_flat = true
    }

    loginf << "cat062 1.21 spf test: end" << logendl;
}
