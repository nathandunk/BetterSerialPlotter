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
        auto x_min = mahi::util::min_element(get_data(x_axis).get_y());
        auto x_max = mahi::util::max_element(get_data(x_axis).get_y());
        x_min -= paused_x_axis_modifier*abs(x_min);
        x_max += paused_x_axis_modifier*abs(x_max);
        ImPlot::SetNextPlotLimitsX(x_min, x_max, plot_monitor->paused ? ImGuiCond_Once : ImGuiCond_Always); 
    }
    else{
        ImPlot::SetNextPlotLimitsX(time - time_frame, time, plot_monitor->paused ? ImGuiCond_Once : ImGuiCond_Always);    
    }
    std::string text = "";
    if(ImPlot::BeginPlot(name.c_str(), "Time (s)", "Value", {-1,plot_height}, ImPlotFlags_NoMenus | ImPlotFlags_YAxis2, 0, 0)){
        plot_data();
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_PLOT")) {
                int i = *(int*)payload->Data;
<<<<<<< HEAD
                // if (!has_identifier(plot_monitor->gui->all_data[i]->m_identifier)) all_plot_data.push_back(plot_monitor->gui->all_data[i]);
                add_identifier(plot_monitor->gui->all_data[i]->m_identifier);
=======
                if (!has_identifier(plot_monitor->gui->all_data[i].identifier)) {
                    all_plot_data.push_back(plot_monitor->gui->all_data[i].identifier);
                }
>>>>>>> data_restructure
            }
            ImGui::EndDragDropTarget();
        }
        for (int y = 0; y < 2; ++y) {
            if (ImPlot::BeginDragDropTargetY(y)) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_PLOT")) {
                    int i = *(int*)payload->Data;
<<<<<<< HEAD
                    // if (!has_identifier(plot_monitor->gui->all_data[i]->m_identifier)) all_plot_data.push_back(plot_monitor->gui->all_data[i]);
                    // y_axis[plot_monitor->gui->all_data[i]->m_identifier] = y;
                    add_identifier(plot_monitor->gui->all_data[i]->m_identifier, y);
=======
                    if (!has_identifier(plot_monitor->gui->all_data[i].identifier)) {
                        all_plot_data.push_back(plot_monitor->gui->all_data[i].identifier);
                    }
                    y_axis[plot_monitor->gui->all_data[i].identifier] = y;
>>>>>>> data_restructure
                }
                ImPlot::EndDragDropTarget();
            }
        }
        if (ImPlot::BeginDragDropTargetX()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_PLOT")) {
                int i = *(int*)payload->Data;
                other_x_axis = true;
                x_axis = plot_monitor->gui->all_data[i].identifier;
            }
            ImPlot::EndDragDropTarget();
        }
        
        // handle x-axis resizing
        if(ImPlot::IsPlotXAxisHovered()){
            if (other_x_axis) paused_x_axis_modifier += plot_monitor->gui->io.MouseWheel/20.0f;
            else time_frame *= 1.0f+plot_monitor->gui->io.MouseWheel/100.0f;
        }
        // handle context menus
        if (ImGui::IsMouseReleased(1)){
            if (ImPlot::IsPlotHovered()){
                ImGui::OpenPopup("##BSPPlotContext");
            }

            else if (ImPlot::IsPlotXAxisHovered()){
                ImGui::OpenPopup("##XAxisContext");
            }

            else if (ImPlot::IsPlotYAxisHovered(0)){
                ImGui::OpenPopup("##YAxis0Context");
            }

            else if (ImPlot::IsPlotYAxisHovered(1)){
                ImGui::OpenPopup("##YAxis1Context");
            }
        }

        if(ImGui::BeginPopup("##BSPPlotContext")){
            if ((ImGui::BeginMenu("Remove Data"))){
                for (auto i = 0; i < idenfifiers.size(); i++){
                    ImGui::MenuItem(get_data(idenfifiers[i]).name.c_str());
                    //     idenfifiers.erase(idenfifiers.begin() + i);
                    //     break;
                    // }
                }
                ImGui::EndMenu();
            } 
            ImGui::EndPopup();
        }

        // update and region of the plot for dragging to resize        
        plot_y_end = ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMax().y;
        ImPlot::EndPlot();
    }
<<<<<<< HEAD

    if (ImGui::GetMousePos().y <= plot_y_end + resize_area && ImGui::GetMousePos().y >= plot_y_end - resize_area){
        ImGui::SetMouseCursor(3);
        if (ImGui::IsMouseClicked(0)){
            is_resizing = true;
        }
    }

    if (ImGui::IsMouseReleased(0) && is_resizing){
        is_resizing = false;
        ImGui::SetMouseCursor(0);
    }

    if (is_resizing){
        plot_height += plot_monitor->gui->io.MouseDelta.y;
        ImGui::SetMouseCursor(3);
    }
=======
>>>>>>> data_restructure
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
            ImPlot::SetPlotYAxis(y_axis[plot_monitor->gui->all_data[i].identifier]);
            ImPlot::PlotLine(id, x_data, &all_plot_paused_data[i].Data[0].y, all_plot_paused_data[i].Data.size(), all_plot_paused_data[i].Offset, 2 * sizeof(float));  
            ImPlot::PopStyleColor();
        }    
    }
    else{
        for (auto i = 0; i < all_plot_data.size(); i++){
            // if x_axis_id is specified, use that as the x-axis rather than from ScrollingData
            auto x_data = (other_x_axis) ? &get_data(x_axis).Data[0].y : &get_data(all_plot_data[i]).Data[0].x;

            ImPlot::PushStyleColor(ImPlotCol_Line,get_data(all_plot_data[i]).color); 
            char id[64];
            sprintf(id,"%s###%i",get_data(all_plot_data[i]).name.c_str(),i);
            ImPlot::SetPlotYAxis(y_axis[plot_monitor->gui->all_data[i].identifier]);
            ImPlot::PlotLine(id, x_data, &get_data(all_plot_data[i]).Data[0].y, get_data(all_plot_data[i]).Data.size(), get_data(all_plot_data[i]).Offset, 2 * sizeof(float));  
            ImPlot::PopStyleColor();
        }    
    }
    
}

void Plot::add_identifier(char identifier, int y_axis_num){
    // check if it is already there
    for (const auto &i : idenfifiers){
        if (identifier == i)
            return;
    }
    
    // if it isnt already there, add it and set y axis to 0 (default)
    idenfifiers.push_back(identifier);
    y_axis[identifier] = y_axis_num;
}

void Plot::remove_identifier(char identifier){
    // look for the identifier in the identifiers vector
    for (auto i = idenfifiers.begin(); i != idenfifiers.end(); i++){
        if (*i == identifier) {
            idenfifiers.erase(i);
            break;
        }
    }
    
    // look for the identifier in the y_axis unordered map
    for (auto i = y_axis.begin(); i != y_axis.end(); i++){
        if (i->first == identifier) {
            y_axis.erase(i);
            break;
        }
    }
}

bool Plot::has_identifier(char identifier) const{
    for (const auto &data : all_plot_data){
        if (data == identifier) return true;
    }
    return false;
}

ScrollingData& Plot::get_data(char identifier){
    return plot_monitor->gui->get_data(identifier);
}

void Plot::update_paused_data(){
    all_plot_paused_data.clear();
    all_plot_paused_data.reserve(all_plot_data.size());
    for (const auto& data : all_plot_data){
        all_plot_paused_data.push_back(get_data(data));
    }
    if (other_x_axis) paused_x_axis = get_data(x_axis);
    // all_plot_paused_data = all_plot_data;
}

}