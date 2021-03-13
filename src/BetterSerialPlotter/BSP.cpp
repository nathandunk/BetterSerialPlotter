#include <tchar.h>
#include <codecvt>
#include <locale>
#include <iostream>
#include <BetterSerialPlotter/BSP.hpp>
#include <BetterSerialPlotter/Utility.hpp>
#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>
#include <regex>
#include <nlohmann/json.hpp>
#include <BetterSerialPLotter/Serialization.hpp>

namespace bsp{

BSP::BSP(/* args */) : 
    mahi::gui::Application(),
    data_panel(this),
    serial_manager(this),
    plot_monitor(this),
    serial_monitor(this),
    PrintBuffer(200)
{
    program_clock.restart();
    auto &app_colors = ImGui::GetStyle().Colors;
    // app_colors[2] = ImVec4(0.33f,0.35f,0.39f,1.0f); // whole window bg
    app_colors[ImGuiCol_ChildBg] = PalleteBlueGray; // child window bg

}

BSP::~BSP()
{
}

void BSP::update(){
    // ImGuiConfigFlags config_flags = ImGuiConfigFlags_

    constexpr ImGuiWindowFlags padding_flag = ImGuiWindowFlags_AlwaysUseWindowPadding;

    time = static_cast<float>(program_clock.get_elapsed_time().as_seconds());
    ImGui::Begin("Better Serial Plotter", &open, padding_flag);

    // if (ImGui::BeginMenuBar()){
    //     if (ImGui::BeginMenu("Export Data")){
    //         if (ImGui::MenuItem("Export Live Data")){

    //         }
    //         if (ImGui::MenuItem("Export Paused Data")){

    //         }
    //         ImGui::EndMenu();
    //     }
    //     ImGui::EndMenuBar();
    // }
    

    io = ImGui::GetIO();
    ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
    // io_ref.ConfigWindowsMoveFromTitleBarOnly = true;

    data_panel.render();
    
    ImGui::SameLine();
    
    ImGui::BeginGroup();

    serial_manager.render();
    

    if (ImGui::BeginTabBar("MainAreaTabs")){
        if (ImGui::BeginTabItem("Plots")){
            plot_monitor.render();
        }
        if (ImGui::BeginTabItem("SerialMonitor")){
            serial_monitor.render();
        }
        ImGui::EndTabBar();
    }

    ImGui::EndGroup();
    
    ImGui::End();
    
    if(!open) quit();
    if (serial_manager.serial_started) serial_manager.read_serial();
    if (mark_deserialize) {
        deserialize();
        mark_deserialize = false;
    }
}

void BSP::append_all_data(std::vector<float> curr_data){
    auto old_size = all_data.size();
    if (old_size != curr_data.size()){
        // ScrollingData NewScrollingData;
        // all_data.resize(curr_data.size());
        if (old_size < curr_data.size()){
            // std::cout << "adding\n";
            for (int i = old_size; i < curr_data.size(); i++){
                all_data.emplace_back();
                // std::cout << "after_emplace\n";
                all_data[i].set_name("data " + std::to_string(i));
                // std::cout << "after_set_name\n";
                all_data[i].identifier = old_size+i;
                all_data[i].color = plot_colors[i%plot_colors.size()];
                // std::cout << static_cast<int>(all_data[i].identifier) << std::endl;
            }
        }
        else{
            for (auto i = old_size-1; i > old_size - curr_data.size(); i--){
                std::cout << i << "\n";
                all_data.erase(all_data.begin()+i);
            }
        }
        
    }
    float curr_time = static_cast<float>(program_clock.get_elapsed_time().as_seconds());
    for (auto i = 0; i < curr_data.size(); i++){
        all_data[i].AddPoint(curr_time, curr_data[i]);
    }
}

ScrollingData& BSP::get_data(char identifier){
    for (auto &data : all_data){
        if (data.identifier == identifier){
            return data;
        }
    }

    return ScrollingData();
}

void BSP::serialize(){
    BSPData bsp_data(this);
    nlohmann::json j_out;
    j_out["bsp_data"] = bsp_data;

    std::string filepath = "test.json";

    std::ofstream ofile(filepath);
    
    ofile << j_out;

    ofile.close();
}

void BSP::deserialize(){
    serial_manager.close_serial();

    std::ifstream ifile("test.json");

    nlohmann::json j_in;
    ifile >> j_in;
    auto bsp_data = j_in["bsp_data"].get<BSPData>(); 

    ifile.close();

    all_data = bsp_data.all_data;
    plot_monitor = bsp_data.plot_monitor;
    plot_monitor.gui = this;
    for (auto &plot : plot_monitor.all_plots)
    {
        plot.plot_monitor = &plot_monitor;
    }
    
    serial_manager = bsp_data.serial_manager;
    serial_manager.gui = this;
    serial_manager.serial_started = true;
    serial_manager.begin_serial();
    
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength=sizeof(dcbSerialParams);

    dcbSerialParams.BaudRate=serial_manager.baud_rate;
    dcbSerialParams.ByteSize=8;
    dcbSerialParams.StopBits=ONESTOPBIT;
    dcbSerialParams.Parity=NOPARITY;
    if(!SetCommState(serial_manager.hSerial, &dcbSerialParams)){
        std::cout << "could not set com state" << std::endl;
    }
    serial_manager.reset_read();

    program_clock.restart();
}

} // namespace bsp