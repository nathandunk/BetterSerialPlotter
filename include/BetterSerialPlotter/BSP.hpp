#pragma once

#include <Mahi/Gui.hpp>
#include <Windows.h>
#include <BetterSerialPlotter/Utility.hpp>
#include <Mahi/Util/Templates/RingBuffer.hpp>
#include <Mahi/Util/Timing/Clock.hpp>

namespace bsp{

class BSP : public mahi::gui::Application
{
private:
    static constexpr int packet_size = 32;
    bool open = true;
    int comport_num = 12;
    char message[packet_size];
    mahi::util::RingBuffer<std::string> PrintBuffer;
    mahi::util::Clock program_clock;
    std::string curr_number_buff;
    bool read_once = false;
    bool verbose = false;
    bool serial_started = false;
    int num_plots = 1;
    float time = 0;

    // all_data[0] contains the full vector for variable 0
    // all_data[1] contains the full vector for variable 1
    // ...etc
    std::vector<ScrollingData> all_data;

    std::vector<float> curr_data;
    HANDLE hSerial;

    void add_plot(int plot_num);
public:
    BSP(/* args */);
    ~BSP();
    void update();
    void begin_serial();
    void read_serial();
    void append_all_data(std::vector<float> curr_data);
};

} // namespace bsp