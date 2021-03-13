#pragma once

#include <imgui.h>

namespace bsp{

class BSP;

class Widget{
    public:
    Widget(BSP* gui_);
    Widget();
    virtual void render() = 0;
    
    BSP* gui;
};

} // namespace bsp