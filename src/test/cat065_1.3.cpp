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

void test_cat065_callback (nlohmann::json& json_data, size_t num_frames, size_t num_records, size_t num_errors)
{
    loginf << "cat065 test: decoded " << num_frames << " frames, " << num_records << " records, " << num_errors
           << " errors: " << json_data.dump(4) << logendl;
    assert (num_errors == 0);

//    {
//        "data_blocks": [
//            {
//                "category": 65,
//                "content": {
//                    "index": 3,
//                    "length": 9,
//                    "records": [
//                        {
//                            "000": {
//                                "Message Type": 2
//                            },
//                            "010": {
//                                "SAC": 0,
//                                "SIC": 5
//                            },
//                            "015": {
//                                "Service Identification": 193
//                            },
//                            "020": {
//                                "Batch Number": 0
//                            },
//                            "030": {
//                                "Time of Message": 33502.296875
//                            },
//                            "FSPEC": [
//                                true,
//                                true,
//                                true,
//                                true,
//                                true,
//                                false,
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

    loginf << "cat065 test: data block" << logendl;

    assert (json_data.find ("data_blocks") != json_data.end());
    assert (json_data.at("data_blocks").is_array());
    assert (json_data.at("data_blocks").size() == 1);
    assert (json_data.at("data_blocks")[0]["category"] == 65);
    assert (json_data.at("data_blocks")[0]["length"] == 12);



    loginf << "cat065 test: num records" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records").size() == 1);

    //    ; FSPEC: 0x f8

    loginf << "cat065 test: fspec" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("FSPEC").size() == 8);

    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("FSPEC")
            == std::vector<bool>({1,1,1,1,1,0,0,0}));

    //    ; Data Record:
    //    ;  I065/010: =0x 00 05
    //    ;  Data Source Identifier: 0x0005 (SAC=0; SIC=5)

    loginf << "cat065 test: 010" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("010").at("SAC") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("010").at("SIC") == 5);

    //    ;  I065/000: =0x 02
    //    ;  Message Type: 2 (end of batch)

    loginf << "cat065 test: 000" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("000").at("Message Type") == 02);

    //    ;  I065/015: =0x c1
    //    ;  Service Identification: 193

    loginf << "cat065 test: 015" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("015").at("Service Identification") == 193);

    //    ;  I065/030: =0x 41 6f 26
    //    ;  Time of Message: 0x416f26 (4288294; 09:18:22.297 UTC)


    loginf << "cat065 test: 030" << logendl;
    double tmp_d = json_data.at("data_blocks")[0].at("content").at("records")[0].at("030").at("Time of Message");
    assert (fabs(tmp_d-33502.296875) < 10e-6);

    //    ;  I065/020: =0x 00
    //    ;  Batch Number: 0

    loginf << "cat065 test: 020" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("020").at("Batch Number") == 0);
}

void test_cat065 (jASTERIX::jASTERIX& jasterix)
{
    loginf << "cat065 test: start" << logendl;

    //    ASTERIX data block at pos 1133: cat=65; len=12
    //      41000cf8000502c1416f2600

    // echo -n 41000cf8000502c1416f2600 | xxd -r -p > cat065ed1.3.bin

    const char *cat065_ed10 = "41000cf8000502c1416f2600";
    char* target = new char[strlen(cat065_ed10)/2];

    size_t size = hex2bin (cat065_ed10, target);

    loginf << "cat065 test: src len " << strlen(cat065_ed10) << " bin len " << size << logendl;

    assert (size == 12);

    assert (jasterix.hasCategory(65));
    std::shared_ptr<jASTERIX::Category> cat065 = jasterix.category(65);
    assert (cat065->hasEdition("1.3"));
    cat065->setCurrentEdition("1.3");
    cat065->setCurrentMapping("");

    jasterix.decodeASTERIX(target, size, test_cat065_callback);

    loginf << "cat065 test: end" << logendl;
}


