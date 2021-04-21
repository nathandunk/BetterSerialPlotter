#include <codecvt>
#include <locale>
#include <iostream>
#include <BetterSerialPlotter/BSP.hpp>
#include <BetterSerialPlotter/Utility.hpp>
#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>
#include <regex>
#include <thread>
#include <nlohmann/json.hpp>
#include <BetterSerialPlotter/Serialization.hpp>

namespace bsp{

BSP::BSP(/* args */) : 
    mahi::gui::Application(1260,720,"Better Serial Plotter",true),
    data_panel(this),
    serial_manager(this),
    plot_monitor(this),
    serial_monitor(this),
    PrintBuffer(200)
{
    program_clock.restart();
    auto &app_colors = ImGui::GetStyle().Colors;
    app_colors[ImGuiCol_ChildBg] = PalleteBlueGray; // child window bg
    ImGui::DisableViewports();

    on_window_resized.connect(this, &BSP::window_resize_handler); 
}

BSP::~BSP()
{
    serial_manager.close_serial();
}

void BSP::window_resize_handler(int width, int height) {
    render_imgui();
}

void BSP::update(){
    if(serial_manager.serial_status){
        std::lock_guard<std::mutex> lock(serial_manager.mtx);
        all_data = mutexed_all_data;
    }

    constexpr ImGuiWindowFlags padding_flag = ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoTitleBar;
    auto w_size = get_window_size();
    ImGui::BeginFixed("Better Serial Plotter", {0,0}, {w_size.x, w_size.y}, padding_flag);

    time = static_cast<float>(program_clock.get_elapsed_time().as_seconds());
    io = ImGui::GetIO();
    ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
    data_panel.render();
    
    ImGui::SameLine();
    
    ImGui::BeginGroup();
    serial_manager.render();
    
    if (ImGui::BeginTabBar("MainAreaTabs")){
        if (ImGui::BeginTabItem("Plots")){
            plot_monitor.render();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("SerialMonitor")){
            serial_monitor.render();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::EndGroup();
    
    ImGui::End();
    
    if(!open) quit();
    if (deserialize_success){
        complete_deserialize();
        deserialize_success = false;
    }    
    // std::cout << "end update\n";
}

void BSP::append_all_data(std::vector<float> curr_data){    
    // std::cout << "begin append\n";
    std::lock_guard<std::mutex> lock(serial_manager.mtx);

    auto old_size = mutexed_all_data.size();
    if (old_size != curr_data.size()){
        if (old_size < curr_data.size()){
            for (int i = old_size; i < curr_data.size(); i++){
                mutexed_all_data.emplace_back();
                mutexed_all_data[i].identifier = old_size+i;
                if (all_data_info.find(mutexed_all_data[i].identifier) == all_data_info.end()){
                    all_data_info[mutexed_all_data[i].identifier].set_name("data " + std::to_string(i));
                    all_data_info[mutexed_all_data[i].identifier].color = plot_colors[i%plot_colors.size()];
                }
            }
        }
        else{
            for (auto i = old_size-1; i > old_size - curr_data.size(); i--){
                mutexed_all_data.erase(mutexed_all_data.begin()+i);
            }
        }
    }
    
    float curr_time = static_cast<float>(program_clock.get_elapsed_time().as_seconds());
    
    for (auto i = 0; i < curr_data.size(); i++){
        mutexed_all_data[i].AddPoint(curr_time, curr_data[i]);
    }
    // std::cout << "end append\n";
}

std::optional<std::reference_wrapper<ScrollingData>> BSP::get_data(char identifier){
    for (auto &data : all_data){
        if (data.identifier == identifier){
            return data;
        }
    }

    return std::nullopt; //ScrollingData();
}

std::string BSP::get_name(char identifier){
    auto found_it = all_data_info.find(identifier);
    return (found_it != all_data_info.end()) ? found_it->second.name : "";
}

ImVec4 BSP::get_color(char identifier){
    auto found_it = all_data_info.find(identifier);
    return (found_it != all_data_info.end()) ? found_it->second.color : ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
}

void BSP::serialize(){
    std::string filepath;
    auto result = mahi::gui::save_dialog(filepath, {{"Config File", "json"}}, "", "bsp_config");
    if (result == mahi::gui::DialogResult::DialogOkay){
        mahi::util::print("Path: {}",filepath);
        
        BSPData bsp_data;

        // std::cout << bsp_data.serial_manager.comport_num << ", " << bsp_data.serial_manager.baud_rate << std::endl;

        {
            std::lock_guard<std::mutex> lock(mtx);
            // std::cout << "pre this" << std::endl;
            bsp_data = BSPData(this);
            // std::cout << "post this" << std::endl;

            // std::cout << bsp_data.serial_manager.comport_num << ", " << bsp_data.serial_manager.baud_rate << std::endl;
        }

        nlohmann::json j_out;

        j_out["bsp_data"] = bsp_data;

        std::ofstream ofile(filepath);
        
        ofile << j_out;

        ofile.close();
    }
}

void BSP::deserialize(){
    auto result = mahi::gui::open_dialog(deserialize_filepath, {{"Config File", "json"}});
    if (result == mahi::gui::DialogResult::DialogOkay){
        std::lock_guard<std::mutex> lock(mtx);
        mahi::util::print("Path: {}",deserialize_filepath);
        deserialize_success = true;
    }
}

void BSP::complete_deserialize(){
    serial_manager.close_serial();

    std::ifstream ifile(deserialize_filepath);
    nlohmann::json j_in;
    ifile >> j_in;
    auto bsp_data = j_in["bsp_data"].get<BSPData>(); 

    ifile.close();

    all_data = bsp_data.all_data;
    mutexed_all_data = all_data;
    all_data_info = bsp_data.all_data_info;

    plot_monitor = bsp_data.plot_monitor;
    plot_monitor.gui = this;
    for (auto &plot : plot_monitor.all_plots)
    {
        plot.plot_monitor = &plot_monitor;
    }

    serial_manager = bsp_data.serial_manager;
    serial_manager.gui = this;
    serial_manager.serial_started = true;
    // this is to try to use the comport
    serial_manager.comport_num = bsp_data.serial_manager.comport_num;
    serial_manager.baud_rate = bsp_data.serial_manager.baud_rate;
    serial_manager.begin_serial();
    serial_manager.reset_read();

    program_clock.restart();
    std::cout << "done deserializing";
}

} // namespace bsp