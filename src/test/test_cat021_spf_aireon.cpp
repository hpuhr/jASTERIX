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
#include "string_conv.h"
#include "test_jasterix.h"

using namespace std;
using namespace nlohmann;

// SYNTHETIC test sample, not a real recording. One hand-crafted CAT021 2.4 record
// carrying only I021/010 (SAC/SIC 255/255), I021/170 (callsign "TEST1234") and the
// Aireon vendor SPF with obviously artificial values:
//   MsgID 123456, SatVID 42, VDist 9.9 NM, ValidState 3 (valid), VRadius 0.7 NM
//
// echo -n 15001981010101810102ffff5054d4c72cf40701e2402ac787 | xxd -r -p > cat021ed2.4_spf_aireon.bin

void test_cat021_spf_aireon_callback(std::unique_ptr<nlohmann::json> json_data,
                                     size_t total_num_bytes, size_t num_frames,
                                     size_t num_records, size_t num_errors)
{
    loginf << "cat021 spf aireon test: decoded " << num_frames << " frames, " << num_records
           << " records, " << num_errors << " errors" << logendl;

    REQUIRE(num_frames == 0);
    REQUIRE(num_records == 1);
    REQUIRE(num_errors == 0);

    REQUIRE(json_data->contains("data_blocks"));
    const json& data_block = json_data->at("data_blocks").at(0);

    REQUIRE(data_block.at("category") == 21);
    REQUIRE(data_block.at("length") == 25);

    const json& records = data_block.at("content").at("records");
    REQUIRE(records.size() == 1);

    const json& record = records.at(0);

    loginf << "cat021 spf aireon test: 010" << logendl;
    REQUIRE(record.at("010").at("SAC") == 255);
    REQUIRE(record.at("010").at("SIC") == 255);

    loginf << "cat021 spf aireon test: 170" << logendl;
    REQUIRE(record.at("170").at("Target Identification") == "TEST1234");

    loginf << "cat021 spf aireon test: SPF" << logendl;
    REQUIRE(record.contains("SPF"));
    const json& spf = record.at("SPF");
    REQUIRE(spf.is_object());

    REQUIRE(spf.at("MsgID") == 123456);
    REQUIRE(spf.at("SatVID") == 42);
    REQUIRE(approximatelyEqual(spf.at("VDist"), 9.9, 10e-6));
    REQUIRE(spf.at("ValidState") == 3);
    REQUIRE(approximatelyEqual(spf.at("VRadius"), 0.7, 10e-6));

    loginf << "cat021 spf aireon test: structured checks passed" << logendl;
}

void test_cat021_spf_aireon_flat_callback(std::unique_ptr<nlohmann::json> json_data,
                                          size_t total_num_bytes, size_t num_frames,
                                          size_t num_records, size_t num_errors)
{
    loginf << "cat021 spf aireon flat test: decoded " << num_frames << " frames, " << num_records
           << " records, " << num_errors << " errors" << logendl;

    REQUIRE(num_frames == 0);
    REQUIRE(num_records == 1);
    REQUIRE(num_errors == 0);

    REQUIRE(json_data->contains("21"));
    const json& cat21 = json_data->at("21");

    REQUIRE(cat21.contains("170.Target Identification"));
    REQUIRE(cat21.at("170.Target Identification").at(0) == "TEST1234");

    REQUIRE(cat21.contains("SPF.MsgID"));
    REQUIRE(cat21.at("SPF.MsgID").at(0) == 123456);

    REQUIRE(cat21.contains("SPF.SatVID"));
    REQUIRE(cat21.at("SPF.SatVID").at(0) == 42);

    REQUIRE(cat21.contains("SPF.VDist"));
    REQUIRE(approximatelyEqual(cat21.at("SPF.VDist").at(0), 9.9, 10e-6));

    REQUIRE(cat21.contains("SPF.ValidState"));
    REQUIRE(cat21.at("SPF.ValidState").at(0) == 3);

    REQUIRE(cat21.contains("SPF.VRadius"));
    REQUIRE(approximatelyEqual(cat21.at("SPF.VRadius").at(0), 0.7, 10e-6));

    loginf << "cat021 spf aireon flat test: flat checks passed" << logendl;
}

void test_cat021_spf_aireon_inactive_callback(std::unique_ptr<nlohmann::json> json_data,
                                              size_t total_num_bytes, size_t num_frames,
                                              size_t num_records, size_t num_errors)
{
    loginf << "cat021 spf aireon inactive test: decoded " << num_frames << " frames, "
           << num_records << " records, " << num_errors << " errors" << logendl;

    REQUIRE(num_frames == 0);
    REQUIRE(num_records == 1);
    REQUIRE(num_errors == 0);

    const json& record =
        json_data->at("data_blocks").at(0).at("content").at("records").at(0);

    // without the SPF edition activated the field is kept as raw hex data
    REQUIRE(record.contains("SPF"));
    REQUIRE(record.at("SPF").is_string());
    REQUIRE(record.at("SPF") == "01e2402ac787");

    loginf << "cat021 spf aireon inactive test: raw passthrough checks passed" << logendl;
}

TEST_CASE("jASTERIX CAT021 SPF Aireon", "[jASTERIX CAT021]")
{
    loginf << "cat021 spf aireon test: start" << logendl;

    const std::string filename = "cat021ed2.4_spf_aireon.bin";

    SECTION("structured")
    {
        jASTERIX::jASTERIX jasterix(definition_path, false, false, false);

        REQUIRE(jasterix.hasCategory(21));
        std::shared_ptr<jASTERIX::Category> cat021 = jasterix.category(21);
        REQUIRE(cat021->hasEdition("2.4"));
        cat021->setCurrentEdition("2.4");
        REQUIRE(cat021->hasSPFEdition("Aireon"));
        cat021->setCurrentSPFEdition("Aireon");

        REQUIRE(jASTERIX::Files::fileExists(data_path + filename));
        REQUIRE(jASTERIX::Files::fileSize(data_path + filename) == 25);

        jasterix.decodeFile(data_path + filename, test_cat021_spf_aireon_callback);
    }

    SECTION("flat")
    {
        jASTERIX::jASTERIX jasterix(definition_path, false, false, false);

        std::shared_ptr<jASTERIX::Category> cat021 = jasterix.category(21);
        cat021->setCurrentEdition("2.4");
        REQUIRE(cat021->hasSPFEdition("Aireon"));
        cat021->setCurrentSPFEdition("Aireon");

        REQUIRE(jASTERIX::Files::fileExists(data_path + filename));

        jasterix.decodeFile(data_path + filename, test_cat021_spf_aireon_flat_callback,
                            true);  // do_flat = true
    }

    SECTION("not activated")
    {
        // the Aireon SPF is deliberately not a default: without explicit activation
        // the SP field must stay raw hex
        jASTERIX::jASTERIX jasterix(definition_path, false, false, false);

        std::shared_ptr<jASTERIX::Category> cat021 = jasterix.category(21);
        cat021->setCurrentEdition("2.4");

        REQUIRE(jASTERIX::Files::fileExists(data_path + filename));

        jasterix.decodeFile(data_path + filename, test_cat021_spf_aireon_inactive_callback);
    }

    loginf << "cat021 spf aireon test: end" << logendl;
}
