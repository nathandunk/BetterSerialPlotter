#pragma once

#include <BetterSerialPlotter/Widget.hpp>

namespace bsp{

class DataPanel : public Widget
{
public:
    void render();
    DataPanel(BSP* gui_);
};

} // namespace bsp