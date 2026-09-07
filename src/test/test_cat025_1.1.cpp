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

void test_cat025_callback(std::unique_ptr<nlohmann::json> json_data, size_t total_num_bytes, size_t num_frames,
                          size_t num_records, size_t num_errors)
{
    loginf << "cat025 test: decoded " << num_frames << " frames, " << num_records << " records, "
           << num_errors << " errors" << logendl;

    REQUIRE(num_frames == 0);
    REQUIRE(num_records == 2);
    REQUIRE(num_errors == 0);

    loginf << "cat025 test: data blocks" << logendl;

    REQUIRE(json_data->contains("data_blocks"));
    REQUIRE(json_data->at("data_blocks").is_array());
    REQUIRE(json_data->at("data_blocks").size() == 2);

    // first data block: Service Statistics report (Report Type 3)

    //    ; ASTERIX data block at pos 0: cat=25; len=64
    //    190040fd20681d06066c5801c70e700444c20000f107168000000115178000000000198000
    //    0000721880000000001a80000000001b80000000001c8000000000

    const json& first_data_block = json_data->at("data_blocks").at(0);

    REQUIRE(first_data_block.contains("category"));
    REQUIRE(first_data_block.at("category") == 25);
    REQUIRE(first_data_block.contains("length"));
    REQUIRE(first_data_block.at("length") == 64);

    REQUIRE(first_data_block.contains("content"));
    REQUIRE(first_data_block.at("content").contains("records"));
    REQUIRE(first_data_block.at("content").at("records").is_array());
    REQUIRE(first_data_block.at("content").at("records").size() == 1);

    const json& record = first_data_block.at("content").at("records").at(0);

    //    ; FSPEC: 0x fd 20

    //    ;  I025/010: =0x 68 1d
    //    ;  Data Source Identifier: 0x681d (SAC=104; SIC=29)

    loginf << "cat025 test: 010" << logendl;
    REQUIRE(record.at("010").at("SAC") == 104);
    REQUIRE(record.at("010").at("SIC") == 29);

    //    ;  I025/000: =0x 06
    //    ;  Report Type: 3 (Service Statistics report); RG=0 (Periodic Report)

    loginf << "cat025 test: 000" << logendl;
    REQUIRE(record.at("000").at("Report Type") == 3);
    REQUIRE(record.at("000").at("RG") == 0);

    //    ;  I025/200: =0x 06 6c 58
    //    ;  Message Identification Number: 420952

    loginf << "cat025 test: 200" << logendl;
    REQUIRE(record.at("200").at("Message Identification Number") == 420952);

    //    ;  I025/015: =0x 01
    //    ;  Service Identification: 1

    loginf << "cat025 test: 015" << logendl;
    REQUIRE(record.at("015").at("SID") == 1);

    //    ;  I025/020: =0x c7 0e 70 04 44 c2
    //    ;  Service Designator: "1090ADSB" (8 characters, 6 bits each)

    loginf << "cat025 test: 020" << logendl;
    REQUIRE(record.at("020").at("Service Designator") == "1090ADSB");

    //    ;  I025/070: =0x 00 00 f1
    //    ;  Time of Day: 241 * 1/128 s = 1.8828125 s

    loginf << "cat025 test: 070" << logendl;
    REQUIRE(approximatelyEqual(record.at("070").at("Time of Day"), 1.8828125, 10e-6));

    //    ;  I025/140: =0x 07 16 80 00 00 01 15 17 80 00 00 00 00 19 80 00 00 00 72
    //    ;            18 80 00 00 00 00 1a 80 00 00 00 00 1b 80 00 00 00 00 1c 80 00 00 00 00
    //    ;  Service Statistics: REP=7 counters, TYPE 22-28, REF=1 (from previous report)

    loginf << "cat025 test: 140" << logendl;
    REQUIRE(record.at("140").at("REP") == 7);

    const json& statistics = record.at("140").at("Service Statistics");
    REQUIRE(statistics.is_array());
    REQUIRE(statistics.size() == 7);

    REQUIRE(statistics.at(0).at("TYPE") == 22);
    REQUIRE(statistics.at(0).at("REF") == 1);
    REQUIRE(statistics.at(0).at("SPARE") == 0);
    REQUIRE(statistics.at(0).at("COUNTER") == 277);

    REQUIRE(statistics.at(2).at("TYPE") == 25);
    REQUIRE(statistics.at(2).at("REF") == 1);
    REQUIRE(statistics.at(2).at("COUNTER") == 114);

    REQUIRE(statistics.at(6).at("TYPE") == 28);
    REQUIRE(statistics.at(6).at("COUNTER") == 0);

    // second data block: Service and System Status report (Report Type 1)

    //    ; ASTERIX data block at pos 64: cat=25; len=26
    //    19001aff40681d02066c5801c70e700444c20001000001ffff00

    const json& second_data_block = json_data->at("data_blocks").at(1);

    REQUIRE(second_data_block.at("category") == 25);
    REQUIRE(second_data_block.at("length") == 26);

    REQUIRE(second_data_block.at("content").at("records").is_array());
    REQUIRE(second_data_block.at("content").at("records").size() == 1);

    const json& record2 = second_data_block.at("content").at("records").at(0);

    //    ; FSPEC: 0x ff 40

    //    ;  I025/000: =0x 02
    //    ;  Report Type: 1 (Service and System Status report); RG=0 (Periodic Report)

    loginf << "cat025 test: record 2 000" << logendl;
    REQUIRE(record2.at("000").at("Report Type") == 1);
    REQUIRE(record2.at("000").at("RG") == 0);

    loginf << "cat025 test: record 2 010" << logendl;
    REQUIRE(record2.at("010").at("SAC") == 104);
    REQUIRE(record2.at("010").at("SIC") == 29);

    loginf << "cat025 test: record 2 015" << logendl;
    REQUIRE(record2.at("015").at("SID") == 1);

    loginf << "cat025 test: record 2 020" << logendl;
    REQUIRE(record2.at("020").at("Service Designator") == "1090ADSB");

    //    ;  I025/070: =0x 00 01 00
    //    ;  Time of Day: 256 * 1/128 s = 2.0 s

    loginf << "cat025 test: record 2 070" << logendl;
    REQUIRE(approximatelyEqual(record2.at("070").at("Time of Day"), 2.0, 10e-6));

    //    ;  I025/100: =0x 00
    //    ;  System and Service Status: NOGO=0 (released); OPS=0 (operational);
    //    ;  SSTAT=0 (running); FX=0

    loginf << "cat025 test: record 2 100" << logendl;
    REQUIRE(record2.at("100").at("NOGO") == 0);
    REQUIRE(record2.at("100").at("OPS") == 0);
    REQUIRE(record2.at("100").at("SSTAT") == 0);
    REQUIRE(record2.at("100").at("FX") == 0);

    //    ;  I025/120: =0x 01 ff ff 00
    //    ;  Component Status: REP=1; CID=65535 (all components); Error Code=0; CS=0 (running)

    loginf << "cat025 test: record 2 120" << logendl;
    REQUIRE(record2.at("120").at("REP") == 1);

    const json& components = record2.at("120").at("Component Status");
    REQUIRE(components.is_array());
    REQUIRE(components.size() == 1);
    REQUIRE(components.at(0).at("CID") == 65535);
    REQUIRE(components.at(0).at("Error Code") == 0);
    REQUIRE(components.at(0).at("CS") == 0);

    loginf << "cat025 test: record 2 200" << logendl;
    REQUIRE(record2.at("200").at("Message Identification Number") == 420952);
}

TEST_CASE("jASTERIX CAT025 1.1", "[jASTERIX CAT025]")
{
    loginf << "cat025 test: start" << logendl;

    jASTERIX::jASTERIX jasterix(definition_path, true, true, false);

    // two raw data blocks extracted from an IOSS-framed 1090 ADS-B ground system recording:
    // a Service Statistics report (type 3) and a Service and System Status report (type 1)

    // echo -n 190040fd20681d06066c5801c70e700444c20000f1071680000001151780000000001980000000721880000000001a80000000001b80000000001c800000000019001aff40681d02066c5801c70e700444c20001000001ffff00 | xxd -r -p > cat025ed1.1.bin

    REQUIRE(jasterix.hasCategory(25));
    std::shared_ptr<jASTERIX::Category> cat025 = jasterix.category(25);
    REQUIRE(cat025->hasEdition("1.1"));
    cat025->setCurrentEdition("1.1");

    const std::string filename = "cat025ed1.1.bin";

    REQUIRE(jASTERIX::Files::fileExists(data_path + filename));
    REQUIRE(jASTERIX::Files::fileSize(data_path + filename) == 90);

    jasterix.decodeFile(data_path + filename, test_cat025_callback);

    loginf << "cat025 test: end" << logendl;
}
