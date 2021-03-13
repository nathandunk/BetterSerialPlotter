#pragma once

#include <BetterSerialPlotter/Widget.hpp>

namespace bsp{

Widget::Widget(BSP* gui_) : gui(gui_){}
Widget::Widget() : gui(nullptr){}

} // namespace bsp