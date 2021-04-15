#pragma once

#include <Mahi/Gui.hpp>
#include <optional>
#include <BetterSerialPlotter/Plot.hpp>
#include <BetterSerialPlotter/DataPanel.hpp>
#include <BetterSerialPlotter/SerialManager.hpp>
#include <BetterSerialPlotter/PlotMonitor.hpp>
#include <BetterSerialPlotter/SerialMonitor.hpp>
#include <BetterSerialPlotter/Utility.hpp>
#include <Mahi/Util/Templates/RingBuffer.hpp>
#include <Mahi/Util/Timing/Clock.hpp>
#include <mutex>

namespace bsp{

class BSP : public mahi::gui::Application
{
public:
    bool open = true;
    bool deserialize_success = false;
    std::string deserialize_filepath;
    
    mahi::util::RingBuffer<std::string> PrintBuffer; // ring buffer to keep track of 
    mahi::util::Clock program_clock;                 // clock to track program time
    
    bool verbose = false;
    int num_plots = 1;
    float time = 0;

    ImGuiIO io;

    // all_data[0] contains the full vector for variable 0
    // all_data[1] contains the full vector for variable 1
    // ...etc
    std::vector<ScrollingData> all_data;
    std::vector<ScrollingData> mutexed_all_data;

    // std::vector<std::string> data_names;
    // std::vector<ImVec4> data_colors;

    std::unordered_map<char,DataInfo> all_data_info;
    
    // HANDLE hSerial;

    ImVec4 PalleteRed       = ImVec4(1.00f,0.40f,0.40f,1.0f);
    ImVec4 PalleteGreen     = ImVec4(0.30f,0.67f,0.34f,1.0f);
    ImVec4 PalleteBlueGray  = ImVec4(0.33f,0.35f,0.39f,1.0f);
    ImVec4 PalleteGray      = ImVec4(0.37f,0.37f,0.37f,1.0f);
    ImVec4 PalleteYellow    = ImVec4(0.98f,0.78f,0.27f,1.0f);
    ImVec4 PalleteOrange    = ImVec4(0.98f,0.43f,0.27f,1.0f);
    ImVec4 PalleteBlue      = ImVec4(0.24f,0.62f,0.71f,1.0f);

    std::vector<ImVec4> plot_colors = {ImVec4(0.26f,0.67f,0.55f,1.00f),
                                       ImVec4(0.74f,0.74f,0.92f,1.00f),
                                       ImVec4(0.34f,0.46f,0.56f,1.00f),
                                       ImVec4(0.73f,0.95f,0.88f,1.00f),
                                       ImVec4(0.98f,0.38f,0.38f,1.00f),
                                       ImVec4(0.78f,0.55f,0.42f,1.00f),
                                       ImVec4(0.98f,0.78f,0.31f,1.00f),
                                       ImVec4(0.66f,0.36f,0.42f,1.00f)};

    DataPanel     data_panel;
    SerialManager serial_manager;
    PlotMonitor   plot_monitor;
    SerialMonitor serial_monitor;

    friend class DataPanel;
    friend class SerialManager;
    friend class PlotMonitor;
    friend class Plot;
    friend class SerialMonitor;

public:
    BSP(/* args */);
    ~BSP();
    void update();
    /// returns an optional reference wrapper to a scrolling data object. This returns the data corresponding to the requested identifier if available, or nullopt otherwise
    std::optional<std::reference_wrapper<ScrollingData>> get_data(char identifier);
    /// appends the vector of current data to the current data set. Need to make sure that this is working for any size of data
    void append_all_data(std::vector<float> curr_data);

    std::string get_name(char identifier);
    ImVec4 get_color(char identifier);

    void serialize();
    void deserialize();
    void complete_deserialize();

    std::mutex mtx;
};

} // namespace bsp