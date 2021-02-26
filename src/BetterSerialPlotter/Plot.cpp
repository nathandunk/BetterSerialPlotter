#include <BetterSerialPlotter/Plot.hpp>
#include <BetterSerialPlotter/PlotMonitor.hpp>
#include <BetterSerialPlotter/BSP.hpp>
#include <Mahi/Gui.hpp>
#include <iostream>

namespace bsp{

Plot::Plot(PlotMonitor* plot_monitor_): plot_monitor(plot_monitor_) {}

void Plot::make_plot(float time, int plot_num){
    ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 3);
    ImPlot::PushStyleColor(ImPlotCol_FrameBg,ImVec4(0.33f,0.35f,0.39f,1.0f));
    // ImPlot::PushStyleColor(ImPlotCol_FrameBg,ImVec4(1.0f,1.0f,1.0f,1.0f));
    ImPlot::SetNextPlotLimitsX(time - 10, time, plot_monitor->paused ? ImGuiCond_Once : ImGuiCond_Always);
    if(ImPlot::BeginPlot(("##Better Serial Plot Monitor" + std::to_string(plot_num)).c_str(), "Time (s)", "Value", {-1,200}, 0, 0, 0)){
        plot_data();
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_PLOT")) {
                int i = *(int*)payload->Data;
                if (!has_identifier(plot_monitor->gui->all_data[i]->m_identifier)) all_plot_data.push_back(plot_monitor->gui->all_data[i]);
            }
            ImGui::EndDragDropTarget();
        }
        ImPlot::EndPlot();
    }
    ImPlot::PopStyleColor();
    ImPlot::PopStyleVar();
}

void Plot::plot_data(){
    // this is a dumb duplication of code, but I can't think of a good way to make this work without it at the moment.
    if (plot_monitor->paused){
        for (auto i = 0; i < all_plot_paused_data.size(); i++){
            ImPlot::PushStyleColor(ImPlotCol_Line,all_plot_paused_data[i].color);
            char id[64];
            sprintf(id,"%s###%i",all_plot_paused_data[i].name.c_str(),i);
            ImPlot::PlotLine(id, &all_plot_paused_data[i].Data[0].x, &all_plot_paused_data[i].Data[0].y, all_plot_paused_data[i].Data.size(), all_plot_paused_data[i].Offset, 2 * sizeof(float));  
            ImPlot::PopStyleColor();
        }    
    }
    else{
        for (auto i = 0; i < all_plot_data.size(); i++){
            ImPlot::PushStyleColor(ImPlotCol_Line,all_plot_data[i]->color);
            char id[64];
            sprintf(id,"%s###%i",all_plot_data[i]->name.c_str(),i);
            ImPlot::PlotLine(id, &all_plot_data[i]->Data[0].x, &all_plot_data[i]->Data[0].y, all_plot_data[i]->Data.size(), all_plot_data[i]->Offset, 2 * sizeof(float));  
            ImPlot::PopStyleColor();
        }    
    }
    
}

bool Plot::has_identifier(char identifier){
    for (const auto &data : all_plot_data){
        if (data->m_identifier == identifier) return true;
    }
    return false;
}

void Plot::update_paused_data(){
    all_plot_paused_data.clear();
    all_plot_paused_data.reserve(all_plot_data.size());
    for (const auto& shared_ptr_data : all_plot_data){
        all_plot_paused_data.push_back(*shared_ptr_data);
    }
    
    // all_plot_paused_data = all_plot_data;
}

}