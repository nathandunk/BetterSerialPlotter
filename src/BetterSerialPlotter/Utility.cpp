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
#elif defined(__APPLE__)
std::vector<std::string> get_serial_ports() //added function to find the present serial 
{
    std::vector<std::string> port_nums;
    std::string usb_string = "tty.";

    std::string path = "/dev/";
    for (const auto & entry : fs::directory_iterator(path)){
        if (entry.path().filename().generic_string().find(usb_string) != std::string::npos) {
            std::string fp = entry.path().filename().generic_string();
            port_nums.push_back(fp);
        }
    }
        
    return port_nums;
}
#else
std::vector<int> get_serial_ports() //added function to find the present serial 
{
// char *comports[RS232_PORTNR]={"/dev/ttyS0","/dev/ttyS1","/dev/ttyS2","/dev/ttyS3","/dev/ttyS4","/dev/ttyS5",
//                        "/dev/ttyS6","/dev/ttyS7","/dev/ttyS8","/dev/ttyS9","/dev/ttyS10","/dev/ttyS11",
//                        "/dev/ttyS12","/dev/ttyS13","/dev/ttyS14","/dev/ttyS15","/dev/ttyUSB0",
//                        "/dev/ttyUSB1","/dev/ttyUSB2","/dev/ttyUSB3","/dev/ttyUSB4","/dev/ttyUSB5",
//                        "/dev/ttyAMA0","/dev/ttyAMA1","/dev/ttyACM0","/dev/ttyACM1",
//                        "/dev/rfcomm0","/dev/rfcomm1","/dev/ircomm0","/dev/ircomm1",
//                        "/dev/cuau0","/dev/cuau1","/dev/cuau2","/dev/cuau3",
//                        "/dev/cuaU0","/dev/cuaU1","/dev/cuaU2","/dev/cuaU3"};

    // these are for searching for each respective type of linux serial connection. The number corresponds
    // to the difference between the number of the type of comport (i.e. ttyUSB1 is 1) in the comports array above
    // and the array where it exists.

    static std::pair<std::string,int> usb_string("ttyUSB",16);
    static std::pair<std::string,int> acm_string("ttyACM",24);
    static std::pair<std::string,int> ama_string("ttyAMA",22);
    static std::pair<std::string,int> rfcomm_string("rfcomm",26);
    static std::pair<std::string,int> ircomm_string("ircomm",28);
    static std::pair<std::string,int> cuau_string("cuau",30);
    static std::pair<std::string,int> cuaU_string("cuaU",34);

    static std::vector<std::pair<std::string,int>> serial_pairs = {usb_string, acm_string, ama_string, rfcomm_string, 
                                                                    ircomm_string, cuau_string, cuaU_string};

    std::vector<int> port_nums;

    std::string path = "/sys/class/tty";
    for (const auto & serial_type : serial_pairs){
        for (const auto & entry : fs::directory_iterator(path)){
            if (entry.path().filename().generic_string().find(serial_type.first) != std::string::npos) {
                std::string fp = entry.path().filename().generic_string();
                fp.erase(fp.begin(),fp.begin()+serial_type.first.length());
                port_nums.push_back(std::stoi(fp)+serial_type.second); // see SerialPort.hpp in mahi-com for enum which makes this + 16
            }
        }
    }
        
    return port_nums;
}
#endif

} // namespace bsp