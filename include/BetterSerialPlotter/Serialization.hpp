#pragma once

#include <vector>
#include <string>
#include <imgui.h>
#include <BetterSerialPlotter/Plot.hpp>
#include <BetterSerialPlotter/SerialManager.hpp>
#include <BetterSerialPlotter/BSP.hpp>
#include <nlohmann/json.hpp>

namespace bsp{

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

void to_json(nlohmann::json& j, const BSPData& bsp_data);
void from_json(const nlohmann::json& j, BSPData& bsp_data);

void to_json(nlohmann::json& j, const bsp::ScrollingData& scrolling_data);
void from_json(const nlohmann::json& j, bsp::ScrollingData& scrolling_data);

void to_json(nlohmann::json& j, const bsp::DataInfo& data_info);
void from_json(const nlohmann::json& j, bsp::DataInfo& data_info);

void to_json(nlohmann::json& j, const PlotMonitor& bsp);
void from_json(const nlohmann::json& j, PlotMonitor& bsp);

void to_json(nlohmann::json& j, const SerialManager& serial_manager);
void from_json(const nlohmann::json& j, SerialManager& serial_manager);

void to_json(nlohmann::json& j, const Plot& plot);
void from_json(const nlohmann::json& j, Plot& plot);

} // namespace bsp

void to_json(nlohmann::json& j, const ImVec4& imvec4);
void from_json(const nlohmann::json& j, ImVec4& imvec4);