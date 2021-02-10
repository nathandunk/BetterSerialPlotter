#include <BetterSerialPlotter/Utility.hpp>
#include <Mahi/Gui.hpp>
#include <Windows.h>
#include <iostream>

namespace bsp{

std::vector<int> get_serial_ports() //added function to find the present serial 
{
    char lpTargetPath[5000]; // buffer to store the path of the COMPORTS
    // bool gotPort = false; // in case the port is not found
    std::vector<int> port_nums;

    for (int i = 0; i < 255; i++) // checking ports from COM0 to COM255
    {
        std::string str = "COM" + std::to_string(i); // converting to COM0, COM1, COM2
        DWORD test = QueryDosDevice(str.c_str(), lpTargetPath, 5000);

        // Test the return value and error if any
        if (test != 0) //QueryDosDevice returns zero if it didn't find an object
        {
            port_nums.push_back(i);
            // std::cout << str << ": " << lpTargetPath << std::endl;
            // gotPort = true;
        }

        if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {
        }
    }

    return port_nums;
}

} // namespace bsp