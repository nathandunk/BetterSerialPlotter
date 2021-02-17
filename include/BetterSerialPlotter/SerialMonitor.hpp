#pragma once

#include <BetterSerialPlotter/Widget.hpp>

namespace bsp{

class SerialMonitor : public Widget
{
public:
    void render();
    SerialMonitor(BSP* gui_);

private:
    bool auto_scroll = true;
};

} // namespace bsp