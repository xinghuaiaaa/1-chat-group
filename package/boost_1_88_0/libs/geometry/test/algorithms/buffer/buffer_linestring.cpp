// Boost.Geometry (aka GGL, Generic Geometry Library)
// Unit Test

// Copyright (c) 2012-2019 Barend Gehrels, Amsterdam, the Netherlands.

// This file was modified by Oracle on 2016-2021.
// Modifications copyright (c) 2016-2021, Oracle and/or its affiliates.
// Contributed and/or modified by Adam Wulkiewicz, on behalf of Oracle

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "test_buffer.hpp"

#include <boost/geometry/algorithms/buffer.hpp>
#include <boost/geometry/core/coordinate_type.hpp>

#include <boost/geometry/strategies/strategies.hpp>

#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include "test_common/test_point.hpp"


static std::string const simplex = "LINESTRING(0 0,4 5)";
static std::string const simplex_vertical = "LINESTRING(0 0,0 1)";
static std::string const simplex_horizontal = "LINESTRING(0 0,1 0)";

static std::string const straight = "LINESTRING(0 0,4 5,8 10)";
static std::string const one_bend = "LINESTRING(0 0,4 5,7 4)";
static std::string const two_bends = "LINESTRING(0 0,4 5,7 4,10 6)";

static std::string const bend_near_start1 = "LINESTRING(0 0,1 0,5 2)";
static std::string const bend_near_start2 = "LINESTRING(0 0,1 0,2 1.5,5 3)";

static std::string const overlapping = "LINESTRING(0 0,4 5,7 4,10 6,10 2,2 2)";
static std::string const overlapping_rev = "LINESTRING(2 2,10 2,10 6,7 4,4 5,0 0)";

static std::string const curve = "LINESTRING(2 7,3 5,5 4,7 5,8 7)";
static std::string const tripod = "LINESTRING(5 0,5 5,1 8,5 5,9 8)"; // with spike

static std::string const degenerate0 = "LINESTRING()";
static std::string const degenerate1 = "LINESTRING(5 5)";
static std::string const degenerate2 = "LINESTRING(5 5,5 5)";
static std::string const degenerate3 = "LINESTRING(5 5,5 5,5 5)";
static std::string const degenerate4 = "LINESTRING(5 5,5 5,4 4,5 5,5 5)";

static std::string const for_collinear = "LINESTRING(2 0,0 0,0 4,6 4,6 0,4 0)";
static std::string const for_collinear2 = "LINESTRING(2 1,2 0,0 0,0 4,6 4,6 0,4 0,4 1)";

static std::string const chained2 = "LINESTRING(0 0,1 1,2 2)";
static std::string const chained3 = "LINESTRING(0 0,1 1,2 2,3 3)";
static std::string const chained4 = "LINESTRING(0 0,1 1,2 2,3 3,4 4)";

static std::string const field_sprayer1 = "LINESTRING(76396.40464822574 410095.6795147947,76397.85016212701 410095.211865792,76401.30666443033 410095.0466387949,76405.05892643372 410096.1007777959,76409.45103273794 410098.257640797,76412.96309264141 410101.6522238015)";
static std::string const aimes120 = "LINESTRING(-2.505218 52.189211,-2.505069 52.189019,-2.504941 52.188854)";
static std::string const aimes167 = "LINESTRING(-2.378569 52.312133,-2.37857 52.312127,-2.378544 52.31209)";
static std::string const aimes175 = "LINESTRING(-2.3116 52.354326,-2.311555 52.35417,-2.311489 52.354145,-2.311335 52.354178)";
static std::string const aimes171 = "LINESTRING(-2.393161 52.265087,-2.393002 52.264965,-2.392901 52.264891)";
static std::string const aimes181 = "LINESTRING(-2.320686 52.43505,-2.320678 52.435016,-2.320697 52.434978,-2.3207 52.434977,-2.320741 52.434964,-2.320807 52.434964,-2.320847 52.434986,-2.320903 52.435022)";

static std::string const crossing = "LINESTRING(0 0,10 10,10 0,0 10)";

// Simplified cases from multi_linestring tesets:
static std::string const mikado1 = "LINESTRING(11.406143344709896325639419956133 0.75426621160409546007485914742574,12 1,11.403846153846153299582510953769 0.75)";

static std::string const mysql_report_2015_03_02a = "LINESTRING(0 0,0 5,5 5,5 0,0 0)"; // closed
static std::string const mysql_report_2015_03_02b = "LINESTRING(0 1,0 5,5 5,5 0,1 0)"; // not closed, 1 difference
static std::string const mysql_report_2015_03_02c = "LINESTRING(0 2,0 5,5 5,5 0,2 0)"; // not closed, 2 difference

static std::string const mysql_report_2015_04_01 = "LINESTRING(103 5,107 2,111 4,116 -1,115 0,112 4)";

static std::string const mysql_report_2015_04_10a = "LINESTRING(1.922421e+307 1.520384e+308, 15 42, 89 -93,-89 -22)";
static std::string const mysql_report_2015_04_10b = "LINESTRING(15 42, 89 -93,-89 -22, 1.922421e+307 1.520384e+308)";
static std::string const mysql_report_2015_04_10c = "LINESTRING(15 42, 1.922421e+307 1.520384e+308, 89 -93,-89 -22)";
static std::string const mysql_report_2015_04_10d = "LINESTRING(1.922421e+307 1.520384e+308, 1.923421e+307 1.521384e+308, 15 42, 89 -93,-89 -22)";
static std::string const mysql_report_2015_04_10e = "LINESTRING(15 42, 89 -93,-89 -22, 1.922421e+307 1.520384e+308, 1.923421e+307 1.521384e+308)";
static std::string const mysql_report_2015_04_10f = "LINESTRING(15 42, 1.922421e+307 1.520384e+308, 1.923421e+307 1.521384e+308, 89 -93,-89 -22)";
static std::string const mysql_report_2015_04_10g = "LINESTRING(15 42, 89 -93,-89 -22)";

static std::string const mysql_report_2015_06_11 = "LINESTRING("
    "-155.9300341531310000 4.1672727531600900, -14.0079144546799000 "
    "-12.2485554508160000, 176.9503531590800000 -3.0930641354495000, "
    "32.6863251871831000 -17.9691125862157000, -17.7739746299451000 "
    "41.3177973084700000, -36.0310834162082000 59.9486214620753000, "
    "153.1574937017440000 46.3007892930418000, 172.7795126069240000 "
    "19.5367061763707000, -85.6306040220105000 35.0128339347489000, "
    "-61.1404987988716000 0.3278080608359490, -127.5034592987520000 "
    "18.6202802642343000, 114.5567005754250000 -83.7227732658958000, "
    "-66.1134822881378000 -75.2141906159065000, -93.7363999307791000 "
    "49.3124773443269000, -8.7182702071584100 56.2071174970861000, "
    "7.7959787229988800 60.8845281744769000, 13.0420633931840000 "
    "58.8150539662759000, -89.9754374613871000 26.4546501154335000, "
    "-44.5746548960799000 -88.8122262334508000, -178.4807616092640000 "
    "10.7770331393820000, 161.8238702890570000 -42.3894892597522000, "
    "136.2382890452810000 28.6261570633511000, 49.6788041059295000 "
    "61.7724885566963000, 52.7876201424690000 -61.9246644395984000, "
    "-162.7456296900030000 11.7183989853218000, 115.6208648232840000 "
    "51.0941612539320000, -48.7772321835054000 50.4339743128205000)";

static std::string const mysql_report_2015_09_08a = "LINESTRING(1 1, 2 1, 1.765258e+308 4, -1 1, 10 4)";
static std::string const mysql_report_2015_09_08b = "LINESTRING(2199023255556 16777218, 32770 8194, 1.417733e+308 7.823620e+307, -8 -9, 2147483649 20)";
static std::string const mysql_report_2015_09_08c = "LINESTRING(-5 -8, 2 8, 2.160023e+307 1.937208e+307, -4 -3, -5 -4, 8796093022208 281474976710653)";

static std::string const mysql_23023665 = "LINESTRING(0 0, 0 5, 5 5, 5 0, 0 0)";

static std::string const mysql_25662426 = "LINESTRING(170 4756, 168 4756, 168 4759, 168 4764, 171 4764, 171 4700)";
static std::string const mysql_25662426a = "LINESTRING(170 4756, 168 4756, 168 4759, 168 4764, 171 4764, 171 4750)";

static std::string const issue_596 = "LINESTRING(292979.660 6688731.370, 292979.600 6688733.420, 292979.540 6688735.440, 292979.540 6688735.500)";
static std::string const issue_803 = "LINESTRING(2773.6899360413681 -17.493356405074767,2767.9084041267429 -9.0288291122330797,2765.7403296587586 -1.6929054585683376,2765.3273630934282 6.6613381477509392e-16,2762.8495637014462 10.157432751410026,2759.9587977441333 24.829280059133669,2756.3453402974928 38.372523727680374,2752.0091913615238 45.708447381345117)";
static std::string const issue_988 = "LINESTRING(0.10144 0.034912,0.106079 0.035156,0.109375 0.034302,0.114502 0.035889,0.116333 0.036743,0.117065 0.036499,0.121582 0.035156,0.12439 0.029175,0.123779 0.026978,0.12146 0.025513,0.119507 0.025513,0.118164 0.025513,0.114624 0.025757,0.111694 0.026001,0.108887 0.02832,0.105957 0.028442,0.099854 0.027344,0.095581 0.029419,0.093506 0.031128,0.090576 0.032593,0.085571 0.032959,0.082153 0.035522,0.081421 0.039307,0.082275 0.044067,0.083862 0.047485,0.08606 0.049805,0.087891 0.051025,0.090942 0.05188,0.094727 0.051392,0.100098 0.050049,0.10144 0.05249,0.100952 0.054932,0.098633 0.05835,0.098267 0.062134,0.098755 0.064697,0.098511 0.067383,0.113892 0.068848,0.110718 0.065552,0.109619 0.064331,0.111084 0.063965,0.118042 0.0625,0.115234 0.049805,0.117676 0.049194,0.118774 0.046997,0.119385 0.04541,0.119507 0.043945,0.116089 0.041138,0.116089 0.041016,0.11438 0.040894,0.11145 0.041016,0.109009 0.042114,0.106079 0.04248,0.102417 0.041138,0.102051 0.040039)";

static std::string const issue_1084 = "LINESTRING(269.3 -7.03, 270.23 -3.57, 270.54 0, 270.54 100, 270.23 103.57, 269.3 107.03, 267.79 110.27, 265.73 113.2, 263.2 115.73, 258.89 118.43, 254.28 108.54, 248.52 109, 80 97.21, 72.71 95.87, 67.46 93.82, 62.61 90.92, 58.32 87.27, 54.69 82.95, 51.83 78.08, 49.81 72.81, 48.7 67.28, 48.45 63.38, 48.52 34.87, 49.29 29.26, 50.96 23.87, 54.69 17.05, 58.32 12.73, 62.61 9.08, 65.57 7.18, 70.68 4.81, 76.12 3.31, 80 2.79, 248.52 -9, 254.28 -8.54, 258.89 -18.43, 263.2 -15.73, 265.73 -13.2, 267.79 -10.27, 269.3 -7.03, 270.23 -3.57, 270.54 0, 270.54 100, 270.23 103.57, 269.3 107.03, 267.79 110.27, 265.73 113.2, 264 114.93, 256.24 107.17, 251.86 108.16, 248.58 108.2, 76.24 95.9, 70.93 94.43, 65.94 92.11, 61.4 88.99, 57.44 85.16, 54.18 80.72, 51.69 75.8, 50.06 70.54, 49.25 63.38, 49.32 34.93, 50.06 29.46, 51.69 24.2, 55.36 17.49, 58.9 13.28, 63.1 9.71, 67.83 6.89, 70.93 5.57, 76.24 4.1, 80.06 3.59, 248.58 -8.2, 251.86 -8.16, 256.24 -7.17, 258.89 -18.43)";
static std::string const issue_1250 = "LINESTRING(3 1, 4 2, 4 4, 2 4, 2 2, 3 2)";
static std::string const issue_1250_rev = "LINESTRING(3 2, 2 2, 2 4, 4 4, 4 2, 3 1)";

template <bool Clockwise, typename P>
void test_all()
{
    typedef bg::model::linestring<P> linestring;
    typedef bg::model::polygon<P, Clockwise> polygon;
    typedef typename bg::coordinate_type<P>::type coor_type;

    bg::strategy::buffer::join_miter join_miter;
    bg::strategy::buffer::join_round join_round(100);
    bg::strategy::buffer::join_round_by_divide join_round_by_divide(4);
    bg::strategy::buffer::end_flat end_flat;
    bg::strategy::buffer::end_round end_round(100);

    // For testing MySQL issues, which uses 32 by default
    bg::strategy::buffer::end_round end_round32(32);
    bg::strategy::buffer::join_round join_round32(32);

    ut_settings const settings;
    ut_settings const specific_settings
            = BOOST_GEOMETRY_CONDITION((std::is_same<coor_type, long double>::value))
              ? ut_settings(0.02) : settings;

    // Simplex (join-type is not relevant)
    test_one<linestring, polygon>("simplex", simplex, join_miter, end_flat, 19.209, 1.5);
    test_one<linestring, polygon>("simplex", simplex, join_miter, end_round, 26.2733, 1.5);

    // Should be about PI + 2
    test_one<linestring, polygon>("simplex_vertical", simplex_vertical, join_round, end_round, 5.14, 1);
    test_one<linestring, polygon>("simplex_horizontal", simplex_horizontal, join_round, end_round, 5.14, 1);

    // Should be a bit less than PI + 2
    test_one<linestring, polygon>("simplex_vertical32", simplex_vertical, join_round32, end_round32, 5.12145, 1);
    test_one<linestring, polygon>("simplex_horizontal32", simplex_horizontal, join_round32, end_round32, 5.12145, 1);

    test_one<linestring, polygon>("simplex_asym_neg", simplex, join_miter, end_flat, 3.2016, +1.5, settings, -1.0);
    test_one<linestring, polygon>("simplex_asym_pos", simplex, join_miter, end_flat, 3.2016, -1.0, settings, +1.5);
    // Do not work yet:
    //    test_one<linestring, polygon>("simplex_asym_neg", simplex, join_miter, end_round, 3.202, +1.5, settings, -1.0);
    //    test_one<linestring, polygon>("simplex_asym_pos", simplex, join_miter, end_round, 3.202, -1.0, settings, +1.5);

    // Generates (initially) a reversed polygon, with a negative area, which is reversed afterwards in assign_parents
    test_one<linestring, polygon>("simplex_asym_neg_rev", simplex, join_miter, end_flat, 3.2016, +1.0, settings, -1.5);
    test_one<linestring, polygon>("simplex_asym_pos_rev", simplex, join_miter, end_flat, 3.2016, -1.5, settings, +1.0);

    test_one<linestring, polygon>("straight", straight, join_round, end_flat, 38.4187, 1.5);
    test_one<linestring, polygon>("straight", straight, join_miter, end_flat, 38.4187, 1.5);

    // One bend/two bends (tests join-type)
    test_one<linestring, polygon>("one_bend", one_bend, join_round, end_flat, 28.496, 1.5);
    test_one<linestring, polygon>("one_bend", one_bend, join_miter, end_flat, 28.696, 1.5);
    test_one<linestring, polygon>("one_bend", one_bend, join_round_by_divide, end_flat, 28.497, 1.5);

    test_one<linestring, polygon>("one_bend", one_bend, join_round, end_round, 35.5603, 1.5);
    test_one<linestring, polygon>("one_bend", one_bend, join_miter, end_round, 35.7601, 1.5);

    test_one<linestring, polygon>("two_bends", two_bends, join_round, end_round, 46.2995, 1.5);
    test_one<linestring, polygon>("two_bends", two_bends, join_round, end_flat, 39.235, 1.5);
    test_one<linestring, polygon>("two_bends", two_bends, join_round_by_divide, end_flat, 39.235, 1.5);
    test_one<linestring, polygon>("two_bends", two_bends, join_miter, end_flat, 39.513, 1.5);
    test_one<linestring, polygon>("two_bends_left", two_bends, join_round, end_flat, 20.025, 1.5, settings, 0.0);
    test_one<linestring, polygon>("two_bends_left", two_bends, join_miter, end_flat, 20.225, 1.5, settings, 0.0);
    test_one<linestring, polygon>("two_bends_right", two_bends, join_round, end_flat, 19.211, 0.0, settings, 1.5);
    test_one<linestring, polygon>("two_bends_right", two_bends, join_miter, end_flat, 19.288, 0.0, settings, 1.5);

    test_one<linestring, polygon>("bend_near_start1", bend_near_start1, join_round, end_flat, 109.2625, 9.0);
    test_one<linestring, polygon>("bend_near_start2", bend_near_start2, join_round, end_flat, 142.8709, 9.0);

    // Next (and all similar cases) which a offsetted-one-sided buffer has to be supported. TODO
    //test_one<linestring, polygon>("two_bends_neg", two_bends, join_miter, end_flat, 99, +1.5, settings, -1.0);
    //test_one<linestring, polygon>("two_bends_pos", two_bends, join_miter, end_flat, 99, -1.5, settings, +1.0);
    //test_one<linestring, polygon>("two_bends_neg", two_bends, join_round, end_flat,99, +1.5, settings, -1.0);
    //test_one<linestring, polygon>("two_bends_pos", two_bends, join_round, end_flat, 99, -1.5, settings, +1.0);

    test_one<linestring, polygon>("overlapping_50", overlapping, join_round, end_flat, 24.6326, 0.5);
    test_one<linestring, polygon>("overlapping_50", overlapping, join_miter, end_flat, 24.9147, 0.5);
    test_one<linestring, polygon>("overlapping_150", overlapping, join_round, end_flat, 65.6786, 1.5);
    test_one<linestring, polygon>("overlapping_150", overlapping, join_miter, end_flat, 68.140, 1.5);

    test_one<linestring, polygon>("overlapping_rev_50", overlapping_rev, join_round, end_flat, 24.6326, 0.5);
    test_one<linestring, polygon>("overlapping_rev_50", overlapping_rev, join_miter, end_flat, 24.9147, 0.5);
    test_one<linestring, polygon>("overlapping_rev_150", overlapping_rev, join_round, end_flat, 65.6786, 1.5);
    test_one<linestring, polygon>("overlapping_rev_150", overlapping_rev, join_miter, end_flat, 68.140, 1.5);

    // Different cases with intersection points on flat and (left/right from line itself)
    test_one<linestring, polygon>("overlapping_asym_150_025", overlapping, join_round, end_flat, 48.308, 1.5, settings, 0.25);
    test_one<linestring, polygon>("overlapping_asym_150_025", overlapping, join_miter, end_flat, 50.770, 1.5, settings, 0.25);
    test_one<linestring, polygon>("overlapping_asym_150_075", overlapping, join_round, end_flat, 58.506, 1.5, settings, 0.75);
    test_one<linestring, polygon>("overlapping_asym_150_075", overlapping, join_miter, end_flat, 60.985, 1.5, settings, 0.75);
    test_one<linestring, polygon>("overlapping_asym_150_100", overlapping, join_round, end_flat, 62.514, 1.5, settings, 1.0);
    test_one<linestring, polygon>("overlapping_asym_150_100", overlapping, join_miter, end_flat, 64.984, 1.5, settings, 1.0);

    // The reverse cases need to reverse the distance too, for the same result
    test_one<linestring, polygon>("overlapping_rev_asym_150_025", overlapping_rev, join_round, end_flat, 48.308, 0.25, settings, 1.5);
    test_one<linestring, polygon>("overlapping_rev_asym_150_025", overlapping_rev, join_miter, end_flat, 50.770, 0.25, settings, 1.5);
    test_one<linestring, polygon>("overlapping_rev_asym_150_075", overlapping_rev, join_round, end_flat, 58.506, 0.75, settings, 1.5);
    test_one<linestring, polygon>("overlapping_rev_asym_150_075", overlapping_rev, join_miter, end_flat, 60.985, 0.75, settings, 1.5);
    test_one<linestring, polygon>("overlapping_rev_asym_150_100", overlapping_rev, join_round, end_flat, 62.514, 1.0, settings, 1.5);
    test_one<linestring, polygon>("overlapping_rev_asym_150_100", overlapping_rev, join_miter, end_flat, 64.984, 1.0, settings, 1.5);

    // Having flat end
    test_one<linestring, polygon>("for_collinear", for_collinear, join_round, end_flat, 68.561, 2.0);
    test_one<linestring, polygon>("for_collinear", for_collinear, join_miter, end_flat, 72, 2.0);
    test_one<linestring, polygon>("for_collinear2", for_collinear2, join_round, end_flat, 74.387, 2.0);
    test_one<linestring, polygon>("for_collinear2", for_collinear2, join_miter, end_flat, 78.0, 2.0);

    test_one<linestring, polygon>("curve", curve, join_round, end_flat, 58.1944, 5.0, settings, 3.0);
    test_one<linestring, polygon>("curve", curve, join_miter, end_flat, 58.7371, 5.0, settings, 3.0);

    test_one<linestring, polygon>("tripod", tripod, join_miter, end_flat, 74.25, 3.0);
    test_one<linestring, polygon>("tripod", tripod, join_miter, end_round, 116.6336, 3.0);

    test_one<linestring, polygon>("chained2", chained2, join_round, end_flat, 11.3137, 2.5, settings, 1.5);
    test_one<linestring, polygon>("chained3", chained3, join_round, end_flat, 16.9706, 2.5, settings, 1.5);
    test_one<linestring, polygon>("chained4", chained4, join_round, end_flat, 22.6274, 2.5, settings, 1.5);

    test_one<linestring, polygon>("field_sprayer1", field_sprayer1, join_round, end_flat, 324.3550, 16.5, settings, 6.5);
    test_one<linestring, polygon>("field_sprayer1", field_sprayer1, join_round, end_round, {718.686, 718.762}, 16.5, settings, 6.5);
    test_one<linestring, polygon>("field_sprayer1", field_sprayer1, join_miter, end_round, {718.845, 718.940}, 16.5, settings, 6.5);

    test_one<linestring, polygon>("degenerate0", degenerate0, join_round, end_round, 0.0, 3.0);
    test_one<linestring, polygon>("degenerate1", degenerate1, join_round, end_round, 28.25, 3.0);
    test_one<linestring, polygon>("degenerate2", degenerate2, join_round, end_round, 28.2503, 3.0);
    test_one<linestring, polygon>("degenerate3", degenerate3, join_round, end_round, 28.2503, 3.0);
    test_one<linestring, polygon>("degenerate4", degenerate4, join_round, end_round, 36.7410, 3.0);
    test_one<linestring, polygon>("degenerate4", degenerate4, join_round, end_flat, 8.4853, 3.0);

    {
        // These tests do test behaviour in end_round strategy:
        // -> it should generate closed pieces, also for an odd number of points.
        // It also tests behaviour in join_round strategy:
        // -> it should generate e.g. 4 points for a full circle,
        //    so a quarter circle does not get points in between
        using bg::strategy::buffer::join_round;
        using bg::strategy::buffer::end_round;

        double const d10 = 1.0;

        test_one<linestring, polygon>("mysql_report_2015_03_02a_3", mysql_report_2015_03_02a, join_round(3), end_round(3), 38.000, d10);
        test_one<linestring, polygon>("mysql_report_2015_03_02a_4", mysql_report_2015_03_02a, join_round(4), end_round(4), 38.000, d10);
        test_one<linestring, polygon>("mysql_report_2015_03_02a_5", mysql_report_2015_03_02a, join_round(5), end_round(5), 38.790, d10);
        test_one<linestring, polygon>("mysql_report_2015_03_02a_6", mysql_report_2015_03_02a, join_round(6), end_round(6), 38.817, d10);
        test_one<linestring, polygon>("mysql_report_2015_03_02a_7", mysql_report_2015_03_02a, join_round(7), end_round(7), 38.851, d10);
        test_one<linestring, polygon>("mysql_report_2015_03_02b_3", mysql_report_2015_03_02b, join_round(3), end_round(3), 36.500, d10);
        test_one<linestring, polygon>("mysql_report_2015_03_02b_4", mysql_report_2015_03_02b, join_round(4), end_round(4), 36.500, d10);
        test_one<linestring, polygon>("mysql_report_2015_03_02b_5", mysql_report_2015_03_02b, join_round(5), end_round(5), 37.346, d10);
        test_one<linestring, polygon>("mysql_report_2015_03_02b_6", mysql_report_2015_03_02b, join_round(6), end_round(6), 37.402, d10);
        test_one<linestring, polygon>("mysql_report_2015_03_02b_7", mysql_report_2015_03_02b, join_round(7), end_round(7), 37.506, d10);
        test_one<linestring, polygon>("mysql_report_2015_03_02c_3", mysql_report_2015_03_02c, join_round(2), end_round(3), 32.500, d10);
        test_one<linestring, polygon>("mysql_report_2015_03_02c_4", mysql_report_2015_03_02c, join_round(4), end_round(4), 32.500, d10);
        test_one<linestring, polygon>("mysql_report_2015_03_02c_5", mysql_report_2015_03_02c, join_round(5), end_round(5), 33.611, d10);
        test_one<linestring, polygon>("mysql_report_2015_03_02c_6", mysql_report_2015_03_02c, join_round(6), end_round(6), 33.719, d10);
        test_one<linestring, polygon>("mysql_report_2015_03_02c_7", mysql_report_2015_03_02c, join_round(7), end_round(7), 33.901, d10);

        // Testing the asymmetric end caps with odd number of points
        double const d15 = 1.5;
        test_one<linestring, polygon>("mysql_report_2015_03_02c_asym1", mysql_report_2015_03_02c, join_round(7), end_round(7), 39.714, d10, settings, d15);
        test_one<linestring, polygon>("mysql_report_2015_03_02c_asym2", mysql_report_2015_03_02c, join_round(7), end_round(7), 46.116, d15, settings, d10);

        double const d100 = 10;
        test_one<linestring, polygon>("mysql_report_2015_04_01", mysql_report_2015_04_01, join_round(32), end_round(32), 632.234, d100);
    }

    if (! BOOST_GEOMETRY_CONDITION((std::is_same<coor_type, float>::value)))
    {
        ut_settings settings;
        settings.tolerance = 0.1;
        settings.use_ln_area = true;

        test_one<linestring, polygon>("aimes120", aimes120, join_miter, end_flat, 1.62669948622351512e-08, 0.000018, settings);
        test_one<linestring, polygon>("aimes120", aimes120, join_round, end_round, 1.72842078427493107e-08, 0.000018, settings);

        test_one<linestring, polygon>("aimes167", aimes167, join_miter, end_flat, 1.88900628472765675e-09, 0.000018, settings);
        test_one<linestring, polygon>("aimes167", aimes167, join_round, end_round, 2.85734813587623648e-09, 0.000018, settings);

        test_one<linestring, polygon>("aimes175", aimes175, join_miter, end_flat, 2.81111809385947709e-08, 0.000036, settings);
        test_one<linestring, polygon>("aimes175", aimes175, join_round, end_round, 3.21215765097804251e-08, 0.000036, settings);

        test_one<linestring, polygon>("aimes171", aimes171, join_miter, end_flat, 1.1721873249825876e-08, 0.000018, settings);
        test_one<linestring, polygon>("aimes171", aimes171, join_round, end_round, 1.2739093335767393e-08, 0.000018, settings);
        test_one<linestring, polygon>("aimes171", aimes171, join_round_by_divide, end_round, 1.2739093335767393e-08, 0.000018, settings);

        test_one<linestring, polygon>("aimes181", aimes181, join_miter, end_flat, 2.1729405830228643e-08, 0.000036, settings);
        test_one<linestring, polygon>("aimes181", aimes181, join_round, end_round, 2.57415564419716247e-08, 0.000036, settings);
        test_one<linestring, polygon>("aimes181", aimes181, join_round_by_divide, end_round, 2.57415564419716247e-08, 0.000036, settings);
    }

    {
        // Expectations can also be 1702.56530051454502 2140.78725663358819
        // so we increase tolerance
        ut_settings settings(0.5);
        test_one<linestring, polygon>("crossing", crossing, join_round32, end_flat, 1702.1, 20.0, settings);
        test_one<linestring, polygon>("crossing", crossing, join_round32, end_round32, 2140.4, 20.0, settings);
    }

    test_one<linestring, polygon>("mikado1", mikado1, join_round32, end_round32, 5441135039.0979, 41751.0);

    {
        // This case gave a spike (invalid result) up to 1.70
        // Fixed by general form for line/line intersection in miter calculation
        using bg::strategy::buffer::join_round;
        using bg::strategy::buffer::end_round;
        test_one<linestring, polygon>("issue_596", issue_596, join_round(12), end_round(12), 0.12462779, 0.015);
        test_one<linestring, polygon>("issue_596", issue_596, join_miter, end_round(12), 0.12462807, 0.015);
    }

    {
        using bg::strategy::buffer::join_round;
        using bg::strategy::buffer::end_round;
        test_one<linestring, polygon>("issue_803", issue_803, join_miter, end_round(36), 1664.3714, 10.0);
        test_one<linestring, polygon>("issue_803", issue_803, join_round(36), end_round(36), 1664.0528, 10.0);
    }

    {
        using bg::strategy::buffer::join_round;
        using bg::strategy::buffer::end_round;
        test_one<linestring, polygon>("issue_988_a", issue_988, join_round(8), end_round(8), 0.0029235, 0.010068);
        test_one<linestring, polygon>("issue_988_b", issue_988, join_round(32), end_round(32), 0.0029614, 0.0101);
        test_one<linestring, polygon>("issue_988_c", issue_988, join_round(32), end_round(32), 0.0031514, 0.011);
    }

    {
        using bg::strategy::buffer::join_round;
        using bg::strategy::buffer::end_round;
        test_one<linestring, polygon>("issue_1084", issue_1084, join_round(10), end_round(10), 13200.83, 10.0);
    }
    {
        using bg::strategy::buffer::join_miter;
        using bg::strategy::buffer::end_flat;
        test_one<linestring, polygon>("issue_1250", issue_1250, join_miter(5), end_flat(), 8.39277, 0.5);
        test_one<linestring, polygon>("issue_1250_rev", issue_1250_rev, join_miter(5), end_flat(), 8.39277, 0.5);
    }

    test_one<linestring, polygon>("mysql_report_2015_06_11",
            mysql_report_2015_06_11, join_round32, end_round32,
            27862.733459829971,
            5.9518403867035365);

    if (BOOST_GEOMETRY_CONDITION((std::is_same<coor_type, double>::value)))
    {
        test_one<linestring, polygon>("mysql_report_2015_09_08a", mysql_report_2015_09_08a, join_round32, end_round32, 0.0, 1.0);
        test_one<linestring, polygon>("mysql_report_2015_09_08b", mysql_report_2015_09_08b, join_round32, end_round32, 0.0, 1099511627778.0);
        test_one<linestring, polygon>("mysql_report_2015_09_08c", mysql_report_2015_09_08c, join_round32, end_round32, 0.0, 0xbe);
    }

    test_one<linestring, polygon>("mysql_23023665_1", mysql_23023665, join_round32, end_flat, 459.1051, 10);
    test_one<linestring, polygon>("mysql_23023665_2", mysql_23023665, join_round32, end_flat, 6877.7097, 50);
    test_one<linestring, polygon>("mysql_25662426", mysql_25662426, join_round32, end_round32, 1, 0, 1660.6673, 10);


    // Test behaviour with different buffer sizes, generating internally turns on different locations
    test_one<linestring, polygon>("mysql_25662426a_05", mysql_25662426a, join_round32, end_round32, 27.6156, 0.5);
    test_one<linestring, polygon>("mysql_25662426a_1", mysql_25662426a, join_round32, end_round32, 54.9018, 1.0);
    test_one<linestring, polygon>("mysql_25662426a_2", mysql_25662426a, join_round32, end_round32, 103.6072, 2.0);
    test_one<linestring, polygon>("mysql_25662426a_3", mysql_25662426a, join_round32, end_round32, 152.1163, 3.0);
    test_one<linestring, polygon>("mysql_25662426a_4", mysql_25662426a, join_round32, end_round32, 206.4831, 4.0);
    test_one<linestring, polygon>("mysql_25662426a_5", mysql_25662426a, join_round32, end_round32, 266.8505, 5.0);
    test_one<linestring, polygon>("mysql_25662426a_10", mysql_25662426a, join_round32, end_round32, 660.7355, 10.0);
    test_one<linestring, polygon>("mysql_25662426a_05", mysql_25662426a, join_round32, end_flat, 26.8352, 0.5);
    test_one<linestring, polygon>("mysql_25662426a_1", mysql_25662426a, join_round32, end_flat, 53.3411, 1.0);
    test_one<linestring, polygon>("mysql_25662426a_2", mysql_25662426a, join_round32, end_flat, 97.3644, 2.0);
    test_one<linestring, polygon>("mysql_25662426a_3", mysql_25662426a, join_round32, end_flat, 138.0697, 3.0);
    test_one<linestring, polygon>("mysql_25662426a_4", mysql_25662426a, join_round32, end_flat, 181.5115, 4.0);
    test_one<linestring, polygon>("mysql_25662426a_5", mysql_25662426a, join_round32, end_flat, 227.8325, 5.0);
    test_one<linestring, polygon>("mysql_25662426a_10", mysql_25662426a, join_round32, end_flat, 534.1083, 10.0, specific_settings);

    // Asymmetric buffers
    // Mostly left
    test_one<linestring, polygon>("mysql_25662426a_mostly_left_05", mysql_25662426a, join_round32, end_round32, 15.6638, 0.5, settings, 0.05);
    test_one<linestring, polygon>("mysql_25662426a_mostly_left_1", mysql_25662426a, join_round32, end_round32, 32.9553, 1.0, settings, 0.1);
    test_one<linestring, polygon>("mysql_25662426a_mostly_left_2", mysql_25662426a, join_round32, end_round32, 72.1159, 2.0, settings, 0.2);
    test_one<linestring, polygon>("mysql_25662426a_mostly_left_3", mysql_25662426a, join_round32, end_round32, 116.3802, 3.0, settings, 0.3);
    test_one<linestring, polygon>("mysql_25662426a_mostly_left_4", mysql_25662426a, join_round32, end_round32, 165.9298, 4.0, specific_settings, 0.4);
    test_one<linestring, polygon>("mysql_25662426a_mostly_left_5", mysql_25662426a, join_round32, end_round32, 220.8054, 5.0, specific_settings, 0.5);
    test_one<linestring, polygon>("mysql_25662426a_mostly_left_10", mysql_25662426a, join_round32, end_round32, 577.3742, 10.0, specific_settings, 1.0);

    // Mostly right
    test_one<linestring, polygon>("mysql_25662426a_mostly_right_05", mysql_25662426a, join_round32, end_round32, 14.3419, 0.05, settings, 0.5);
    test_one<linestring, polygon>("mysql_25662426a_mostly_right_1", mysql_25662426a, join_round32, end_round32, 27.1955, 0.1, settings, 1.0);
    test_one<linestring, polygon>("mysql_25662426a_mostly_right_2", mysql_25662426a, join_round32, end_round32, 43.1821, 0.2, settings, 2.0);
    test_one<linestring, polygon>("mysql_25662426a_mostly_right_3", mysql_25662426a, join_round32, end_round32, 54.4337, 0.3, settings, 3.0);
    test_one<linestring, polygon>("mysql_25662426a_mostly_right_4", mysql_25662426a, join_round32, end_round32, 75.6376, 0.4, settings, 4.0);
    test_one<linestring, polygon>("mysql_25662426a_mostly_right_5", mysql_25662426a, join_round32, end_round32, 102.6952, 0.5, settings, 5.0);
    test_one<linestring, polygon>("mysql_25662426a_mostly_right_10", mysql_25662426a, join_round32, end_round32, 260.1850, 1.0, settings, 10.0);

    // Left
    test_one<linestring, polygon>("mysql_25662426a_left_05", mysql_25662426a, join_round32, end_round32, 14.2803, 0.5, settings, 0.0);
    test_one<linestring, polygon>("mysql_25662426a_left_1", mysql_25662426a, join_round32, end_round32, 30.1214, 1.0, settings, 0.0);
    test_one<linestring, polygon>("mysql_25662426a_left_2", mysql_25662426a, join_round32, end_round32, 66.4858, 2.0, ut_settings(0.01, false), 0.0); // It has a self touching point
    test_one<linestring, polygon>("mysql_25662426a_left_3", mysql_25662426a, join_round32, end_round32, 108.3305, 3.0, settings, 0.0);
    test_one<linestring, polygon>("mysql_25662426a_left_4", mysql_25662426a, join_round32, end_round32, 155.5128, 4.0, specific_settings, 0.0);
    test_one<linestring, polygon>("mysql_25662426a_left_5", mysql_25662426a, join_round32, end_round32, 208.1289, 5.0, specific_settings, 0.0);
    test_one<linestring, polygon>("mysql_25662426a_left_10", mysql_25662426a, join_round32, end_round32, 554.8818, 10.0, specific_settings, 0.0);

    // Right
    test_one<linestring, polygon>("mysql_25662426a_right_05", mysql_25662426a, join_round32, end_round32, 12.9451, 0.0, settings, 0.5);
    test_one<linestring, polygon>("mysql_25662426a_right_1", mysql_25662426a, join_round32, end_round32, 24.3902, 0.0, settings, 1.0);
    test_one<linestring, polygon>("mysql_25662426a_right_2", mysql_25662426a, join_round32, end_round32, 37.5607, 0.0, settings, 2.0);
    test_one<linestring, polygon>("mysql_25662426a_right_3", mysql_25662426a, join_round32, end_round32, 46.2741, 0.0, settings, 3.0);
    test_one<linestring, polygon>("mysql_25662426a_right_4", mysql_25662426a, join_round32, end_round32, 70.2429, 0.0, settings, 4.0);
    test_one<linestring, polygon>("mysql_25662426a_right_5", mysql_25662426a, join_round32, end_round32, 95.7545, 0.0, settings, 5.0);
    test_one<linestring, polygon>("mysql_25662426a_right_10", mysql_25662426a, join_round32, end_round32, 244.5337, 0.0, settings, 10.0);

    // Flat end for one-sided buffer
    test_one<linestring, polygon>("mysql_25662426a_left_05", mysql_25662426a, join_round32, end_flat, 14.0853, 0.5, settings, 0.0);
    test_one<linestring, polygon>("mysql_25662426a_right_05", mysql_25662426a, join_round32, end_flat, 12.7500, 0.0, settings, 0.5);
    test_one<linestring, polygon>("mysql_25662426a_left_25", mysql_25662426a, join_round32, end_flat, 82.1318, 2.5, settings, 0.0);
    test_one<linestring, polygon>("mysql_25662426a_right_25", mysql_25662426a, join_round32, end_flat, 39.0000, 0.0, settings, 2.5);
}

template <bool Clockwise, typename P>
void test_invalid()
{
    typedef typename bg::coordinate_type<P>::type coor_type;
    if (! BOOST_GEOMETRY_CONDITION((std::is_same<coor_type, double>::value)))
    {
        return;
    }
    typedef bg::model::linestring<P> linestring;
    typedef bg::model::polygon<P, Clockwise> polygon;

    bg::strategy::buffer::end_round end_round32(32);
    bg::strategy::buffer::join_round join_round32(32);

    // Linestring contains extreme differences causing numeric errors. Empty geometry is returned
    test_one<linestring, polygon>("mysql_report_2015_04_10a", mysql_report_2015_04_10a, join_round32, end_round32, 0.0, 100.0);
    test_one<linestring, polygon>("mysql_report_2015_04_10b", mysql_report_2015_04_10b, join_round32, end_round32, 0.0, 100.0);
    test_one<linestring, polygon>("mysql_report_2015_04_10c", mysql_report_2015_04_10c, join_round32, end_round32, 0.0, 100.0);
    test_one<linestring, polygon>("mysql_report_2015_04_10d", mysql_report_2015_04_10d, join_round32, end_round32, 0.0, 100.0);
    test_one<linestring, polygon>("mysql_report_2015_04_10e", mysql_report_2015_04_10e, join_round32, end_round32, 0.0, 100.0);
    test_one<linestring, polygon>("mysql_report_2015_04_10f", mysql_report_2015_04_10f, join_round32, end_round32, 0.0, 100.0);

    // The equivalent, valid, case
    test_one<linestring, polygon>("mysql_report_2015_04_10g", mysql_report_2015_04_10g, join_round32, end_round32, 86527.871, 100.0);
}


int test_main(int, char* [])
{
    BoostGeometryWriteTestConfiguration();

    test_all<true, bg::model::point<default_test_type, 2, bg::cs::cartesian> >();

#if ! defined(BOOST_GEOMETRY_TEST_ONLY_ONE_ORDER)
    test_all<false, bg::model::point<default_test_type, 2, bg::cs::cartesian> >();
#endif

    test_invalid<true, bg::model::point<default_test_type, 2, bg::cs::cartesian> >();

#if ! defined(BOOST_GEOMETRY_TEST_ONLY_ONE_TYPE)
    test_invalid<true, bg::model::point<long double, 2, bg::cs::cartesian> >();
#endif

    return 0;
}
