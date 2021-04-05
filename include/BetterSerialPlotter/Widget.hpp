#pragma once

#include <imgui.h>

namespace bsp{

class BSP;

/// default abstract class with connection to the main gui class and abstract render function
class Widget{
    public:
    /// default constructor that doesn't set the gui pointer
    Widget();
    /// constructor to set the gui variable
    Widget(BSP* gui_);
    /// virtual render function which must be overriden
    virtual void render() = 0;
    
    BSP* gui; // pointer to the main gui class
};

} // namespace bsp