#include <BetterSerialPlotter/PlotMonitor.hpp>
#include <BetterSerialPlotter/BSP.hpp>
#include <iostream>
#include <Mahi/Util/Logging/Csv.hpp>
#include <array>
#include <Mahi/Util/Random.hpp>
#include <Mahi/Util/Print.hpp>
#include <thread>

using namespace mahi::util;

namespace bsp{

PlotMonitor::PlotMonitor(BSP* gui_): 
    Widget(gui_)
    {
        all_plots.emplace_back(this);
    }

void PlotMonitor::render(){
    // std::cout << "being plot monitor\n";
    bool was_paused = paused;
    if (ImGui::Button(paused ? "Resume" : "Pause")) paused = !paused; ImGui::SameLine();
    if (ImGui::Button("Export CSV")) export_data(); 
    ImGui::SameLine();
    if (ImGui::Button("Save Config")) gui->serialize();
    ImGui::SameLine();
    if (ImGui::Button("Load Config")) gui->deserialize();
    if (paused && !was_paused){
        for (auto &plot : all_plots){
            plot.update_paused_data();
        }
        paused_time = gui->time;
    }
    // std::cout << "Before make_plot\n";
    for (auto i = 0; i < all_plots.size(); i++){
        all_plots[i].make_plot(paused ? paused_time : gui->time, i);
    }
    // std::cout << "After make_plot\n";
    ImGui::PushStyleColor(ImGuiCol_Button,gui->PalleteBlue);
    if(ImGui::Button("Add Plot")) {
        all_plots.emplace_back(this);
        all_plots[all_plots.size()-1].name = "Plot " + std::to_string(all_plots.size());
    }
    ImGui::PopStyleColor();
    // std::cout << "end plot monitor\n";
}

void PlotMonitor::export_data(){
    std::string filepath;
    auto result = mahi::gui::save_dialog(filepath, {{"CSV", "csv"}}, "", "data.csv");
    if (result == mahi::gui::DialogResult::DialogOkay){
        auto num_datas = gui->all_data.size();

        if (num_datas == 0){
            std::cout << "no data to export";
            return;
        }

        auto num_samples = gui->all_data[0].Data.size();
        auto offset = gui->all_data[0].Offset;

        mahi::util::print("Path: {}",filepath);
        
        std::vector<std::string> headers;
        std::vector<std::vector<double>> all_rows;
        
        headers.reserve(num_datas+1);

        // add the names as headers for the csv file
        headers.push_back("Program Time [s]");
        for (const auto &data : gui->all_data){
            headers.push_back(gui->get_name(data.identifier));
        }
        
        // // add all of the data points
        for (auto i = 0; i < num_samples; i++){
            std::vector<double> row;
            row.reserve(num_datas+1);
            row.push_back(gui->all_data[0].Data[(i + offset)%num_samples].x);
            for (const auto &data : gui->all_data){           
                row.push_back(data.Data[(i + offset)%num_samples].y);
            }
            all_rows.push_back(row);
        }
        
        csv_write_row(filepath, headers);
        csv_append_rows(filepath, all_rows);
    }
}

} // namespace bsp