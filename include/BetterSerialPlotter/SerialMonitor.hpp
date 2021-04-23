#pragma once

#include <BetterSerialPlotter/Widget.hpp>

namespace bsp{

/// widget that renders the serial_monitor in the second tab of the
/// main area of the GUI
class SerialMonitor : public Widget
{
public:
    /// renders the imgui information for the widget
    void render();
    /// constructor which takes in pointer to the main gui
    SerialMonitor(BSP* gui_);

private:
    bool auto_scroll = true; // defines whether serial monitor always scrolls to bottom or not
};

} // namespace bsp