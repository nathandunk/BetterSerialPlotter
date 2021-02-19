#pragma once

#include <BetterSerialPlotter/Widget.hpp>

namespace bsp{

class DataPanel : public Widget
{
private:
    bool editing_name = false;
    int editing_num = -1;
public:
    void render();
    DataPanel(BSP* gui_);
};

} // namespace bsp