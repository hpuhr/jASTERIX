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

void test_cat001_callback (nlohmann::json& json_data, size_t num_frames, size_t num_records)
{
    loginf << "cat001 test: decoded " << num_frames << " frames, " << num_records << " records: " << json_data.dump(4)
               << logendl;

    //    {
    //        "data_blocks": [
    //            {
    //                "category": 1,
    //                "content": {
    //                    "index": 3,
    //                    "length": 17,
    //                    "records": [
    //                        {
    //                            "010": {
    //                                "SAC": 0,
    //                                "SIC": 1
    //                            },
    //                            "020": {
    //                                "ANT": 0,
    //                                "FX": 0,
    //                                "RAB": 0,
    //                                "SIM": 0,
    //                                "SPI": 0,
    //                                "SSR/PSR": 2,
    //                                "TYP": 0
    //                            },
    //                            "040": {
    //                                "RHO": 127.4375,
    //                                "THETA": 256.61865222695997
    //                            },
    //                            "070": {
    //                                "G": 0,
    //                                "L": 0,
    //                                "Mode-3/A reply": 5543,
    //                                "V": 0
    //                            },
    //                            "090": {
    //                                "G": 0,
    //                                "Mode-C HEIGHT": 38000.0,
    //                                "V": 0
    //                            },
    //                            "130": {
    //                                "Radar Plot Characteristics": [
    //                                    {
    //                                        "Value": 96,
    //                                        "extend": 1
    //                                    },
    //                                    {
    //                                        "Value": 60,
    //                                        "extend": 1
    //                                    },
    //                                    {
    //                                        "Value": 96,
    //                                        "extend": 0
    //                                    }
    //                                ]
    //                            },
    //                            "141": {
    //                                "Truncated Time of Day": 221.4296875
    //                            },
    //                            "FSPEC": [
    //                                true,
    //                                true,
    //                                true,
    //                                true,
    //                                true,
    //                                true,
    //                                true,
    //                                false
    //                            ]
    //                        }
    //                    ]
    //                },
    //                "length": 20
    //            }
    //        ]
    //    }

    loginf << "cat001 test: data block" << logendl;

    assert (json_data.find ("data_blocks") != json_data.end());
    assert (json_data.at("data_blocks").is_array());
    assert (json_data.at("data_blocks").size() == 1);
    assert (json_data.at("data_blocks")[0]["category"] == 1);
    assert (json_data.at("data_blocks")[0]["length"] == 20);


    loginf << "cat001 test: num records" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records").size() == 1);

    //    ; FSPEC: 0x fe
    // 11111110

    loginf << "cat001 test: fspec" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("FSPEC").size() == 8);

    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("FSPEC")
            == std::vector<bool>({1,1,1,1,1,1,1,0}));

    //    ; Data Record:
    //    ;  I001/010: =0x 00 01
    //    ;  Data Source Identifier: 0x0001 (SAC=0; SIC=1)

    loginf << "cat001 test: 010" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("010").at("SAC") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("010").at("SIC") == 1);

    //    ;  I001/020: =0x 20
    //    ;  Target Report Descriptor: PLT ACT SEC A1

    loginf << "cat001 test: 020" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("020").at("TYP") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("020").at("SIM") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("020").at("SSR/PSR") == 2);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("020").at("ANT") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("020").at("SPI") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("020").at("RAB") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("020").at("FX") == 0);

    //    ;  I001/040: =0x 3f b8 b6 7c
    //    ;  Measured Position: rng=16312 (127.438 nmi); azm=46716 (256.619 deg)
    loginf << "cat001 test: 040" << logendl;
    double tmp_d = json_data.at("data_blocks")[0].at("content").at("records")[0].at("040").at("RHO");
    assert (fabs(tmp_d - 127.4375) < 10e-3);
    tmp_d = json_data.at("data_blocks")[0].at("content").at("records")[0].at("040").at("THETA");
    assert (fabs(tmp_d - 256.61865222695997) < 10e-3);

    //    ;  I001/070: =0x 0b 63
    //    ;  Mode 3/A Code: v=0; g=0; l=0; code=05543
    loginf << "cat001 test: 070" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("070").at("V") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("070").at("G") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("070").at("L") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("070").at("Mode-3/A reply") == 5543);

    //    ;  I001/090: =0x 05 f0
    //    ;  Mode C Code: v=0; g=0; code=1520 (380.00 FL)
    loginf << "cat001 test: 090" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("090").at("V") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("090").at("G") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("090").at("Mode-C HEIGHT") == 38000.0);

    //    ;  I001/130: =0x c1 79 c0
    //    ;  Radar Plot Characteristics: 0x c1 79 c0
    // 193 -> 96, 121 -> 60, 192 -> 96
    loginf << "cat001 test: 130" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("130").at("Radar Plot Characteristics").size() == 3);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("130").at("Radar Plot Characteristics")[0].at("Value") == 96);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("130").at("Radar Plot Characteristics")[1].at("Value") == 60);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("130").at("Radar Plot Characteristics")[2].at("Value") == 96);


    //    ;  I001/141: =0x 6e b7
    //    ;  Truncated Time of Day: 0x6eb7
    loginf << "cat001 test: 141" << logendl;
    tmp_d = json_data.at("data_blocks")[0].at("content").at("records")[0].at("141").at("Truncated Time of Day");
    assert (fabs(tmp_d-221.4296875) < 10e-6);

    //     [--:--:--.---] - --:--:--.--- -- 0x0001 A1 P:SSR ----- AR  256.619  127.438                       A:05543 --- C: 380    -- ;

}

void test_cat001 (jASTERIX::jASTERIX& jasterix)
{
    loginf << "cat001 test: start" << logendl;

    //      010014fe0001203fb8b67c0b6305f0c179c06eb7
    // echo -n 010014fe0001203fb8b67c0b6305f0c179c06eb7 | xxd -r -p > cat001ed1.1.bin

    const char *cat001_ed11 = "010014fe0001203fb8b67c0b6305f0c179c06eb7";
    char* target = new char[strlen(cat001_ed11)/2];

    size_t size = hex2bin (cat001_ed11, target);

    loginf << "cat001 test: src len " << strlen(cat001_ed11) << " bin len " << size << logendl;

    assert (size == 20);

    assert (jasterix.hasCategory("001"));
    assert (jasterix.hasEdition("001", "1.1"));
    jasterix.setEdition("001", "1.1");
    jasterix.setMapping("001", "");

    jasterix.decodeASTERIX(target, size, test_cat001_callback);

    loginf << "cat001 test: end" << logendl;
}


