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
    std::vector<std::shared_ptr<ScrollingData>> all_plot_data;
};
    
} // namespace bsp