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


#include "jasterix.h"
#include "logger.h"
#include "string_conv.h"

#include <cmath>

void test_cat002_callback (nlohmann::json& json_data, size_t num_frames, size_t num_records, size_t num_errors)
{
    loginf << "cat002 test: decoded " << num_frames << " frames, " << num_records << " records, " << num_errors
           << " errors: " << json_data.dump(4) << logendl;
    assert (num_errors == 0);

//    {
//        "data_blocks": [
//            {
//                "category": 2,
//                "content": {
//                    "index": 3,
//                    "length": 9,
//                    "records": [
//                        {
//                            "000": {
//                                "Message Type": 1
//                            },
//                            "010": {
//                                "SAC": 0,
//                                "SIC": 1
//                            },
//                            "030": {
//                                "Time of Day": 33501.4140625
//                            },
//                            "050": {
//                                "Station Configuration Status": [
//                                    {
//                                        "Value": 73,
//                                        "extend": 1
//                                    },
//                                    {
//                                        "Value": 1,
//                                        "extend": 0
//                                    }
//                                ]
//                            },
//                            "FSPEC": [
//                                true,
//                                true,
//                                false,
//                                true,
//                                false,
//                                true,
//                                false,
//                                false
//                            ]
//                        }
//                    ]
//                },
//                "length": 12
//            }
//        ]
//    }

    loginf << "cat002 test: data block" << logendl;

    assert (json_data.find ("data_blocks") != json_data.end());
    assert (json_data.at("data_blocks").is_array());
    assert (json_data.at("data_blocks").size() == 1);
    assert (json_data.at("data_blocks")[0]["category"] == 2);
    assert (json_data.at("data_blocks")[0]["length"] == 12);

//    ; Netto frame 1 (length=12) at offset 0x00000000 (0):
//      0x 02 00 0c d4  00 01 01 41  6e b5 93 02
//    ; Netto frame 1:
//    ; ASTERIX data block at pos 0: cat=2; len=12
//      0x 02 00 0c d4  00 01 01 41  6e b5 93 02


    loginf << "cat002 test: num records" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records").size() == 1);

    //    ; FSPEC: 0x d4

    loginf << "cat002 test: fspec" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("FSPEC").size() == 8);

    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("FSPEC")
            == std::vector<bool>({1,1,0,1,0,1,0,0}));

    //    ; Data Record:
    //    ;  I002/010: =0x 00 01
    //    ;  Data Source Identifier: 0x0001 (SAC=0; SIC=1)

    loginf << "cat002 test: 010" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("010").at("SAC") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("010").at("SIC") == 1);

    //    ;  I002/000: =0x 01
    //    ;  Message Type: mtp=1 (North marker message)
    loginf << "cat002 test: 000" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("000").at("Message Type") == 1);


    //    ;  I002/030: =0x 41 6e b5
    //    ;  Time of Day: 0x416eb5 (4288181; 33501.414062 secs; 09:18:21.414 UTC)
    loginf << "cat002 test: 030" << logendl;
    double tmp_d = json_data.at("data_blocks")[0].at("content").at("records")[0].at("030").at("Time of Day");
    assert (fabs(tmp_d-33501.4140625) < 10e-6);

    //    ;  I002/050: =0x 93 02
    //    ;  Station Configuration Status: 0x 92 02
    loginf << "cat002 test: 050" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("050").at("Station Configuration Status").size() == 2);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("050").at("Station Configuration Status")[0].at("Value") == 73);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("050").at("Station Configuration Status")[1].at("Value") == 1);

    //     [--:--:--.---] M 09:18:21.414 -- 0x0001 A1 S:NMK ----- A-    0.000
}

void test_cat002 (jASTERIX::jASTERIX& jasterix)
{
    loginf << "cat002 test: start" << logendl;

    //      02000cd4000101416eb59302
    // echo -n 02000cd4000101416eb59302 | xxd -r -p > cat002ed1.0.bin

    const char *cat002_ed10 = "02000cd4000101416eb59302";
    char* target = new char[strlen(cat002_ed10)/2];

    size_t size = hex2bin (cat002_ed10, target);

    loginf << "cat002 test: src len " << strlen(cat002_ed10) << " bin len " << size << logendl;

    assert (size == 12);

    assert (jasterix.hasCategory(2));
    std::shared_ptr<jASTERIX::Category> cat002 = jasterix.category(2);
    assert (cat002->hasEdition("1.0"));
    cat002->setCurrentEdition("1.0");
    cat002->setCurrentMapping("");

    jasterix.decodeASTERIX(target, size, test_cat002_callback);

    loginf << "cat002 test: end" << logendl;
}


