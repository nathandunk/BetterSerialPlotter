#pragma once

#include <BetterSerialPlotter/Widget.hpp>
#include <Windows.h>

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
    HANDLE hSerial;
    int comport_num = -1;
    std::vector<float> curr_data;
    std::string curr_number_buff;
    std::string curr_line_buff;
    bool read_once = false;
    static constexpr int packet_size = 1024;
    char message[packet_size];
    int baud_rate = 9600;
    bool baud_status = false;
    bool serial_status = false;
    int cycles_waited = 0;
    int cycle_timeout = 20;
    bool serial_started = false;
private:
    std::vector<int> baud_rates = {CBR_110,
                                   CBR_300,
                                   CBR_600,
                                   CBR_1200,
                                   CBR_2400,
                                   CBR_4800,
                                   CBR_9600,
                                   CBR_14400,
                                   CBR_19200,
                                   CBR_38400,
                                   CBR_56000,
                                   CBR_57600,
                                   CBR_115200,
                                   CBR_128000,
                                   CBR_256000};
};

} // namespace bsp