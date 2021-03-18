#include <iostream>
#include <string>
#include <Windows.h>
#include <Mahi/Com.hpp>
#include <Mahi/Util.hpp>
#include <BetterSerialPlotter/BSP.hpp>
#include <BetterSerialPlotter/Serialization.hpp>
#include <fstream>

using namespace mahi::util;
using namespace bsp;

int main(){
    BSP bsp;
    bsp.serial_manager.comport_num = 3;
    auto bsp_data = BSPData(&bsp);
    print_var(bsp_data.serial_manager.comport_num);
}