#include <BetterSerialPlotter/PlotMonitor.hpp>
#include <BetterSerialPlotter/BSP.hpp>
#include <iostream>

namespace bsp{

PlotMonitor::PlotMonitor(BSP* gui_): 
    Widget(gui_)
    {
        all_plots.emplace_back(this);
    }

void PlotMonitor::render(){
    bool was_paused = paused;
    if (ImGui::Button(paused ? "Resume" : "Pause")) paused = !paused; ImGui::SameLine();
    ImGui::Button("Export CSV"); ImGui::SameLine();
    ImGui::Button("Save Config");
    if (paused && !was_paused){
        for (auto &plot : all_plots){
            plot.update_paused_data();
        }
        paused_time = gui->time;
    }
    for (auto i = 0; i < all_plots.size(); i++){
        all_plots[i].make_plot(paused ? paused_time : gui->time, i);
    }
    ImGui::PushStyleColor(ImGuiCol_Button,gui->PalleteBlue);
    if(ImGui::Button("Add Plot")) {
        all_plots.emplace_back(this);
        all_plots[all_plots.size()-1].name = "Plot " + std::to_string(all_plots.size());
    }
    ImGui::PopStyleColor();
    ImGui::EndTabItem();
}

void export_data(){

}

} // namespace bsp