#pragma once

#include <BetterSerialPlotter/Utility.hpp>

namespace bsp{

class PlotMonitor;

class Plot
{
private:
    PlotMonitor* plot_monitor;
public:
    Plot(PlotMonitor* plot_monitor_);
    void make_plot(float time, int plot_num);
    void plot_data();
    bool has_identifier(char identifier);
    void update_paused_data();
    std::shared_ptr<ScrollingData> get_data(char identifier);
    std::vector<std::shared_ptr<ScrollingData>> all_plot_data;
    std::vector<ScrollingData> all_plot_paused_data;
    bool other_x_axis = false;
    std::shared_ptr<ScrollingData> x_axis;
    ScrollingData paused_x_axis;
    std::vector<char> idenfifiers;
    std::unordered_map<char,int> y_axis;
    float time_frame = 10.0f;
    float paused_x_axis_modifier = 0.1f;
};
    
} // namespace bsp