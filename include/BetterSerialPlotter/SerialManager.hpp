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
    void render();
    bool begin_serial();
    void close_serial();
    void reset_read();
    void read_serial();
    SerialManager(BSP* gui_);
    SerialManager();
    SerialManager(const SerialManager& serial_manager);
    SerialManager& operator=(const SerialManager& serial_manager);
    std::vector<float> curr_data;
    std::string curr_number_buff;
    std::string curr_line_buff;
    bool read_once = false;
    static constexpr int packet_size = 1024;
    unsigned char message[packet_size];
    int baud_rate = 115200;
    std::atomic<bool> baud_status = false;
    std::atomic<bool> serial_status = false;
    int cycles_waited = 0;
    int cycle_timeout = 5000;
    std::atomic<bool> serial_started = false;
    mahi::com::SerialPort serial_port;
    std::mutex mtx;

    std::string get_port_name(BspPort port_num);
    bool comport_valid();

#ifdef __APPLE__
    BspPort comport_num;
#else
    BspPort comport_num = -1;
#endif

private:
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