#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <istream>
#include <Mahi/Gui.hpp>
#include <imgui.h>

namespace bsp{

struct ScrollingData {
     int MaxSize = 1000;
     int Offset  = 0;
     std::string name;
     bool show = false;
     ImVector<ImVec2> Data;
     ScrollingData(std::string name_):
     name(name_)
     { 
          Data.reserve(MaxSize); 
     }
     void AddPoint(float x, float y) {
          if (Data.size() < MaxSize)
               Data.push_back(ImVec2(x,y));
          else {
               Data[Offset] = ImVec2(x,y);
               Offset =  (Offset + 1) % MaxSize;
          }
     }
     void set_name(std::string name_){name = name_;}
};

// void plot_data(const ScrollingData &data, int i);

std::vector<int> get_serial_ports();

} // namespace bsp