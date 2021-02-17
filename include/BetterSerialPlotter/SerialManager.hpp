#pragma once

#include <BetterSerialPlotter/Widget.hpp>

namespace bsp{

class SerialManager : public Widget
{
public:
    void render();
    SerialManager(BSP* gui_);
};

} // namespace bsp