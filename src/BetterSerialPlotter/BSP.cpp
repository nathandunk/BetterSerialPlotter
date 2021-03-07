#include <tchar.h>
#include <codecvt>
#include <locale>
#include <iostream>
#include <BetterSerialPlotter/BSP.hpp>
#include <BetterSerialPlotter/Utility.hpp>
#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>
#include <regex>

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

    constexpr ImGuiWindowFlags padding_flag = ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_MenuBar;

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
}

void BSP::append_all_data(std::vector<float> curr_data){
    auto old_size = all_data.size();
    if (old_size != curr_data.size()){
        // ScrollingData NewScrollingData;
        // all_data.resize(curr_data.size());
        if (old_size < curr_data.size()){
            // std::cout << "adding\n";
            for (auto i = old_size; i < curr_data.size(); i++){
                // std::cout << i << "\n";
                all_data.emplace_back(std::make_shared<ScrollingData>());
                // std::cout << "after_emplace\n";
                all_data[i]->set_name("data " + std::to_string(i));
                // std::cout << "after_set_name\n";
                all_data[i]->set_identifier(i);
                all_data[i]->color = plot_colors[i%plot_colors.size()];
            }
        }
        else{
            // std::cout << "removing " << curr_data.size() << "\n";
            for (auto i = old_size-1; i > old_size - curr_data.size(); i--){
                std::cout << i << "\n";
                all_data.erase(all_data.begin()+i);
            }
        }
    }
    float curr_time = static_cast<float>(program_clock.get_elapsed_time().as_seconds());
    for (auto i = 0; i < curr_data.size(); i++){
        all_data[i]->AddPoint(curr_time, curr_data[i]);
    }
}

} // namespace bsp