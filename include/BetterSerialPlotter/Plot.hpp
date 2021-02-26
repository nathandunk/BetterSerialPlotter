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
    std::vector<std::shared_ptr<ScrollingData>> all_plot_data;
    std::vector<ScrollingData> all_plot_paused_data;
};
    
} // namespace bsp