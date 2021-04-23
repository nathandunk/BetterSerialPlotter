#pragma once

#include <BetterSerialPlotter/Utility.hpp>
#include <unordered_map>
#include <optional>

namespace bsp{

class PlotMonitor;

class Plot
{
public:
    PlotMonitor* plot_monitor; // pointer to the PlotMonitor parent object
public:
    /// constructor with pointer to parent PlotMonitor
    Plot(PlotMonitor* plot_monitor_);
    /// default constructor
    Plot();
    /// code for setup of plot information
    void make_plot(float time, int plot_num);
    /// actually plots the data that goes on the plot
    void plot_data();
    /// adds a unique identifier to this plot, adding the variable to the dsiplay
    void add_identifier(char identifier, int y_axis_num = 0);
    /// removes a unique identifier to this plot, removing the variable from the display
    void remove_identifier(char identifier);
    /// checks if a unique identifier is used on this plot
    bool has_identifier(char identifier) const;
    /// sets all_plot_paused_data and paused_x_axis to current data
    void update_paused_data();
    /// returns the data corresponding to a specific identifier
    std::optional<std::reference_wrapper<ScrollingData>> get_data(char identifier);
    /// vector of all identifiers for this plot
    std::vector<char> all_plot_data;
    
    /// saved paused data for all variables on this plot
    std::vector<ScrollingData> all_plot_paused_data;

    bool other_x_axis = false; // indicates that x-axis for this plot is something other than this program time
    bool x_axis_realtime = true; // indicates that x-axis for this plot corresponds to something realtime, meaning that we want it to scroll with the 
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