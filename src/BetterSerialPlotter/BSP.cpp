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

    constexpr ImGuiWindowFlags padding_flag = ImGuiWindowFlags_AlwaysUseWindowPadding;

    time = static_cast<float>(program_clock.get_elapsed_time().as_seconds());
    ImGui::Begin("Better Serial Plotter", &open, padding_flag);

    

    io = ImGui::GetIO();
    ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

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
        if (old_size < curr_data.size()){
            for (int i = old_size; i < curr_data.size(); i++){
                all_data.emplace_back();
                all_data[i].set_name("data " + std::to_string(i));
                all_data[i].identifier = old_size+i;
                all_data[i].color = plot_colors[i%plot_colors.size()];
            }
        }
        else{
            std::cout << "here";
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

    auto func = [this]() {
        std::string filepath;
        auto result = mahi::gui::save_dialog(filepath, {{"Config File", "json"}}, "", "bsp_config");
        if (result == mahi::gui::DialogResult::DialogOkay){
            mahi::util::print("Path: {}",filepath);
            
            BSPData bsp_data;

            {
                std::lock_guard<std::mutex> lock(mtx);
                bsp_data = BSPData(this);
            }

            nlohmann::json j_out;

            j_out["bsp_data"] = bsp_data;

            std::ofstream ofile(filepath);
            
            ofile << j_out;

            ofile.close();
        }
    };

    std::thread thrd(func); thrd.detach();
    
}

void BSP::deserialize(){

    auto func = [this]() {
        std::string filepath;
        auto result = mahi::gui::open_dialog(filepath, {{"Config File", "json"}});
        if (result == mahi::gui::DialogResult::DialogOkay){
            std::lock_guard<std::mutex> lock(mtx);
            // std::cout << "here";
            mahi::util::print("Path: {}",filepath);
            // std::cout << "here";
            serial_manager.close_serial();

            std::ifstream ifile(filepath);
            // std::cout << "here";
            nlohmann::json j_in;
            ifile >> j_in;
            std::cout << "pre_json\n";
            auto bsp_data = j_in["bsp_data"].get<BSPData>(); 
            std::cout << "post_json\n";

            ifile.close();

            all_data = bsp_data.all_data;
            std::cout << "post_all_data\n";
            data_names.resize(all_data.size());
            data_colors.resize(all_data.size());
            for (auto i = 0; i < all_data.size(); i++){
                data_names[i] = all_data[i].name;
                data_colors[i] = all_data[i].color;
            }

            plot_monitor = bsp_data.plot_monitor;
            plot_monitor.gui = this;
            for (auto &plot : plot_monitor.all_plots)
            {
                plot.plot_monitor = &plot_monitor;
            }

            std::cout << "bsp_data comport_num: " << bsp_data.serial_manager.comport_num << "\n";
            serial_manager = bsp_data.serial_manager;
            std::cout << "post serial_manager\n";
            serial_manager.gui = this;
            serial_manager.serial_started = true;
            // std::cout << "here3";
            // this is to try to use the comport
            serial_manager.comport_num = bsp_data.serial_manager.comport_num;
            std::cout << bsp_data.serial_manager.comport_num << std::endl;
            serial_manager.baud_rate = bsp_data.serial_manager.baud_rate;
            serial_manager.begin_serial();
            // std::cout << "here4";
            serial_manager.reset_read();

            program_clock.restart();
        }
    };
    std::thread thrd(func); thrd.detach();

    
}

} // namespace bsp