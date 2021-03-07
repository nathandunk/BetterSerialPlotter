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
    void add_identifier(char identifier, int y_axis_num = 0);
    void remove_identifier(char identifier);
    bool has_identifier(char identifier) const;
    void update_paused_data();
    std::shared_ptr<ScrollingData> get_data(char identifier);
    std::vector<std::shared_ptr<ScrollingData>> all_plot_data;
    std::vector<ScrollingData> all_plot_paused_data;
    bool other_x_axis = false;
    std::shared_ptr<ScrollingData> x_axis;
    ScrollingData paused_x_axis;
    std::vector<char> idenfifiers;
    std::unordered_map<char,int> y_axis;
    bool is_resizing = false;
    float time_frame = 10.0f;
    float paused_x_axis_modifier = 0.1f;
    float plot_height = 200.0f;
    float plot_y_end = 0.0;
    float resize_area = 6.0f;
    std::string name = "Plot 1";
};
    
} // namespace bsp