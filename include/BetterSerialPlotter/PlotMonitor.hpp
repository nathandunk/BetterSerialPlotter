#pragma once

#include <BetterSerialPlotter/Widget.hpp>
#include <BetterSerialPlotter/Utility.hpp>
#include <BetterSerialPlotter/Plot.hpp>

namespace bsp{

class PlotMonitor : public Widget
{
public:
    std::vector<Plot> all_plots;
    std::vector<std::shared_ptr<ScrollingData>> paused_data;
    float paused_time = 0;
    bool paused = false;
    void render();
    PlotMonitor(BSP* gui_);
};

} // namespace bsp