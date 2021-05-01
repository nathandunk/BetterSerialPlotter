#pragma once

#include <BetterSerialPlotter/Widget.hpp>
#include <BetterSerialPlotter/Utility.hpp>
#include <BetterSerialPlotter/Plot.hpp>

namespace bsp{

class PlotMonitor : public Widget
{
public:
    /// constructor which takes in gui pointer
    PlotMonitor(BSP* gui_);
    
    /// default constructor
    PlotMonitor(){};
    
    /// renders the imgui information for the widget
    void render();
    
    /// plots all data on the first plot of the interface
    void plot_all_data();

    /// exports the data as a CSV
    void export_data();

    std::vector<Plot> all_plots; // vector of plot objects

    bool paused = false;                                     // tracks whether the plots are paused
    std::vector<std::shared_ptr<ScrollingData>> paused_data; // stores all data states for when pause is pressed
    float paused_time = 0;                                   // time that gui was paused
};

} // namespace bsp