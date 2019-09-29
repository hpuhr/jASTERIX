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

void test_cat063_callback (nlohmann::json& json_data, size_t num_frames, size_t num_records)
{
    loginf << "cat063 test: decoded " << num_frames << " frames, " << num_records << " records: " << json_data.dump(4)
               << logendl;

//    {
//        "data_blocks": [
//            {
//                "category": 63,
//                "content": {
//                    "index": 3,
//                    "length": 27,
//                    "records": [
//                        {
//                            "010": {
//                                "SAC": 0,
//                                "SIC": 5
//                            },
//                            "015": {
//                                "Service Identification": 193
//                            },
//                            "030": {
//                                "Time of Message": 33509.796875
//                            },
//                            "050": {
//                                "SAC": 0,
//                                "SIC": 1
//                            },
//                            "060": {
//                                "ADS": 0,
//                                "CON": 0,
//                                "FX": 0,
//                                "MDS": 0,
//                                "MLT": 0,
//                                "PSR": 0,
//                                "SSR": 0
//                            },
//                            "070": {
//                                "TSB": -53.0
//                            },
//                            "080": {
//                                "SRB": 0.0,
//                                "SRG": 0.00011
//                            },
//                            "081": {
//                                "SAB": -0.00549316406
//                            },
//                            "090": {
//                                "PRB": 0.0078125,
//                                "PRG": 1e-05
//                            },
//                            "091": {
//                                "PAB": -0.01098632812
//                            },
//                            "092": {
//                                "PEB": 0.0
//                            },
//                            "FSPEC": [
//                                true,
//                                true,
//                                true,
//                                true,
//                                true,
//                                true,
//                                true,
//                                true,
//                                true,
//                                true,
//                                true,
//                                true,
//                                false,
//                                false,
//                                false,
//                                false
//                            ]
//                        }
//                    ]
//                },
//                "length": 30
//            }
//        ]
//    }

    loginf << "cat063 test: data block" << logendl;

    assert (json_data.find ("data_blocks") != json_data.end());
    assert (json_data.at("data_blocks").is_array());
    assert (json_data.at("data_blocks").size() == 1);
    assert (json_data.at("data_blocks")[0]["category"] == 63);
    assert (json_data.at("data_blocks")[0]["length"] == 30);

    loginf << "cat063 test: num records" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records").size() == 1);

    //    ; FSPEC: 0x ff f0

    loginf << "cat063 test: fspec" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("FSPEC").size() == 2*8);

    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("FSPEC")
            == std::vector<bool>({1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0}));

    //    ; Data Record:
    //    ;  I063/010: =0x 00 05
    //    ;  Data Source Identifier: 0x0005 (SAC=0; SIC=5)

    loginf << "cat063 test: 010" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("010").at("SAC") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("010").at("SIC") == 5);

    //    ;  I063/015: =0x c1
    //    ;  Service Identification: 193

    loginf << "cat063 test: 015" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("015").at("Service Identification") == 193);

    //    ;  I063/030: =0x 41 72 e6
    //    ;  Time of Message: 0x4172e6 (4289254; 09:18:29.797 UTC)

    loginf << "cat063 test: 030" << logendl;
    double tmp_d = json_data.at("data_blocks")[0].at("content").at("records")[0].at("030").at("Time of Message");
    assert (fabs(tmp_d-33509.796875) < 10e-6);

    //    ;  I063/050: =0x 00 01
    //    ;  Sensor Identifier: 0x0001 (SAC=0; SIC=1)

    loginf << "cat063 test: 050" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("050").at("SAC") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("050").at("SIC") == 1);

    //    ;  I063/060: =0x 00
    //    ;  Sensor Configuration and Status: con=0 (operational); PSR GO; SSR GO; ModeS GO; ADS GO; MLT GO

    loginf << "cat063 test: 060" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("060").at("CON") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("060").at("PSR") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("060").at("SSR") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("060").at("MDS") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("060").at("ADS") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("060").at("MLT") == 0);

    //    ;  I063/070: =0x ff cb
    //    ;  Time Stamping Bias: -53 msec

    loginf << "cat063 test: 070" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("070").at("TSB") == -53);

    //    ;  I063/080: =0x 00 0b 00 00
    //    ;  SSR/ModeS Range Gain and Bias: bias=0 (0.000 nmi) gain=11 (0.000110)

    loginf << "cat063 test: 080" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("SRG") == 0.00011);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("SRB") == 0.0);

    //    ;  I063/081: =0x ff ff
    //    ;  SSR/ModeS Azimuth Bias: -1 (-0.005 deg)

    loginf << "cat063 test: 081" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("081").at("SAB") == -0.00549316406);

    //    ;  I063/090: =0x 00 01 00 01
    //    ;  PSR Range Gain and Bias: bias=1 (0.008 nmi) gain=1 (0.000010)

    loginf << "cat063 test: 090" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("090").at("PRG") == 0.00001);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("090").at("PRB") == 0.0078125);

    //    ;  I063/091: =0x ff fe
    //    ;  PSR Azimuth Bias: -2 (-0.011 deg)

    loginf << "cat063 test: 091" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("091").at("PAB") == -0.01098632812);

    //    ;  I063/092: =0x 00 00
    //    ;  PSR Elevation Bias: 0 (0.000 deg)

    loginf << "cat063 test: 092" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("092").at("PEB") == 0.0);
}

void test_cat063 (jASTERIX::jASTERIX& jasterix)
{
    loginf << "cat063 test: start" << logendl;

    //  	  ; ASTERIX data block at pos 0: cat=63; len=30
    //      3f001efff00005c14172e6000100ffcb000b0000ffff00010001fffe0000

    // echo -n 3f001efff00005c14172e6000100ffcb000b0000ffff00010001fffe0000 | xxd -r -p > cat063ed1.0.bin

    const char *cat063_ed10 = "3f001efff00005c14172e6000100ffcb000b0000ffff00010001fffe0000";
    char* target = new char[strlen(cat063_ed10)/2];

    size_t size = hex2bin (cat063_ed10, target);

    loginf << "cat063 test: src len " << strlen(cat063_ed10) << " bin len " << size << logendl;

    assert (size == 30);

    assert (jasterix.hasCategory(63));
    assert (jasterix.hasEdition(63, "1.0"));
    jasterix.setEdition(63, "1.0");
    jasterix.setMapping(63, "");

    jasterix.decodeASTERIX(target, size, test_cat063_callback);

    loginf << "cat063 test: end" << logendl;
}


