#pragma once

#include <BetterSerialPlotter/Utility.hpp>
#include <unordered_map>
#include <optional>

namespace bsp{

class PlotMonitor;

class Plot
{
public:
    PlotMonitor* plot_monitor;
public:
    Plot(PlotMonitor* plot_monitor_);
    Plot();
    void make_plot(float time, int plot_num);
    void plot_data();
    void add_identifier(char identifier, int y_axis_num = 0);
    void remove_identifier(char identifier);
    bool has_identifier(char identifier) const;
    void update_paused_data();
    std::optional<std::reference_wrapper<ScrollingData>> get_data(char identifier);
    std::vector<char> all_plot_data;
    std::vector<ScrollingData> all_plot_paused_data;
    bool other_x_axis = false;
    bool x_axis_realtime = true;
    char x_axis = -1;
    ScrollingData paused_x_axis;
    std::unordered_map<char,int> y_axis;
    bool is_resizing = false;
    float time_frame = 10.0f;
    float paused_x_axis_modifier = 0.1f;
    float plot_height = 300.0f;
    float plot_y_end = 0.0;
    float resize_area = 6.0f;
    std::string name = "Plot 1";
    bool autoscale = false;
};
    
} // namespace bsp