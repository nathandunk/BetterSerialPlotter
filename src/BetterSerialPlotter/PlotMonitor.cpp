#include <BetterSerialPlotter/PlotMonitor.hpp>
#include <BetterSerialPlotter/BSP.hpp>

namespace bsp{

PlotMonitor::PlotMonitor(BSP* gui_): Widget(gui_){}

void PlotMonitor::render(){
    for (auto i = 0; i < gui->all_plots.size(); i++){
        gui->all_plots[i].make_plot(gui->time, i, gui->all_data);
    }
    ImGui::PushStyleColor(ImGuiCol_Button,gui->PalleteBlue);
    if(ImGui::Button("Add Plot")) {
        gui->all_plots.emplace_back();
    }
    ImGui::PopStyleColor();
    ImGui::EndTabItem();
}

} // namespace bsp