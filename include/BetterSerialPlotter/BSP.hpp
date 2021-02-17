#pragma once

#include <Mahi/Gui.hpp>
#include <Windows.h>
#include <BetterSerialPlotter/Plot.hpp>
#include <BetterSerialPlotter/Utility.hpp>
#include <Mahi/Util/Templates/RingBuffer.hpp>
#include <Mahi/Util/Timing/Clock.hpp>

namespace bsp{

class BSP : public mahi::gui::Application
{
private:
    static constexpr int packet_size = 32;
    bool open = true;
    int comport_num = -1;
    int baud_rate = 9600;
    bool baud_status = false;
    bool serial_status = false;
    char message[packet_size];
    mahi::util::RingBuffer<std::string> PrintBuffer;
    mahi::util::Clock program_clock;
    std::string curr_number_buff;
    std::string curr_line_buff;
    bool read_once = false;
    bool verbose = false;
    bool serial_started = false;
    int num_plots = 1;
    float time = 0;
    int cycles_waited = 0;
    int cycle_timeout = 20;

    // all_data[0] contains the full vector for variable 0
    // all_data[1] contains the full vector for variable 1
    // ...etc
    std::vector<ScrollingData> all_data;

    std::vector<Plot> all_plots;

    std::vector<float> curr_data;
    HANDLE hSerial;

    ImVec4 PalleteRed       = ImVec4(1.00f,0.40f,0.40f,1.0f);
    ImVec4 PalleteGreen     = ImVec4(0.30f,0.67f,0.34f,1.0f);
    ImVec4 PalleteBlueGray  = ImVec4(0.33f,0.35f,0.39f,1.0f);
    ImVec4 PalleteGray      = ImVec4(0.37f,0.37f,0.37f,1.0f);
    ImVec4 PalleteYellow    = ImVec4(0.98f,0.78f,0.27f,1.0f);
    ImVec4 PalleteOrange    = ImVec4(0.98f,0.43f,0.27f,1.0f);
    ImVec4 PalleteBlue      = ImVec4(0.24f,0.62f,0.71f,1.0f);
    // ImVec4 PalleteBlue      = ImGui::ColorConvertU32ToFloat4(0x08B2E3FF);

    // std::vector<ImVec4> plot_colors = {ImGui::ColorConvertU32ToFloat4(0xA85B6AFF),
    //                                    ImGui::ColorConvertU32ToFloat4(0x577690FF),
    //                                    ImGui::ColorConvertU32ToFloat4(0x90BE6DFF),
    //                                    ImGui::ColorConvertU32ToFloat4(0xF94144FF),
    //                                    ImGui::ColorConvertU32ToFloat4(0xF3722CFF),
    //                                    ImGui::ColorConvertU32ToFloat4(0xF8961EFF),
    //                                    ImGui::ColorConvertU32ToFloat4(0xF9C74FFF),
    //                                    ImGui::ColorConvertU32ToFloat4(0x43AA8BFF)};

    std::vector<ImVec4> plot_colors = {ImVec4(0.26f,0.67f,0.55f,1.00f),
                                       ImVec4(0.74f,0.74f,0.92f,1.00f),
                                       ImVec4(0.34f,0.46f,0.56f,1.00f),
                                       ImVec4(0.73f,0.95f,0.88f,1.00f),
                                       ImVec4(0.98f,0.38f,0.38f,1.00f),
                                       ImVec4(0.78f,0.55f,0.42f,1.00f),
                                       ImVec4(0.98f,0.78f,0.31f,1.00f),
                                       ImVec4(0.66f,0.36f,0.42f,1.00f)};


    std::vector<int> baud_rates = {CBR_110, CBR_300, CBR_600, CBR_1200, CBR_2400, CBR_4800, CBR_9600, CBR_14400, CBR_19200, CBR_38400, CBR_56000, CBR_57600, CBR_115200, CBR_128000, CBR_256000};

    bool auto_scroll = true;

public:
    BSP(/* args */);
    ~BSP();
    void update();
    bool begin_serial();
    void close_serial();
    void read_serial();
    void reset_read();
    void append_all_data(std::vector<float> curr_data);
};

} // namespace bsp