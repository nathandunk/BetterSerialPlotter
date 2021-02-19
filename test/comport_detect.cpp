#include <iostream>
#include <string>
#include <Windows.h>
#include <BetterSerialPlotter/Utility.hpp>

using namespace bsp;

int main(){
    std::vector<std::shared_ptr<ScrollingData>> my_data;

    for (size_t i = 0; i < 5; i++){
        my_data.emplace_back(std::make_shared<ScrollingData>());
        my_data[i]->set_name("test" + std::to_string(i));
    }

    for (size_t i = 0; i < 5; i++){
        std::cout << my_data[i]->name << "\n";
    }

}