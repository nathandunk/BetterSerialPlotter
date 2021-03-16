#include <iostream>
#include <string>
#include <Windows.h>
#include <Mahi/Com.hpp>
#include <fstream>

using namespace mahi::com;

int main(){
    unsigned char message[1];
    mahi::com::SerialPort serial_port;
    serial_port.open((Port)11,115200);
    // serial_port.flush_TX();

    unsigned char recv[1];
    for (size_t i = 0; i < 32; i++)
    {
        serial_port.receive_data(recv, 1);
        for (std::size_t i = 0; i < 1; ++i)
            std::cout << recv[i] << std::endl;
    }

    // for (auto i = 0; i < 32; i++){
    //     int bits = 0;
    //     while(!bits){
    //         bits = serial_port.receive_data(message, 1);
    //     }
    //     std::cout << message[i];
    // }

    // for (size_t i = 0; i < 32; i++){
    //     std::cout << message[i] << std::endl;;
    // }
    
}