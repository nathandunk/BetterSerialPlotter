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
    /// default constructor
    BSPData():
        all_data(),
        serial_manager(),
        plot_monitor()
        {}
    
    /// constructor which requires gui pointer, and creates all 
    /// resulting widgets with that pointer
    BSPData(BSP* bsp):
        all_data(bsp->all_data),
        serial_manager(bsp->serial_manager),
        plot_monitor(bsp->plot_monitor)
        {}
    
    std::vector<ScrollingData> all_data; // copied all_data
    SerialManager serial_manager;        // copied serial_manager
    PlotMonitor plot_monitor;            // copied plot_monitor
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