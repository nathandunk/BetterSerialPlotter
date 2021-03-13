#include <iostream>
#include <string>
#include <Windows.h>
#include <BetterSerialPlotter/Serialization.hpp>
#include <fstream>

using namespace bsp;

int main(){
    ImVec4 my_color(0,1,2,3);

    ScrollingData sd_test;
    sd_test.color = my_color;
    sd_test.name = "test_name";

    nlohmann::json j_out;
    j_out["sd"] = sd_test;

    std::string filepath = "test.json";

    std::ofstream ofile(filepath);
    
    ofile << j_out;

    ofile.close();

    std::ifstream ifile(filepath);

    nlohmann::json j_in;
    ifile >> j_in;
    auto my_sd = j_in["sd"].get<ScrollingData>(); 
    std::cout << my_sd.name;

    ifile.close();

}