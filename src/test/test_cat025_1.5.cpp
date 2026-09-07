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

// SYNTHETIC test sample, not a real recording. One hand-crafted CAT025 edition 1.5
// record exercising the items added after edition 1.1 (I025/100 first extension,
// I025/600, I025/610) with obviously artificial values:
//   SAC/SIC 255/255, SID 42, Service Designator "TESTDATA", Message ID 999999,
//   Time of Day 12345 s, NOGO 1, OPS 2, SSTAT 2, SySTAT 3, SeSTAT 1,
//   error codes [2, 5], reference point 11.111111 deg / -22.222222 deg, height -123.75 m
//
// echo -n 190024ff8cffff030f423f2a5054d4101501181c80c5320202050fcd6e9bf0329165fe11 | xxd -r -p > cat025ed1.5.bin

void test_cat025_15_callback(std::unique_ptr<nlohmann::json> json_data, size_t total_num_bytes,
                             size_t num_frames, size_t num_records, size_t num_errors)
{
    loginf << "cat025 1.5 test: decoded " << num_frames << " frames, " << num_records
           << " records, " << num_errors << " errors" << logendl;

    REQUIRE(num_frames == 0);
    REQUIRE(num_records == 1);
    REQUIRE(num_errors == 0);

    REQUIRE(json_data->contains("data_blocks"));
    const json& data_block = json_data->at("data_blocks").at(0);

    REQUIRE(data_block.at("category") == 25);
    REQUIRE(data_block.at("length") == 36);

    const json& records = data_block.at("content").at("records");
    REQUIRE(records.size() == 1);

    const json& record = records.at(0);

    loginf << "cat025 1.5 test: 010" << logendl;
    REQUIRE(record.at("010").at("SAC") == 255);
    REQUIRE(record.at("010").at("SIC") == 255);

    loginf << "cat025 1.5 test: 000" << logendl;
    REQUIRE(record.at("000").at("Report Type") == 1);
    REQUIRE(record.at("000").at("RG") == 1);

    loginf << "cat025 1.5 test: 200" << logendl;
    REQUIRE(record.at("200").at("Message Identification Number") == 999999);

    loginf << "cat025 1.5 test: 015" << logendl;
    REQUIRE(record.at("015").at("SID") == 42);

    loginf << "cat025 1.5 test: 020" << logendl;
    REQUIRE(record.at("020").at("Service Designator") == "TESTDATA");

    loginf << "cat025 1.5 test: 070" << logendl;
    REQUIRE(approximatelyEqual(record.at("070").at("Time of Day"), 12345.0, 10e-6));

    // I025/100 with the first extension added in edition 1.3

    loginf << "cat025 1.5 test: 100" << logendl;
    REQUIRE(record.at("100").at("NOGO") == 1);
    REQUIRE(record.at("100").at("OPS") == 2);
    REQUIRE(record.at("100").at("SSTAT") == 2);
    REQUIRE(record.at("100").at("FX") == 1);
    REQUIRE(record.at("100").at("SPARE") == 0);
    REQUIRE(record.at("100").at("SySTAT") == 3);
    REQUIRE(record.at("100").at("SeSTAT") == 1);
    REQUIRE(record.at("100").at("FX2") == 0);

    loginf << "cat025 1.5 test: 105" << logendl;
    REQUIRE(record.at("105").at("REP") == 2);

    const json& errors = record.at("105").at("System and Service Error Codes");
    REQUIRE(errors.is_array());
    REQUIRE(errors.size() == 2);
    REQUIRE(errors.at(0).at("ERR") == 2);
    REQUIRE(errors.at(1).at("ERR") == 5);

    // I025/600 and I025/610 added in edition 1.2

    loginf << "cat025 1.5 test: 600" << logendl;
    REQUIRE(approximatelyEqual(record.at("600").at("Latitude"), 11.111111, 10e-6));
    REQUIRE(approximatelyEqual(record.at("600").at("Longitude"), -22.222222, 10e-6));

    loginf << "cat025 1.5 test: 610" << logendl;
    REQUIRE(approximatelyEqual(record.at("610").at("Height"), -123.75, 10e-6));
}

TEST_CASE("jASTERIX CAT025 1.5", "[jASTERIX CAT025]")
{
    loginf << "cat025 1.5 test: start" << logendl;

    jASTERIX::jASTERIX jasterix(definition_path, true, true, false);

    REQUIRE(jasterix.hasCategory(25));
    std::shared_ptr<jASTERIX::Category> cat025 = jasterix.category(25);
    REQUIRE(cat025->hasEdition("1.5"));
    cat025->setCurrentEdition("1.5");

    const std::string filename = "cat025ed1.5.bin";

    REQUIRE(jASTERIX::Files::fileExists(data_path + filename));
    REQUIRE(jASTERIX::Files::fileSize(data_path + filename) == 36);

    jasterix.decodeFile(data_path + filename, test_cat025_15_callback);

    loginf << "cat025 1.5 test: end" << logendl;
}
