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
    if(other_x_axis){
        auto x_min = mahi::util::min_element(x_axis->get_y());
        auto x_max = mahi::util::max_element(x_axis->get_y());
        x_min -= paused_x_axis_modifier*abs(x_min);
        x_max += paused_x_axis_modifier*abs(x_max);
        ImPlot::SetNextPlotLimitsX(x_min, x_max, plot_monitor->paused ? ImGuiCond_Once : ImGuiCond_Always);   
    }
    else{
        ImPlot::SetNextPlotLimitsX(time - time_frame, time, plot_monitor->paused ? ImGuiCond_Once : ImGuiCond_Always);    
    }
    
    if(ImPlot::BeginPlot(("##Better Serial Plot Monitor" + std::to_string(plot_num)).c_str(), "Time (s)", "Value", {-1,200}, ImPlotFlags_YAxis2, 0, 0)){
        plot_data();
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_PLOT")) {
                int i = *(int*)payload->Data;
                if (!has_identifier(plot_monitor->gui->all_data[i]->m_identifier)) all_plot_data.push_back(plot_monitor->gui->all_data[i]);
                // for (int y = 0; y < 2; y++) {
                //     if (ImPlot::IsPlotYAxisHovered(y)){
                //         y_axis[plot_monitor->gui->all_data[i]->m_identifier] = y;
                //     }
                // }
                // if (ImPlot::IsPlotXAxisHovered()){
                //     other_x_axis = true;
                //     x_axis = plot_monitor->gui->all_data[i];
                // }
            }
            ImGui::EndDragDropTarget();
        }
        for (int y = 0; y < 2; ++y) {
            if (ImPlot::BeginDragDropTargetY(y)) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_PLOT")) {
                    int i = *(int*)payload->Data;
                    if (!has_identifier(plot_monitor->gui->all_data[i]->m_identifier)) all_plot_data.push_back(plot_monitor->gui->all_data[i]);
                    y_axis[plot_monitor->gui->all_data[i]->m_identifier] = y;
                }
                ImPlot::EndDragDropTarget();
            }
        }
        if (ImPlot::BeginDragDropTargetX()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_PLOT")) {
                int i = *(int*)payload->Data;
                other_x_axis = true;
                x_axis = plot_monitor->gui->all_data[i];
            }
            ImPlot::EndDragDropTarget();
        }
        if(ImPlot::IsPlotXAxisHovered()){
            if (other_x_axis) paused_x_axis_modifier += plot_monitor->gui->io.MouseWheel/20.0f;
            else time_frame *= 1.0f+plot_monitor->gui->io.MouseWheel/100.0f;
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
            // if x_axis_id is specified, use that as the x-axis rather than from ScrollingData
            auto x_data = (other_x_axis) ? &paused_x_axis.Data[0].y : &all_plot_paused_data[i].Data[0].x;

            ImPlot::PushStyleColor(ImPlotCol_Line,all_plot_paused_data[i].color);
            char id[64];
            sprintf(id,"%s###%i",all_plot_paused_data[i].name.c_str(),i);
            ImPlot::SetPlotYAxis(y_axis[plot_monitor->gui->all_data[i]->m_identifier]);
            ImPlot::PlotLine(id, x_data, &all_plot_paused_data[i].Data[0].y, all_plot_paused_data[i].Data.size(), all_plot_paused_data[i].Offset, 2 * sizeof(float));  
            ImPlot::PopStyleColor();
        }    
    }
    else{
        for (auto i = 0; i < all_plot_data.size(); i++){
            // if x_axis_id is specified, use that as the x-axis rather than from ScrollingData
            auto x_data = (other_x_axis) ? &x_axis->Data[0].y : &all_plot_data[i]->Data[0].x;

            ImPlot::PushStyleColor(ImPlotCol_Line,all_plot_data[i]->color); 
            char id[64];
            sprintf(id,"%s###%i",all_plot_data[i]->name.c_str(),i);
            ImPlot::SetPlotYAxis(y_axis[plot_monitor->gui->all_data[i]->m_identifier]);
            ImPlot::PlotLine(id, x_data, &all_plot_data[i]->Data[0].y, all_plot_data[i]->Data.size(), all_plot_data[i]->Offset, 2 * sizeof(float));  
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

std::shared_ptr<ScrollingData> Plot::get_data(char identifier){
    for (auto &data : all_plot_data){
        if (data->m_identifier == identifier) return data;
    }
    // if we didn't find a valid data point, return a default shared_ptr scrolling data
    return std::make_shared<ScrollingData>();
}

void Plot::update_paused_data(){
    all_plot_paused_data.clear();
    all_plot_paused_data.reserve(all_plot_data.size());
    for (const auto& shared_ptr_data : all_plot_data){
        all_plot_paused_data.push_back(*shared_ptr_data);
    }
    paused_x_axis = *x_axis;
    
    // all_plot_paused_data = all_plot_data;
}

}