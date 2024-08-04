#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <istream>
#include <Mahi/Gui.hpp>
#include <imgui.h>

namespace bsp{

// data structure to handle a single variable coming in from serial
struct ScrollingData {
     std::string identifier;   // unique identifier that can be used to pull this data
     int MaxSize = 5000;    // maximum amount of data points that will be stored
     int Offset  = 0;       // offset to handle plotting
     ImVector<ImVec2> Data; // vector of x and y data. X data always is time
     
     /// default constructor
     ScrollingData(){
          Data.reserve(MaxSize);
     }

     /// add a point to this variable instance. Adds a variable to the Data vector
     /// and updates the offset for plotting
     void AddPoint(float x, float y) {
          if (Data.size() < MaxSize)
               Data.push_back(ImVec2(x,y));
          else {
               Data[Offset] = ImVec2(x,y);
               Offset =  (Offset + 1) % MaxSize;
          }
     }

     /// returns the most recent data point received from serial
     ImVec2& get_back(){
          if (Data.size() < MaxSize){
               return Data.back();
          }
          else{
               return Data[Offset != 0 ? (Offset-1) : MaxSize];
          }
     }

     /// returns vector of x-data (not fixed for offset)
     ImVector<float> get_x(){
          ImVector<float> x_data;
          for (const auto &i : Data) x_data.push_back(i.x);
          return x_data;
     }

     /// returns vector of all y-data (not fixed for offset)
     ImVector<float> get_y(){
          ImVector<float> y_data;
          for (const auto &i : Data) y_data.push_back(i.y);
          return y_data;
     }

     /// set the identifier
     void set_identifier(std::string identifier){identifier = identifier;}
};

struct DataInfo {
     // char identifier = 0;
     std::string name;
     ImVec4 color;
     
     /// set the unique identifier of the variable
     void set_name(std::string name_){name = name_;}
     void set_identifier(char identifier){identifier = identifier;}
};

struct NamedSerialData {
    std::string name;
    float data;
};

// void plot_data(const ScrollingData &data, int i);
#ifdef __APPLE__
std::vector<std::string> get_serial_ports();
#else 
std::vector<int> get_serial_ports();
#endif

} // namespace bsp