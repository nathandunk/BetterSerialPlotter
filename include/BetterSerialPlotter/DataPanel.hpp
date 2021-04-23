#pragma once

#include <BetterSerialPlotter/Widget.hpp>

namespace bsp{

/// object for Data Panel on the left side of the GUI. Mainly handles
/// sizing and the editing of variable properties
class DataPanel : public Widget
{
private:
    bool editing_name = false;
    int editing_num = -1; // number that is currently being edited. -1 if none
public:
    /// constructor that assigns the gui pointer
    DataPanel(BSP* gui_);
    
    /// render the imgui appearance
    void render();
};

} // namespace bsp