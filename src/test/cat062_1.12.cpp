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

void test_cat062_callback (nlohmann::json& json_data, size_t num_frames, size_t num_records)
{
    loginf << "cat062 1.12 test: decoded " << num_frames << " frames, " << num_records << " records: " << json_data.dump(4)
               << logendl;

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

    loginf << "cat062 1.12 test: data block" << logendl;

    assert (json_data.find ("data_blocks") != json_data.end());
    assert (json_data.at("data_blocks").is_array());
    assert (json_data.at("data_blocks").size() == 1);
    assert (json_data.at("data_blocks")[0]["category"] == 62);
    assert (json_data.at("data_blocks")[0]["length"] == 151);

    loginf << "cat062 1.12 test: num records" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records").size() == 1);

    //    ; FSPEC: 0x bf df ef 06
    // 10111111110111111110111100000110

    loginf << "cat062 1.12 test: fspec" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("FSPEC").size() == 4*8);

    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("FSPEC")
            == std::vector<bool>({1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,0,0,0,0,0,1,1,0}));

    //    ;  I062/010: =0x 00 04
    //    ;  Data Source Identifier: 0x0004 (SAC=0; SIC=4)

    loginf << "cat062 1.12 test: 010" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("010").at("SAC") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("010").at("SIC") == 4);

    //    ;  I062/015: =0x e1
    //    ;  Service Identification: 225

    loginf << "cat062 1.12 test: 015" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("015").at("Service Identification") == 225);

    //    ;  I062/070: =0x 41 6f 40
    //    ;  Time of Track Information: 0x416f40 (4288320; 09:18:22.500 UTC)

    loginf << "cat062 1.12 test: 070" << logendl;
    double tmp_d = json_data.at("data_blocks")[0].at("content").at("records")[0].at("070").at("Time Of Track Information");
    assert (fabs(tmp_d-33502.5) < 10e-6);

    //    ;  I062/105: =0x 00 81 52 c4  00 31 4d 24
    //    ;  Calculated Position in WGS-84 Co-ordinates:
    //    ;   lat=8475332 (45:27:54.806N); lon=3231012 (017:19:56.996E)

    loginf << "cat062 1.12 test: 105" << logendl;
    tmp_d = json_data.at("data_blocks")[0].at("content").at("records")[0].at("105").at("Latitude");
    assert (fabs(tmp_d-45.4652237892) < 10e-4);
    tmp_d = json_data.at("data_blocks")[0].at("content").at("records")[0].at("105").at("Longitude");
    assert (fabs(tmp_d-17.3324990273) < 10e-4);

    //    ;  I062/100: =0x 07 f4 6a f9  43 b1
    //    ;  Calculated Track Position (Cartesian): x=521322 (140.746 nmi); y=-441423 (-119.175 nmi)

    loginf << "cat062 1.12 test: 100" << logendl;
    tmp_d = json_data.at("data_blocks")[0].at("content").at("records")[0].at("100").at("X");
    assert (fabs(tmp_d-260661) < 10e-4);
    tmp_d = json_data.at("data_blocks")[0].at("content").at("records")[0].at("100").at("Y");
    assert (fabs(tmp_d+220711.5) < 10e-4);

    //    ;  I062/185: =0x 03 3e fe 1e
    //    ;  Calculated Track Velocity (Cartesian): vx=830 (403.348 kts); vy=-482 (-234.233 kts)

    loginf << "cat062 1.12 test: 185" << logendl;
    tmp_d = json_data.at("data_blocks")[0].at("content").at("records")[0].at("185").at("Vx");
    assert (fabs(tmp_d-207.5) < 10e-4);
    tmp_d = json_data.at("data_blocks")[0].at("content").at("records")[0].at("185").at("Vy");
    assert (fabs(tmp_d+120.5) < 10e-4);

    //    ;  I062/210: =0x 00 00
    //    ;  Calculated Acceleration (Cartesian): ax=0.00 m/s**2; ay=0.00 m/s**2

    loginf << "cat062 1.12 test: 210" << logendl;
    tmp_d = json_data.at("data_blocks")[0].at("content").at("records")[0].at("210").at("Ax");
    assert (fabs(tmp_d) < 10e-4);
    tmp_d = json_data.at("data_blocks")[0].at("content").at("records")[0].at("210").at("Ay");
    assert (fabs(tmp_d) < 10e-4);

    //    ;  I062/060: =0x 0f 91
    //    ;  Track Mode 3/A Code: i=0; g=0; c=0; code=07621

    loginf << "cat062 1.12 test: 060" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("060").at("Mode-3/A reply") == 7621);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("060").at("CH") == 0);

    //    ;  I062/380: 0x e3 05 01 0c  66 3c a6 10  c2 39 0c b8  20 55 40 05
    //    ;          + 0x 78 ff fb 01  0a 00 62
    //    ;  Aircraft Derived Data:

    loginf << "cat062 1.12 test: 380" << logendl;
    // available: e3 05 01 0c

    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("380").at("available").size() == 4*8);

    std::vector<bool> tmp_boolvec ({1,1,1,0,0,0,1,1,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,0,0,0,1,1,0,0});

    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("380").at("available")
            == tmp_boolvec);

    //    ;   Target address: 0x663ca6
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("380").at("ADR").at("Target Address") == 6700198);
    //    ;   Target identification: "DLH9CK  "
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("380").at("ID").at("Target Identification") == "DLH9CK  ");
    //    ;   Magnetic heading: 21824 (119.883 deg)
    tmp_d = json_data.at("data_blocks")[0].at("content").at("records")[0].at("380").at("MHG").at("Magnetic Heading");
    assert (fabs(tmp_d-119.88281244544) < 10e-4);
    //    ;   Final state selected altitude: mv=0; ah=0; am=0; alt=1400(350.00 FL)
    tmp_d = json_data.at("data_blocks")[0].at("content").at("records")[0].at("380").at("FSS").at("Altitude");
    assert (fabs(tmp_d-35000.0) < 10e-4);
    //    ;   Barometric vertical rate: -5 (-31.25 ft/min)
    tmp_d = json_data.at("data_blocks")[0].at("content").at("records")[0].at("380").at("BVR").at("Barometric Vertical Rate");
    assert (fabs(tmp_d+31.25) < 10e-4);
    //    ;   Indicated air speed: 266 (kts)
    tmp_d = json_data.at("data_blocks")[0].at("content").at("records")[0].at("380").at("IAR").at("Indicated Air Speed");
    assert (fabs(tmp_d-266) < 10e-4);
    //    ;   Mach number: 98 (0.784 Mach)
    tmp_d = json_data.at("data_blocks")[0].at("content").at("records")[0].at("380").at("MAC").at("Mach Number");
    assert (fabs(tmp_d-0.784) < 10e-4);

    //    ;  I062/040: =0x 13 de
    //    ;  Track Number: 5086
    loginf << "cat062 1.12 test: 040" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("040").at("Track Number") == 5086);

    //    ;  I062/080: =0x 0d 13 01 48
    //    ;  Track Status:
    //    ;   MUL MRH=0 SRC=3 (triangulation) CNF
    //    ;   FPC KOS=1
    //    ;   SSR MDS
    loginf << "cat062 1.12 test: 080" << logendl;
    // 00001101
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("MON") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("SPI") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("MRH") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("SRC") == 3);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("CNF") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("FX") == 1);

    // 00010011
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("SIM") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("TSE") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("TSB") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("FPC") == 1);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("AFF") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("STP") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("KOS") == 1);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("FX2") == 1);

    // 00000001
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("AMA") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("MD4") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("ME") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("MI") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("MD5") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("FX3") == 1);

    // 01001000
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("CST") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("PSR") == 1);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("SSR") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("MDS") == 0); //TODO ????
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("ADS") == 1);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("SUC") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("080").at("AAC") == 0);

    //    ;  I062/290: 0x 75 20 ff 08  08 ff ff
    //    ;  System Track Update Ages:
    //    ;   PSR age: 255 (63.75 sec)
    //    ;   SSR age: 8 (2.00 sec)
    //    ;   MDS age: 8 (2.00 sec)
    //    ;   ES age: 255 (63.75 sec)
    //    ;   MLT age: 255 (63.75 sec)

    loginf << "cat062 1.12 test: 290" << logendl;

    // 0111 0101 0010 0000
    tmp_boolvec  = {0,1,1,1,0,1,0,1,0,0,1,0,0,0,0,0};
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("290").at("available") == tmp_boolvec);

    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("290").at("PSR").at("Age") == 63.75);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("290").at("SSR").at("Age") == 2.00);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("290").at("MDS").at("Age") == 2.00);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("290").at("ES").at("Age") == 63.75);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("290").at("MLT").at("Age") == 63.75);

    //    ;  I062/200: =0x 00
    //    ;  Mode of Movement: trans=0 (constant course); long=0 (constant groundspeed); vert=0 (level)
    loginf << "cat062 1.12 test: 200" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("200").at("TRANS") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("200").at("LONG") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("200").at("VERT") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("200").at("ADF") == 0);

    //    ;  I062/295: 0x 93 11 41 01  c0 08 08 08  08 08 08 08
    //    ;  Track Data Ages:
    //    ;   Measured flight level age: 8 (2.00 sec)
    //    ;   Mode 3/A age: 8 (2.00 sec)
    //    ;   Magnetic heading age: 8 (2.00 sec)
    //    ;   Final state selected altitude age: 8 (2.00 sec)
    //    ;   Barometric vertical rate age: 8 (2.00 sec)
    //    ;   Indicated airspeed data age: 8 (2.00 sec)
    //    ;   Mach number data age: 8 (2.00 sec)

    loginf << "cat062 1.12 test: 295" << logendl;

    // available 93 11 41 01  c0
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("295").at("available").size() == 5*8);

    tmp_boolvec = {1,0,0,1,0,0,1,1,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0};

    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("295").at("available")
            == tmp_boolvec);

    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("295").at("MFL").at("Age") == 2.00);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("295").at("MDA").at("Age") == 2.00);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("295").at("MHG").at("Age") == 2.00);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("295").at("FSS").at("Age") == 2.00);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("295").at("BVR").at("Age") == 2.00);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("295").at("IAR").at("Age") == 2.00);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("295").at("MAC").at("Age") == 2.00);

    //    ;  I062/136: =0x 05 77
    //    ;  Measured Flight Level: 1399 (349.75 FL)
    loginf << "cat062 1.12 test: 136" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("136").at("Measured Flight Level") == 349.75);

    //    ;  I062/135: =0x 05 77
    //    ;  Calculated Track Barometric Altitude: qnh=0; alt=1399 (349.75 FL)
    loginf << "cat062 1.12 test: 135" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("135").at("Calculated Track Barometric Altitude") == 34975.0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("135").at("QNH") == 0);

    //    ;  I062/220: =0x 00 00
    //    ;  Calculated Rate of Climb/Descent: 0 (0.00 ft/min)
    loginf << "cat062 1.12 test: 220" << logendl;
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("220").at("Rate of Climb/Descent") == 0.0);

    //    ;  I062/390: 0x ff a1 00 00  00 44 4c 48  39 43 4b 20  43 c5 39 85
    //    ;          + 0x 44 41 33 32  30 4d 45 44  44 46 4c 42  53 46 05 78
    loginf << "cat062 1.12 test: 390" << logendl;

    //available ff a1 00

    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("390").at("available").size() == 3*8);

    tmp_boolvec = {1,1,1,1,1,1,1,1,1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0};
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("390").at("available")
            == tmp_boolvec);

    //    ;  Flight Plan Related Data:
    //    ;   FPPS identification tag: 0x0000 (SAC=0; SIC=0)
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("390").at("TAG").at("SAC") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("390").at("TAG").at("SIC") == 0);
    //    ;   Callsign: [DLH9CK ]
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("390").at("CSN").at("Callsign") == "DLH9CK ");
    //    ;   IFPS flight ID: typ=1; nbr=63256965
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("390").at("IFI").at("NBR") == 63256965);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("390").at("IFI").at("TYP") == 1);
    //    ;   Flight category: GAT/OAT=1; FR=0; RVSM=1; HPR=0
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("390").at("FCT").at("GAT/OAT") == 1);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("390").at("FCT").at("FR1/FR2") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("390").at("FCT").at("RVSM") == 1);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("390").at("FCT").at("HPR") == 0);
    //    ;   Type of aircraft: [A320]
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("390").at("TAC").at("Type of Aircraft") == "A320");
    //    ;   Wake turbulence category: M
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("390").at("WTC").at("Wake Turbulence Category") == "M");
    //    ;   Departure airport: [EDDF]
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("390").at("DEP").at("Departure Airport") == "EDDF");
    //    ;   Destination airport: [LBSF]
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("390").at("DST").at("Destination Airport") == "LBSF");
    //    ;   Current cleared flight level: 1400 (1/4 FL)
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("390").at("CFL").at("CFL") == 350.0);

    //    ;  I062/500: 0x bf 80 00 50  00 7e 00 69  00 5f ff 01  0b 13 02 03
    //    ;          + 0x 10

    loginf << "cat062 1.12 test: 500" << logendl;

    //available bf 80

    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("500").at("available").size() == 2*8);

    tmp_boolvec = {1,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0};
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("500").at("available")
            == tmp_boolvec);

    //    ;  Estimated Accuracies:
    //    ;   Estimated accuracy of track position (Cartesian): x=80 (40.0 mtr); y=126 (63.0 mtr)
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("500").at("APC").at("APC (X-Component)") == 40.0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("500").at("APC").at("APC (Y-Component)") == 63.0);
    //    ;   Estimated accuracy of track position (WGS-84): lat=105 (0.0005633 deg); lon=95 (0.0005096 deg)
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("500").at("APW").at("APW (Latitude Component)") == 0.00056326305);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("500").at("APW").at("APW (Longitude Component)") == 0.00050961895);
    //    ;   Estimated accuracy of calculated track geometric altitude: 255 (15.94 FL)
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("500").at("AGA").at("AGA") == 1593.75);
    //    ;   Estimated accuracy of calculated track barometric altitude: 1 (0.25 FL)
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("500").at("ABA").at("ABA") == 0.25);
    //    ;   Estimated accuracy of track velocity (Cartesian): x=11 (2.75 m/s); y=19 (4.75 m/s)
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("500").at("ATV").at("ATV (X-Component)") == 2.75);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("500").at("ATV").at("ATV (Y-Component)") == 4.75);
    //    ;   Estimated accuracy of acceleration (Cartesian): x=2 (0.50 m/s**2); y=3 (0.75 m/s**2)
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("500").at("AA").at("AA (X-Component)") == 0.50);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("500").at("AA").at("AA (Y-Component)") == 0.75);
    //    ;   Estimated accuracy of rate of climb/descent: 16 (100.00 ft/min)
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("500").at("ARC").at("ARC") == 100.00);

    //    ;  I062/340: 0x dc 32 14 7e  75 5b 07 05  77 2f 91 a0

    loginf << "cat062 1.12 test: 340" << logendl;

    //available dc

    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("340").at("available").size() == 8);

    tmp_boolvec = {1,1,0,1,1,1,0,0};
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("340").at("available")
            == tmp_boolvec);

    //    ;  Measured Information:
    //    ;   Sensor identification: SAC=0; SIC=1
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("340").at("SID").at("SAC") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("340").at("SID").at("SIC") == 1);
    //    ;   Measured position: rho=32373 (126.457 nmi); theta=23303 (128.007 deg)
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("340").at("POS").at("RHO") == 126.45703125);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("340").at("POS").at("THETA") == 128.00720209018);
    //    ;   Last measured mode C code: v=0; g=0; code=1399 (349.75 FL)
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("340").at("MDC").at("V") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("340").at("MDC").at("G") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("340").at("MDC").at("Last Measured Mode C Code") == 349.75);
    //    ;   Last measured mode 3/A code: v=0; g=0; l=1; code=07621
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("340").at("MDA").at("V") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("340").at("MDA").at("G") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("340").at("MDA").at("L") == 1);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("340").at("MDA").at("Mode-3/A reply") == 7621);
    //    ;   Report type: typ=5 (single mode S roll-call); sim=0; rab=0; tst=0
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("340").at("TYP").at("TYP") == 5);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("340").at("TYP").at("SIM") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("340").at("TYP").at("RAB") == 0);
    assert (json_data.at("data_blocks")[0].at("content").at("records")[0].at("340").at("TYP").at("TST") == 0);

    //     [--:--:--.---] U 09:18:22.500 -- 0x0004 A- T:US+ #5086 UV  140.746 -119.175 S:0x663ca6 =DLH9CK  = A:07621 --- C: 349.75 -- ; cor

}

void test_cat062_112 (jASTERIX::jASTERIX& jasterix)
{
    loginf << "cat062 1.12 test: start" << logendl;

    //    ; ASTERIX data block at pos 0: cat=62; len=151
    // 3e0097bfdfef060004e1416f40008152c400314d2407f46af943b1033efe1e00000f91e305010c663ca610c2390cb82055400578fffb010a006213de0d1301487520ff0808ffff0093114101c008080808080808057705770000ffa1000000444c4839434b2043c5398544413332304d454444464c4253460578bf800050007e0069005fff010b13020310dc00017e755b0705772f91a0

    // echo -n 3e0097bfdfef060004e1416f40008152c400314d2407f46af943b1033efe1e00000f91e305010c663ca610c2390cb82055400578fffb010a006213de0d1301487520ff0808ffff0093114101c008080808080808057705770000ffa1000000444c4839434b2043c5398544413332304d454444464c4253460578bf800050007e0069005fff010b13020310dc00017e755b0705772f91a0 | xxd -r -p > cat062ed1.12.bin


    const char *cat062_ed112 =
            "3e0097bfdfef060004e1416f40008152c400314d2407f46af943b1033efe1e00000f91e305010c663ca610c2390cb82055400578fffb010a006213de0d1301487520ff0808ffff0093114101c008080808080808057705770000ffa1000000444c4839434b2043c5398544413332304d454444464c4253460578bf800050007e0069005fff010b13020310dc00017e755b0705772f91a0";
    char* target = new char[strlen(cat062_ed112)/2];

    size_t size = hex2bin (cat062_ed112, target);

    loginf << "cat062 1.12 test: src len " << strlen(cat062_ed112) << " bin len " << size << logendl;

    assert (size == 151);

    assert (jasterix.hasCategory(62));
    assert (jasterix.hasEdition(62, "1.12"));
    jasterix.setEdition(62, "1.12");
    jasterix.setMapping(62, "");

    jasterix.decodeASTERIX(target, size, test_cat062_callback);

    loginf << "cat062 1.12 test: end" << logendl;
}






