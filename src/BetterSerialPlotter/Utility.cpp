#include <BetterSerialPlotter/Utility.hpp>
#include <Mahi/Gui.hpp>
#include <iostream>

#if defined(WIN32)
#include <Windows.h>
#else
    #include <filesystem>
    namespace fs = std::filesystem;
#endif

namespace bsp{
#if defined(WIN32)
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
            port_nums.push_back(i-1);
            // std::cout << str << ": " << lpTargetPath << std::endl;
            // gotPort = true;
        }

        if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {
        }
    }

    return port_nums;
}
#elif defined(__linux__)
std::vector<int> get_serial_ports() //added function to find the present serial 
{
    std::vector<int> port_nums;
    std::string usb_string = "ttyUSB";
    std::string acm_string = "ttyACM";

    std::string path = "/sys/class/tty";
    for (const auto & entry : fs::directory_iterator(path)){
        if (entry.path().filename().generic_string().find(usb_string) != std::string::npos) {
            std::string fp = entry.path().filename().generic_string();
            fp.erase(fp.begin(),fp.begin()+6);
            port_nums.push_back(std::stoi(fp)+16); // see SerialPort.hpp in mahi-com for enum which makes this + 16
        }
        if (entry.path().filename().generic_string().find(acm_string) != std::string::npos) {
            std::string fp = entry.path().filename().generic_string();
            fp.erase(fp.begin(),fp.begin()+6);
            port_nums.push_back(std::stoi(fp)+24); // see SerialPort.hpp in mahi-com for enum which makes this + 24
        }
    }
        
    return port_nums;
}
#endif

} // namespace bsp