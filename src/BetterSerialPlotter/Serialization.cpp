#include <BetterSerialPlotter/Serialization.hpp>

// BSP
void bsp::to_json(nlohmann::json& j, const BSPData& bsp_data){
    j = nlohmann::json{{"all_data",       bsp_data.all_data}, 
                       {"serial_manager", bsp_data.serial_manager}, 
                       {"plot_monitor",   bsp_data.plot_monitor}};
}
void bsp::from_json(const nlohmann::json& j, BSPData& bsp_data){
    j.at("all_data").get_to(bsp_data.all_data);
    j.at("serial_manager").get_to(bsp_data.serial_manager);
    j.at("plot_monitor").get_to(bsp_data.plot_monitor);
    std::cout << bsp_data.serial_manager.comport_num << ", " << bsp_data.serial_manager.baud_rate;
}

// ScrollingData
void bsp::to_json(nlohmann::json& j, const bsp::ScrollingData& scrolling_data){
    j = nlohmann::json{{"identifier", scrolling_data.identifier}, 
                       {"MaxSize",    scrolling_data.MaxSize}, 
                       {"name",       scrolling_data.name},
                       {"color",      scrolling_data.color}};
}
void bsp::from_json(const nlohmann::json& j, bsp::ScrollingData& scrolling_data){
    j.at("identifier").get_to(scrolling_data.identifier);
    j.at("MaxSize").get_to(scrolling_data.MaxSize);
    j.at("name").get_to(scrolling_data.name);
    j.at("color").get_to(scrolling_data.color);
}

// PlotMonitor
void bsp::to_json(nlohmann::json& j, const PlotMonitor& plot_monitor){
    j = nlohmann::json{{"all_plots", plot_monitor.all_plots}};
}
void bsp::from_json(const nlohmann::json& j, PlotMonitor& plot_monitor){
    j.at("all_plots").get_to(plot_monitor.all_plots);
}

// SerialManager
void bsp::to_json(nlohmann::json& j, const bsp::SerialManager& serial_manager){
    j = nlohmann::json{{"comport_num", serial_manager.comport_num}, 
                       {"baud_rate",   serial_manager.baud_rate}};
}
void bsp::from_json(const nlohmann::json& j, bsp::SerialManager& serial_manager){
    j.at("comport_num").get_to(serial_manager.comport_num);
    j.at("baud_rate").get_to(serial_manager.baud_rate);
}

// Plot
void bsp::to_json(nlohmann::json& j, const Plot& plot){
    j = nlohmann::json{{"other_x_axis",           plot.other_x_axis}, 
                       {"all_plot_data",          plot.all_plot_data}, 
                       {"x_axis_realtime",        plot.x_axis_realtime}, 
                       {"x_axis",                 plot.x_axis},
                       {"y_axis",                 plot.y_axis},
                       {"time_frame",             plot.time_frame},
                       {"paused_x_axis_modifier", plot.paused_x_axis_modifier},
                       {"plot_height",            plot.plot_height},
                       {"name",                   plot.name},
                       {"autoscale",              plot.autoscale}};
}
void bsp::from_json(const nlohmann::json& j, Plot& plot){
    j.at("other_x_axis").get_to(plot.other_x_axis);
    j.at("all_plot_data").get_to(plot.all_plot_data);
    j.at("x_axis_realtime").get_to(plot.x_axis_realtime);
    j.at("x_axis").get_to(plot.x_axis);
    j.at("y_axis").get_to(plot.y_axis);
    j.at("time_frame").get_to(plot.time_frame);
    j.at("paused_x_axis_modifier").get_to(plot.paused_x_axis_modifier);
    j.at("plot_height").get_to(plot.plot_height);
    j.at("name").get_to(plot.name);
    j.at("autoscale").get_to(plot.autoscale);
}

// ImVec4
void to_json(nlohmann::json& j, const ImVec4& imvec4){
    j = nlohmann::json{{"w", imvec4.w}, 
                       {"x", imvec4.x}, 
                       {"y", imvec4.y}, 
                       {"z", imvec4.z}};
}
void from_json(const nlohmann::json& j, ImVec4& imvec4){
    j.at("w").get_to(imvec4.w);
    j.at("x").get_to(imvec4.x);
    j.at("y").get_to(imvec4.y);
    j.at("z").get_to(imvec4.z);
}