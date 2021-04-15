#pragma once

#include <vector>
#include <string>
#include <imgui.h>
#include <BetterSerialPlotter/Plot.hpp>
#include <BetterSerialPlotter/SerialManager.hpp>
#include <BetterSerialPlotter/BSP.hpp>
#include <nlohmann/json.hpp>

namespace bsp{

/// struct which is intended to hold the serialized data of the gui for 
/// serialization.
struct BSPData{
    BSPData(BSP* bsp):
        all_data(bsp->all_data),
        serial_manager(bsp->serial_manager),
        plot_monitor(bsp->plot_monitor),
        all_data_info(bsp->all_data_info)
        {
            // std::cout << "BSP non-default: " << serial_manager.comport_num << std::endl;
        }
        
    BSPData():
        all_data(),
        serial_manager(),
        plot_monitor(),
        all_data_info()
        {
            // std::cout << "BSP default: " << serial_manager.comport_num << std::endl;
        }

    // BSPData(const BSPData& bsp_data):
    //     all_data(bsp_data.all_data),
    //     serial_manager(bsp_data.serial_manager),
    //     plot_monitor(bsp_data.plot_monitor)
    //     {}

    // ~BSPData(){};
        
    std::vector<ScrollingData> all_data;
    SerialManager serial_manager;
    PlotMonitor plot_monitor;
    std::unordered_map<char,DataInfo> all_data_info;
};

// all of these need to be in the bsp namespace because the to_json
// and from_json functions require the namespace to be the same as
// the object that you are converting

/// converts BSPData object to json
void to_json(nlohmann::json& j, const BSPData& bsp_data);
/// converts json to BSPData object
void from_json(const nlohmann::json& j, BSPData& bsp_data);

/// converts ScrollingData object to json
void to_json(nlohmann::json& j, const bsp::ScrollingData& scrolling_data);
/// converts json to ScrollingData object
void from_json(const nlohmann::json& j, bsp::ScrollingData& scrolling_data);

/// converts data_info object to json
void to_json(nlohmann::json& j, const bsp::DataInfo& data_info);
/// converts json to data_info object
void from_json(const nlohmann::json& j, bsp::DataInfo& data_info);

/// converts PlotMonitor object to json
void to_json(nlohmann::json& j, const PlotMonitor& bsp);
/// converts json to PlotMonitor object
void from_json(const nlohmann::json& j, PlotMonitor& bsp);

/// converts SerialManager object to json
void to_json(nlohmann::json& j, const SerialManager& serial_manager);
/// converts json to SerialManager object
void from_json(const nlohmann::json& j, SerialManager& serial_manager);

/// converts Plot object to json
void to_json(nlohmann::json& j, const Plot& plot);
/// converts json to Plot object
void from_json(const nlohmann::json& j, Plot& plot);

} // namespace bsp

// this needs to be in generic namespace, the same place that ImVec4 lives

/// converts ImVec4 object to json
void to_json(nlohmann::json& j, const ImVec4& imvec4);
/// converts json to ImVec4 object
void from_json(const nlohmann::json& j, ImVec4& imvec4);