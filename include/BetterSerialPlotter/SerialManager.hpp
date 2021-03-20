#pragma once

#include <BetterSerialPlotter/Widget.hpp>
#include <Mahi/Com/SerialPort.hpp>

namespace bsp{

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
    int comport_num = -1;
    std::vector<float> curr_data;
    std::string curr_number_buff;
    std::string curr_line_buff;
    bool read_once = false;
    static constexpr int packet_size = 1024;
    unsigned char message[packet_size];
    int baud_rate = 115200;
    bool baud_status = false;
    bool serial_status = false;
    int cycles_waited = 0;
    int cycle_timeout = 50;
    bool serial_started = false;
    mahi::com::SerialPort serial_port;

    std::string get_port_name(int port_num);

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