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

void test_cat062_116_callback (std::unique_ptr<nlohmann::json> json_data, size_t num_frames, size_t num_records, size_t num_errors)
{
    loginf << "cat062 1.16 test: decoded " << num_frames << " frames, " << num_records << " records, " << num_errors
           << " errors: " << json_data->dump(4) << logendl;
    assert (num_errors == 0);

    //    {
    //        "data_blocks": [
    //            {
    //                "category": 62,
    //                "content": {
    //                    "index": 3,
    //                    "length": 148,
    //                    "records": [
    //                        {
    //                            "010": {
    //                                "SAC": 0,
    //                                "SIC": 4
    //                            },
    //                            "015": {
    //                                "Service Identification": 225
    //                            },
    //                            "040": {
    //                                "Track Number": 5086
    //                            },
    //                            "060": {
    //                                "CH": 0,
    //                                "Mode-3/A reply": 7621
    //                            },
    //                            "070": {
    //                                "Time Of Track Information": 33502.5
    //                            },
    //                            "080": {
    //                                "AAC": 0,
    //                                "ADS": 1,
    //                                "AFF": 0,
    //                                "AMA": 0,
    //                                "CNF": 0,
    //                                "CST": 0,
    //                                "FPC": 1,
    //                                "FX": 1,
    //                                "FX2": 1,
    //                                "FX3": 1,
    //                                "KOS": 1,
    //                                "MD4": 0,
    //                                "MD5": 0,
    //                                "MDS": 0,
    //                                "ME": 0,
    //                                "MI": 0,
    //                                "MON": 0,
    //                                "MRH": 0,
    //                                "PSR": 1,
    //                                "SIM": 0,
    //                                "SPI": 0,
    //                                "SRC": 3,
    //                                "SSR": 0,
    //                                "STP": 0,
    //                                "SUC": 0,
    //                                "TSB": 0,
    //                                "TSE": 0
    //                            },
    //                            "100": {
    //                                "X": 260661.0,
    //                                "Y": -220711.5
    //                            },
    //                            "105": {
    //                                "Latitude": 45.465155734119996,
    //                                "Longitude": 17.33247308292
    //                            },
    //                            "135": {
    //                                "Calculated Track Barometric Altitude": 34975.0,
    //                                "QNH": 0
    //                            },
    //                            "136": {
    //                                "Measured Flight Level": 349.75
    //                            },
    //                            "185": {
    //                                "Vx": 207.5,
    //                                "Vy": -120.5
    //                            },
    //                            "200": {
    //                                "ADF": 0,
    //                                "LONG": 0,
    //                                "TRANS": 0,
    //                                "VERT": 0
    //                            },
    //                            "210": {
    //                                "Ax": 0.0,
    //                                "Ay": 0.0
    //                            },
    //                            "220": {
    //                                "Rate of Climb/Descent": 0.0
    //                            },
    //                            "290": {
    //                                "ES": {
    //                                    "Age": 63.75
    //                                },
    //                                "MDS": {
    //                                    "Age": 2.0
    //                                },
    //                                "MLT": {
    //                                    "Age": 63.75
    //                                },
    //                                "PSR": {
    //                                    "Age": 63.75
    //                                },
    //                                "SSR": {
    //                                    "Age": 2.0
    //                                },
    //                                "available": [
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    true,
    //                                    false,
    //                                    false,
    //                                    true,
    //                                    false,
    //                                    true,
    //                                    false,
    //                                    true,
    //                                    true,
    //                                    true,
    //                                    false
    //                                ]
    //                            },
    //                            "295": {
    //                                "BVR": {
    //                                    "Age": 2.0
    //                                },
    //                                "FSS": {
    //                                    "Age": 2.0
    //                                },
    //                                "IAR": {
    //                                    "Age": 2.0
    //                                },
    //                                "MAC": {
    //                                    "Age": 2.0
    //                                },
    //                                "MDA": {
    //                                    "Age": 2.0
    //                                },
    //                                "MFL": {
    //                                    "Age": 2.0
    //                                },
    //                                "MHG": {
    //                                    "Age": 2.0
    //                                },
    //                                "available": [
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    true,
    //                                    true,
    //                                    true,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    true,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    true,
    //                                    false,
    //                                    true,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    true,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    true,
    //                                    true,
    //                                    false,
    //                                    false,
    //                                    true,
    //                                    false,
    //                                    false,
    //                                    true
    //                                ]
    //                            },
    //                            "340": {
    //                                "MDA": {
    //                                    "G": 0,
    //                                    "L": 1,
    //                                    "Mode-3/A reply": 7621,
    //                                    "V": 0
    //                                },
    //                                "MDC": {
    //                                    "G": 0,
    //                                    "Last Measured Mode C Code": 349.75,
    //                                    "V": 0
    //                                },
    //                                "POS": {
    //                                    "RHO": 126.45703125,
    //                                    "THETA": 128.00720209018
    //                                },
    //                                "SID": {
    //                                    "SAC": 0,
    //                                    "SIC": 1
    //                                },
    //                                "TYP": {
    //                                    "RAB": 0,
    //                                    "SIM": 0,
    //                                    "TST": 0,
    //                                    "TYP": 5
    //                                },
    //                                "available": [
    //                                    false,
    //                                    false,
    //                                    true,
    //                                    true,
    //                                    true,
    //                                    false,
    //                                    true,
    //                                    true
    //                                ]
    //                            },
    //                            "380": {
    //                                "ADR": {
    //                                    "Target Address": 6700198
    //                                },
    //                                "BVR": {
    //                                    "Barometric Vertical Rate": -31.25
    //                                },
    //                                "FSS": {
    //                                    "AH": 0,
    //                                    "AM": 0,
    //                                    "Altitude": 35000.0,
    //                                    "MV": 0
    //                                },
    //                                "IAR": {
    //                                    "Indicated Air Speed": 266.0
    //                                },
    //                                "ID": {
    //                                    "Target Identification": "DLH9CK  "
    //                                },
    //                                "MAC": {
    //                                    "Mach Number": 0.784
    //                                },
    //                                "MHG": {
    //                                    "Magnetic Heading": 119.88281244544
    //                                },
    //                                "available": [
    //                                    false,
    //                                    false,
    //                                    true,
    //                                    true,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    true,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    true,
    //                                    false,
    //                                    true,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    true,
    //                                    true,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    true,
    //                                    true,
    //                                    true
    //                                ]
    //                            },
    //                            "390": {
    //                                "CFL": {
    //                                    "CFL": 350.0
    //                                },
    //                                "CSN": {
    //                                    "Callsign": "DLH9CK "
    //                                },
    //                                "DEP": {
    //                                    "Departure Airport": "EDDF"
    //                                },
    //                                "DST": {
    //                                    "Destination Airport": "LBSF"
    //                                },
    //                                "FCT": {
    //                                    "FR1/FR2": 0,
    //                                    "GAT/OAT": 1,
    //                                    "HPR": 0,
    //                                    "RVSM": 1
    //                                },
    //                                "IFI": {
    //                                    "NBR": 63256965,
    //                                    "TYP": 1
    //                                },
    //                                "TAC": {
    //                                    "Type of Aircraft": "A320"
    //                                },
    //                                "TAG": {
    //                                    "SAC": 0,
    //                                    "SIC": 0
    //                                },
    //                                "WTC": {
    //                                    "Wake Turbulence Category": "M"
    //                                },
    //                                "available": [
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    true,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    true,
    //                                    false,
    //                                    true,
    //                                    true,
    //                                    true,
    //                                    true,
    //                                    true,
    //                                    true,
    //                                    true,
    //                                    true,
    //                                    true
    //                                ]
    //                            },
    //                            "500": {
    //                                "AA": {
    //                                    "AA (X-Component)": 0.5,
    //                                    "AA (Y-Component)": 0.75
    //                                },
    //                                "ABA": {
    //                                    "ABA": 0.25
    //                                },
    //                                "AGA": {
    //                                    "AGA": 1593.75
    //                                },
    //                                "APC": {
    //                                    "APC (X-Component)": 40.0,
    //                                    "APC (Y-Component)": 63.0
    //                                },
    //                                "APW": {
    //                                    "APW (Latitude Component)": 0.00056326305,
    //                                    "APW (Longitude Component)": 0.00050961895
    //                                },
    //                                "ARC": {
    //                                    "ARC": 100.0
    //                                },
    //                                "ATV": {
    //                                    "ATV (X-Component)": 2.75,
    //                                    "ATV (Y-Component)": 4.75
    //                                },
    //                                "available": [
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    false,
    //                                    true,
    //                                    true,
    //                                    true,
    //                                    true,
    //                                    true,
    //                                    true,
    //                                    true,
    //                                    false,
    //                                    true
    //                                ]
    //                            },
    //                            "FSPEC": [
    //                                true,
    //                                false,
    //                                true,
    //                                true,
    //                                true,
    //                                true,
    //                                true,
    //                                true,
    //                                true,
    //                                true,
    //                                false,
    //                                true,
    //                                true,
    //                                true,
    //                                true,
    //                                true,
    //                                true,
    //                                true,
    //                                true,
    //                                false,
    //                                true,
    //                                true,
    //                                true,
    //                                true,
    //                                false,
    //                                false,
    //                                false,
    //                                false,
    //                                false,
    //                                true,
    //                                true,
    //                                false
    //                            ]
    //                        }
    //                    ]
    //                },
    //                "length": 151
    //            }
    //        ]
    //    }

    loginf << "cat062 1.16 test: data block" << logendl;

    assert (json_data->find ("data_blocks") != json_data->end());
    assert (json_data->at("data_blocks").is_array());
    assert (json_data->at("data_blocks").size() == 1);
    assert (json_data->at("data_blocks")[0]["category"] == 62);
    assert (json_data->at("data_blocks")[0]["length"] == 64);

    loginf << "cat062 1.16 test: num records" << logendl;
    assert (json_data->at("data_blocks")[0].at("content").at("records").size() == 1);

    //    ; FSPEC: 0x bf cf 3d 0b 00


    loginf << "cat062 1.16 test: fspec" << logendl;
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("FSPEC").size() == 5*8);

    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("FSPEC")
            == std::vector<bool>({1,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,1,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0}));

    //    ; Data Record:
    //    ;  I062/010: =0x 00 05
    //    ;  Data Source Identifier: 0x0005 (SAC=0; SIC=5)

    loginf << "cat062 1.16 test: 010" << logendl;
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("010").at("SAC") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("010").at("SIC") == 5);

    //    ;  I062/015: =0x 15
    //    ;  Service Identification: 21

    loginf << "cat062 1.16 test: 015" << logendl;
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("015").at("Service Identification") == 21);

    //    ;  I062/070: =0x 70 88 9d
    //    ;  Time of Track Information: 0x70889d (7375005; 16:00:17.227 UTC)

    loginf << "cat062 1.16 test: 070" << logendl;
    double tmp_d = json_data->at("data_blocks")[0].at("content").at("records")[0].at("070").at("Time Of Track Information");
    assert (fabs(tmp_d- 57617.2265625) < 10e-6);

    //    ;  I062/105: =0x 00 63 f3 2e  ff dd 64 f6
    //    ;  Calculated Position in WGS-84 Co-ordinates:
    //    ;   lat=6550318 (35:08:19.118N); lon=-2267914 (012:09:57.740W)

    loginf << "cat062 1.16 test: 105" << logendl;
    tmp_d = json_data->at("data_blocks")[0].at("content").at("records")[0].at("105").at("Latitude");
    assert (fabs(tmp_d-35.13864398) < 10e-4);
    tmp_d = json_data->at("data_blocks")[0].at("content").at("records")[0].at("105").at("Longitude");
    assert (fabs(tmp_d+ 12.1660387516) < 10e-4);

    //    ;  I062/100: =0x f7 93 02 f3  da 58
    //    ;  Calculated Track Position (Cartesian): x=-552190 (-149.079 nmi); y=-796072 (-214.922 nmi)

    loginf << "cat062 1.16 test: 100" << logendl;
    tmp_d = json_data->at("data_blocks")[0].at("content").at("records")[0].at("100").at("X");
    assert (fabs(tmp_d+276095) < 10e-4);
    tmp_d = json_data->at("data_blocks")[0].at("content").at("records")[0].at("100").at("Y");
    assert (fabs(tmp_d+398036) < 10e-4);

    //    ;  I062/185: =0x 01 fd 02 d5
    //    ;  Calculated Track Velocity (Cartesian): vx=509 (247.354 kts); vy=725 (352.322 kts)

    loginf << "cat062 1.16 test: 185" << logendl;
    tmp_d = json_data->at("data_blocks")[0].at("content").at("records")[0].at("185").at("Vx");
    assert (fabs(tmp_d-127.25) < 10e-4);
    tmp_d = json_data->at("data_blocks")[0].at("content").at("records")[0].at("185").at("Vy");
    assert (fabs(tmp_d-181.25) < 10e-4);

    //    ;  I062/210: =0x 00 00
    //    ;  Calculated Acceleration (Cartesian): ax=0.00 m/s**2; ay=0.00 m/s**2

    loginf << "cat062 1.16 test: 210" << logendl;
    tmp_d = json_data->at("data_blocks")[0].at("content").at("records")[0].at("210").at("Ax");
    assert (fabs(tmp_d) < 10e-4);
    tmp_d = json_data->at("data_blocks")[0].at("content").at("records")[0].at("210").at("Ay");
    assert (fabs(tmp_d) < 10e-4);


    //    ;  I062/060: =0x 0c 84
    //    ;  Track Mode 3/A Code: i=0; g=0; c=0; code=06204

    loginf << "cat062 1.16 test: 060" << logendl;
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("060").at("Mode-3/A reply") == 6204);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("060").at("CH") == 0);

    //    ;  I062/040: =0x 15 9d
    //    ;  Track Number: 5533

    loginf << "cat062 1.16 test: 040" << logendl;
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("040").at("Track Number") == 5533);

    //    ;  I062/080: =0x 19 03 01 58
    //    ;  Track Status:
    //    ;   MUL MRH=0 SRC=6 (default height) CNF
    //    ;   KOS=1
    //    ;   SSR

    loginf << "cat062 1.16 test: 080" << logendl;
    // 00011001
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("MON") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("SPI") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("MRH") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("SRC") == 6);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("CNF") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("FX") == 1);

    // 00000011
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("SIM") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("TSE") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("TSB") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("FPC") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("AFF") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("STP") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("KOS") == 1);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("FX2") == 1);

    // 00000001
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("AMA") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("MD4") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("ME") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("MI") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("MD5") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("FX3") == 1);

    // 01011000
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("CST") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("PSR") == 1);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("SSR") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("MDS") == 1);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("ADS") == 1);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("SUC") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("AAC") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("080").at("FX4") == 0);

    //    ;  I062/290: 0x 70 ff 24 ff
    //    ;  System Track Update Ages:
    //    ;   PSR age: 255 (63.75 sec)
    //    ;   SSR age: 36 (9.00 sec)
    //    ;   MDS age: 255 (63.75 sec)

    loginf << "cat062 1.16 test: 290" << logendl;

    // 01110000
    std::vector<bool> tmp_boolvec  = {0,1,1,1,0,0,0,0};
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("290").at("available") == tmp_boolvec);

    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("290").at("PSR").at("Age") == 63.75);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("290").at("SSR").at("Age") == 9.00);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("290").at("MDS").at("Age") == 63.75);

    //    ;  I062/136: =0x 05 f0
    //    ;  Measured Flight Level: 1520 (380.00 FL)

    loginf << "cat062 1.16 test: 136" << logendl;
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("136").at("Measured Flight Level") == 380.0);

    //    ;  I062/130: =0x 15 c6
    //    ;  Calculated Track Geometric Altitude: 5574 (348.38 FL)

    loginf << "cat062 1.16 test: 130" << logendl;
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("130").at("Altitude") == 34837.5);

    //    ;  I062/135: =0x 05 f0
    //    ;  Calculated Track Barometric Altitude: qnh=0; alt=1520 (380.00 FL)

    loginf << "cat062 1.16 test: 135" << logendl;
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("135").at("QNH") == 0.0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("135").at("Calculated Track Barometric Altitude") == 38000);

    //    ;  I062/220: =0x 00 00
    //    ;  Calculated Rate of Climb/Descent: 0 (0.00 ft/min)

    loginf << "cat062 1.16 test: 220" << logendl;
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("220").at("Rate of Climb/Descent") == 0.0);

    //    ;  I062/510: 0x 06 1b be
    //    ;  Composed Track Number:
    //    ;   Master track number: sui=6; stn=3551

    loginf << "cat062 1.16 test: 510" << logendl;
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("510").at("Composed Track Number")[0].at("SYSTEM UNIT IDENTIFICATION") == 6);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("510").at("Composed Track Number")[0].at("SYSTEM TRACK NUMBER") == 3551);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("510").at("Composed Track Number")[0].at("extend") == 0);

    loginf << "cat062 1.16 test: 340" << logendl;

    //    ;  I062/340: 0x 98 00 03 05  f0 0c 84
    // 1001 1000 0000 0000
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("340").at("available").size() == 8);

    tmp_boolvec = {1,0,0,1,1,0,0,0};
    //std::reverse(tmp_boolvec.begin(), tmp_boolvec.end());

    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("340").at("available")
            == tmp_boolvec);

    //    ;  Measured Information:
    //    ;   Sensor identification: SAC=0; SIC=3
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("340").at("SID").at("SAC") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("340").at("SID").at("SIC") == 3);
    //    ;   Last measured mode C code: v=0; g=0; code=1520 (380.00 FL)
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("340").at("MDC").at("V") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("340").at("MDC").at("G") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("340").at("MDC").at("Last Measured Mode C Code") == 380.0);
    //    ;   Last measured mode 3/A code: v=0; g=0; l=0; code=06204
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("340").at("MDA").at("V") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("340").at("MDA").at("G") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("340").at("MDA").at("L") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("340").at("MDA").at("Mode-3/A reply") == 6204);

    //     [--:--:--.---] U 16:00:17.227 -- 0x0005 A- T:US+ #5533 UV -149.079 -214.922                       A:06204 --- C: 380    -- ; G:348.38

}

void test_cat062_116 (jASTERIX::jASTERIX& jasterix)
{
    loginf << "cat062 1.16 test: start" << logendl;

    //    ; ASTERIX data block at pos 0: cat=62; len=64
    //  3e0040bfcf3d0b0000051570889d0063f32effdd64f6f79302f3da5801fd02d500000c84159d1903015870ff24ff05f015c605f00000061bbe98000305f00c84

    // echo -n 3e0040bfcf3d0b0000051570889d0063f32effdd64f6f79302f3da5801fd02d500000c84159d1903015870ff24ff05f015c605f00000061bbe98000305f00c84 | xxd -r -p > cat062ed1.16.bin


    const char *cat062_ed116 =
            "3e0040bfcf3d0b0000051570889d0063f32effdd64f6f79302f3da5801fd02d500000c84159d1903015870ff24ff05f015c605f00000061bbe98000305f00c84";
    char* target = new char[strlen(cat062_ed116)/2];

    size_t size = hex2bin (cat062_ed116, target);

    loginf << "cat062 1.16 test: src len " << strlen(cat062_ed116) << " bin len " << size << logendl;

    assert (size == 64);

    assert (jasterix.hasCategory(62));
    std::shared_ptr<jASTERIX::Category> cat062 = jasterix.category(62);
    assert (cat062->hasEdition("1.16"));
    cat062->setCurrentEdition("1.16");
    cat062->setCurrentMapping("");

    jasterix.decodeASTERIX(target, size, test_cat062_116_callback);

    delete[] target;

    loginf << "cat062 1.16 test: end" << logendl;
}






