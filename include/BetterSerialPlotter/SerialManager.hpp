#pragma once

#include <BetterSerialPlotter/Widget.hpp>
#include <Mahi/Com/SerialPort.hpp>
#include <mutex>

namespace bsp{

#ifdef __APPLE__
typedef std::string BspPort;
#else
typedef int BspPort;
#endif

class SerialManager : public Widget
{
public:

    // variables related to the current state of the serial port
    // int  comport_num    = -1;    // serial port number selected (this is related to enums from SerialPort class in mahi-com)
                                 // -1 indicates that there is not comport num selected yet
    int  baud_rate      = 9600;  // baud rate selected
    std::atomic<bool> serial_started = false; // indicates whether serial prot has been successfully opened
    std::atomic<bool> baud_status    = false; // indicates whether or not the serial port is consistently reading good data at this baud rate
    std::atomic<bool> serial_status  = false; // indicates that the program is able to receive any information from the serial port

    // variables related to the current state of data collection (either current number or current line)
    std::string        curr_line_buff;   // string representing the current analyzed line

    std::mutex mtx; // mutex so that data can be shared when in the serial_manager thread
    
    bool read_once    = false; // marks whether we have read through data at least once
    int cycles_waited = 0;     // number of cycles waited since a valid read, for timeouts
    int cycle_timeout = 50;    //  how many cycles to wait before showing a timeout

    
    static constexpr int packet_size = 1024;

    // abstracted serial port object for windows/mac/linux
    mahi::com::SerialPort serial_port;

    /// constructur
    SerialManager(BSP* gui_);
    /// default constructor for serialization
    SerialManager();
    /// copy constructor. this importantly creates a default SerialPort, 
    /// and only copies over comport_num and baud_rate
    SerialManager(const SerialManager& serial_manager);
    /// assignment operator. only copies over comport_num and baud_rate
    SerialManager& operator=(const SerialManager& serial_manager);

    /// render the appropriate imgui content for this portion of the gui
    void render();
    /// open a serial port with the comport_number of this class
    bool begin_serial();
    /// close a serial port if it is open, otherwise do nothing
    void close_serial();
    /// clear all data and buffers related to reading from the serial port
    void reset_read();
    /// attempt to read from the serial port
    void read_serial();
    /// just the abstracted read from serial which is platform dependent
    int receive_data(unsigned char * message, int packet_size);
    /// returns human readable port name (e.g. COM5 for windows or ttyUSB3 for Linux)
    std::string get_port_name(int port_num);
    /// parses a buffer received from a serial port read
    void parse_buffer(unsigned char* message, size_t buff_len);
    /// parses a single line received from the buffer
    std::vector<float> parse_line(std::string line);

    bool comport_valid();

#ifdef __APPLE__
    BspPort comport_num;
#else
    BspPort comport_num = -1;
#endif

private:
    // list of valid baudrates
    std::vector<int> baud_rates = {110,
                                   300,
                                   600,
                                   1200,
                                   2400,
                                   4800,
                                   9600,
                                   14400,
                                   19200,
                                   38400,
                                   56000,
                                   57600,
                                   115200,
                                   128000,
                                   256000};
    
    
};

} // namespace bsp