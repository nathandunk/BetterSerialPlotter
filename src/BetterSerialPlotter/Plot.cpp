#include <BetterSerialPlotter/Plot.hpp>
#include <Mahi/Gui.hpp>
#include <iostream>

namespace bsp{

Plot::Plot(/* args */)
{
}

void Plot::make_plot(float time, int plot_num, std::vector<ScrollingData> &all_data){
    ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 3);
    ImPlot::PushStyleColor(ImPlotCol_FrameBg,ImVec4(0.33f,0.35f,0.39f,1.0f));
    // ImPlot::PushStyleColor(ImPlotCol_FrameBg,ImVec4(1.0f,1.0f,1.0f,1.0f));
    ImPlot::SetNextPlotLimitsX(time - 10, time, ImGuiCond_Always);
    if(ImPlot::BeginPlot(("##Better Serial Plot Monitor" + std::to_string(plot_num)).c_str(), "Time (s)", "Value", {-1,200}, 0, 0, 0)){
        plot_data();
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_PLOT")) {
                int i = *(int*)payload->Data;
                if (!has_identifier(all_data[i].m_identifier)) all_plot_data.push_back(std::ref(all_data[i]));
            }
            ImGui::EndDragDropTarget();
        }
        ImPlot::EndPlot();
    }
    ImPlot::PopStyleColor();
    ImPlot::PopStyleVar();
}

void Plot::plot_data(){
    for (auto i = 0; i < all_plot_data.size(); i++){
        ImPlot::PushStyleColor(ImPlotCol_Line,all_plot_data[i].get().color);
        char id[64];
        sprintf(id,"%s###%i",all_plot_data[i].get().name.c_str(),i);
        ImPlot::PlotLine(id, &all_plot_data[i].get().Data[0].x, &all_plot_data[i].get().Data[0].y, all_plot_data[i].get().Data.size(), all_plot_data[i].get().Offset, 2 * sizeof(float));  
        ImPlot::PopStyleColor();
    }    
}

bool Plot::has_identifier(char identifier){
    for (const auto &data : all_plot_data){
        if (data.get().m_identifier == identifier) return true;
    }
    return false;
}

}