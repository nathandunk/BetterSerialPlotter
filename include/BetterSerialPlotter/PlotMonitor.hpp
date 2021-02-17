#pragma once

#include <BetterSerialPlotter/Widget.hpp>

namespace bsp{

class PlotMonitor : public Widget
{
public:
    void render();
    PlotMonitor(BSP* gui_);
};

} // namespace bsp