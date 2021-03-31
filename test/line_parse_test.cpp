#include "gtest/gtest.h"
#include "BetterSerialPlotter/SerialManager.hpp"

#define line_parse_test(num,name) \
    TEST(line_parsing, name){\
        auto str_num = strings[num];\
        auto num_num = numbers[num];\
        auto parse_result = sm.parse_line(str_num);\
        EXPECT_EQ(parse_result[0], num_num);\
    }

bsp::SerialManager sm;

std::vector<std::string> strings = {"1.23e+16", // positive exponential
                                    "-0.24e-12",// negative exponential
                                    "0",        // 0
                                    "0.0",      // 0 decimal
                                    "-0",       // -0
                                    "-0.0",     // -0 decimal
                                    "1234.15",  // positive number
                                    "-1234.15", // negative number
                                    "+1234.15"};// plus preceded number

std::vector<float>       numbers = {1.23e16f,   // positive exponential
                                    -0.24e-12f, // negative exponential
                                    0.0f,       // 0
                                    0.0f,       // 0 decimal
                                    -0.0f,      // -0 
                                    -0.0f,      // -0 decimal
                                    1234.15f,   // positive number
                                    -1234.15f,  // negative number
                                    1234.15f}; // plus-preceded number

line_parse_test(0,positive_exponent);

line_parse_test(1,negative_exponent);

line_parse_test(2,zero);

line_parse_test(3,zero_decimal);

line_parse_test(4,neagitve_zero);

line_parse_test(5,negative_zero_decimal);

line_parse_test(6,positive_number);

line_parse_test(7,negative_number);

line_parse_test(8,plus_preceded_number);


TEST(line_parsing, tab_delimited){
    std::string test_string;
    for (auto i = 0; i < strings.size(); i++){
        test_string += strings[i];
        if (i != test_string.size()-1) test_string += "\t";
    }

    auto parse_result = sm.parse_line(test_string);
    EXPECT_EQ(parse_result,numbers);
}

TEST(line_parsing, space_delimited){
    std::string test_string;
    for (auto i = 0; i < strings.size(); i++){
        test_string += strings[i];
        if (i != test_string.size()-1) test_string += " ";
    }

    auto parse_result = sm.parse_line(test_string);
    EXPECT_EQ(parse_result,numbers);
}

TEST(line_parsing, both_delimited){
    std::string test_string;
    for (auto i = 0; i < strings.size(); i++){
        test_string += strings[i];
        if (i != test_string.size()-1) test_string += (i%2) ? "\t" : " ";
    }

    auto parse_result = sm.parse_line(test_string);
    EXPECT_EQ(parse_result,numbers);
}

TEST(line_parsing, combined_numbers){
    std::string combined_numbers = "0.10.2";

    auto parse_result = sm.parse_line(combined_numbers);
    EXPECT_EQ(parse_result.size(),0);
}

TEST(line_parsing, one_combined_number){
    std::string combined_numbers = "1.23 0.10.2 1.3e7";
    std::vector<float> float_result = {1.23f, 1.3e7f};

    auto parse_result = sm.parse_line(combined_numbers);
    EXPECT_EQ(parse_result, float_result);
}

TEST(line_parsing, extra_spaces){
    std::string combined_numbers = "  1.23 1.3e7   ";
    std::vector<float> float_result = {1.23f, 1.3e7f};

    auto parse_result = sm.parse_line(combined_numbers);
    EXPECT_EQ(parse_result, float_result);
}