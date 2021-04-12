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
     char identifier = 0;
     int MaxSize = 5000;
     int Offset  = 0;
     // std::string name;
     // bool show = false;
     ImVector<ImVec2> Data;
     // ImVec4 color;
     // int y_axis = 0;
     ScrollingData()
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
     ImVec2& get_back(){
          if (Data.size() < MaxSize){
               return Data.back();
          }
          else{
               return Data[Offset != 0 ? (Offset-1) : MaxSize];
          }
     }
     ImVector<float> get_x(){
          ImVector<float> x_data;
          for (const auto &i : Data) x_data.push_back(i.x);
          return x_data;
     }
     ImVector<float> get_y(){
          ImVector<float> y_data;
          for (const auto &i : Data) y_data.push_back(i.y);
          return y_data;
     }
     // void set_name(std::string name_){name = name_;}
     void set_identifier(char identifier){identifier = identifier;}
};

struct DataInfo {
     // char identifier = 0;
     std::string name;
     ImVec4 color;
     void set_name(std::string name_){name = name_;}
     void set_identifier(char identifier){identifier = identifier;}
};

// void plot_data(const ScrollingData &data, int i);
#ifdef __APPLE__
std::vector<std::string> get_serial_ports();
#else 
std::vector<int> get_serial_ports();
#endif

} // namespace bsp