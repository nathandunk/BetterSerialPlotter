#include <BetterSerialPlotter/Plot.hpp>
#include <BetterSerialPlotter/PlotMonitor.hpp>
#include <BetterSerialPlotter/BSP.hpp>
#include <Mahi/Gui.hpp>
#include <iostream>
#include <array>

namespace bsp{

Plot::Plot(PlotMonitor* plot_monitor_): 
    plot_monitor(plot_monitor_)
    {}

Plot::Plot(): plot_monitor(nullptr){}

void Plot::make_plot(float time, int plot_num){
    ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 3);
    ImPlot::PushStyleVar(ImPlotStyleVar_LabelPadding,ImVec2(3,2));
    ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding,ImVec2(2,2));
    ImPlot::PushStyleColor(ImPlotCol_FrameBg,ImVec4(0.33f,0.35f,0.39f,1.0f));
    if(other_x_axis && !x_axis_realtime){
        if (autoscale && !plot_monitor->paused){
            
            float x_min = 0.0f;
            float x_max = 1.0f;

            if (get_data(x_axis).Data.size()){
                // std::cout << get_data(x_axis).Data.size();
                x_min = mahi::util::min_element(get_data(x_axis).get_y());
                x_max = mahi::util::max_element(get_data(x_axis).get_y());
            }
            ImPlot::SetNextPlotLimitsX(x_min, x_max, plot_monitor->paused ? ImGuiCond_Once : ImGuiCond_Always); 
        }
    }
    else{
        if (other_x_axis){
            float most_recent_time = 0.0f;
            if (get_data(x_axis).Data.size()){
                most_recent_time = mahi::util::max_element(get_data(x_axis).get_y());
            }
            ImPlot::SetNextPlotLimitsX(most_recent_time - time_frame, most_recent_time, plot_monitor->paused ? ImGuiCond_Once : ImGuiCond_Always);
        }
        else{
            ImPlot::SetNextPlotLimitsX(time - time_frame, time, plot_monitor->paused ? ImGuiCond_Once : ImGuiCond_Always);
        }
    }
    if (autoscale){
        // vectors which contain min and max for y axis 0 and y axis 1
        std::vector<std::vector<float>> min;
        std::vector<std::vector<float>> max;

        min.push_back(std::vector<float>());
        min.push_back(std::vector<float>());
        max.push_back(std::vector<float>());
        max.push_back(std::vector<float>());

        for (auto it = y_axis.begin(); it != y_axis.end(); it++){
            if(!get_data(it->first).Data.empty()){
                min[it->second].push_back(mahi::util::min_element(get_data(it->first).get_y()));
                max[it->second].push_back(mahi::util::max_element(get_data(it->first).get_y()));
            }
        }
        for (auto i = 0; i < 2; i++){
            bool axis_exists = false;

            for (const auto & [key, value] : y_axis){
                if (value == i && !min[i].empty()){
                    axis_exists = true;
                    break;
                }
            }
            
            if (axis_exists){
                ImPlot::SetNextPlotLimitsY(mahi::util::min_element(min[i]),mahi::util::max_element(max[i]),ImGuiCond_Always,i);
            }           
        }        
    }
    // std::cout << "before begin_plot\n";
    std::string text = "";
    if(ImPlot::BeginPlot(name.c_str(), other_x_axis ? get_data(x_axis).name.c_str() : "Time (s)", 0, {-1,plot_height}, ImPlotFlags_NoMenus | ImPlotFlags_YAxis2, 0, 0)){
        plot_data();
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_PLOT")) {
                int i = *(int*)payload->Data;
                add_identifier(plot_monitor->gui->all_data[i].identifier);
            }
            ImGui::EndDragDropTarget();
        }
        for (int y = 0; y < 2; ++y) {
            if (ImPlot::BeginDragDropTargetY(y)) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_PLOT")) {
                    int i = *(int*)payload->Data;
                    add_identifier(plot_monitor->gui->all_data[i].identifier, y);                    
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
            if (other_x_axis & !x_axis_realtime) paused_x_axis_modifier += plot_monitor->gui->io.MouseWheel/20.0f;
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
            if ((ImGui::BeginMenu("Add Data"))){
                for (auto i = 0; i < plot_monitor->gui->all_data.size(); i++){
                    ImPlot::ItemIcon(plot_monitor->gui->all_data[i].color); ImGui::SameLine();
                    if(ImGui::BeginMenu(plot_monitor->gui->all_data[i].name.c_str())){
                        // add based on y axis
                        for (auto y = 0; y < 2; y++){
                            char menu_item_name[10];
                            sprintf(menu_item_name, "Y-Axis %i", y);
                            if(ImGui::MenuItem(menu_item_name)){
                                add_identifier(plot_monitor->gui->all_data[i].identifier,y);
                            }
                        }
                        // add to x axis
                        if(ImGui::MenuItem("X-Axis")){
                            x_axis = plot_monitor->gui->all_data[i].identifier;
                            other_x_axis = true;
                        }
                        ImGui::EndMenu();
                    }
                }
                ImGui::EndMenu();
            } 
            if ((ImGui::BeginMenu("Remove Data"))){
                for (auto i = 0; i < all_plot_data.size(); i++){
                    ImPlot::ItemIcon(get_data(all_plot_data[i]).color); ImGui::SameLine();
                    if(ImGui::MenuItem((get_data(all_plot_data[i]).name + " (y-axis " + std::to_string(y_axis[all_plot_data[i]]) + ")").c_str())){
                        remove_identifier(get_data(all_plot_data[i]).identifier);
                        break;
                    }
                }
                if (other_x_axis){
                    ImPlot::ItemIcon(get_data(x_axis).color);  ImGui::SameLine();
                    if(ImGui::MenuItem((get_data(x_axis).name + " (x-axis)").c_str())){
                        other_x_axis = false;
                        x_axis = -1;
                    }
                }
                ImGui::EndMenu();
            } 
            ImGui::MenuItem("Auto Scale",0,&autoscale);
            ImGui::MenuItem("Realtime X-axis",0,&x_axis_realtime);
            ImGui::EndPopup();
        }

        // update and region of the plot for dragging to resize        
        plot_y_end = ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMax().y;
        ImPlot::EndPlot();
    }
    // std::cout << "before resizing\n";

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
    ImPlot::PopStyleColor();
    ImPlot::PopStyleVar();
    ImPlot::PopStyleVar();
    ImPlot::PopStyleVar();
}

void Plot::plot_data(){

    for (auto i = 0; i < all_plot_data.size(); i++){
        // get the correct set of data based on what we are currently doing
        auto &curr_data = plot_monitor->paused ? all_plot_paused_data[i] : get_data(all_plot_data[i]);
        auto &curr_identifier = plot_monitor->paused ? all_plot_paused_data[i].identifier : all_plot_data[i];
        float* curr_x_data; // would prefer this to be a reference, but have to assign immediately, so can't
        if (plot_monitor->paused){
            curr_x_data = (other_x_axis) ? &paused_x_axis.Data[0].y : &all_plot_paused_data[i].Data[0].x;
        }
        else{
            curr_x_data = (other_x_axis) ? &get_data(x_axis).Data[0].y : &curr_data.Data[0].x;
        }

        ImPlot::PushStyleColor(ImPlotCol_Line,curr_data.color); 
        char id[64];
        sprintf(id,"%s###%i",curr_data.name.c_str(),i);
        ImPlot::SetPlotYAxis(y_axis[curr_identifier]);
        ImPlot::PlotLine(id, curr_x_data, &curr_data.Data[0].y, curr_data.Data.size(), curr_data.Offset, 2 * sizeof(float));  
        ImPlot::PopStyleColor();
    }   
    
}

void Plot::add_identifier(char identifier, int y_axis_num){
    bool exists = false;
    // check if it is already there
    for (const auto &i : all_plot_data){
        if (identifier == i) {
            // std::cout << (int)identifier <<  " " << (int)i << std::endl;
            exists = true;
            break;
        }
    }
    // std::cout << exists;
    
    // if it isnt already there, add it and set y axis to 0 (default)
    if (!exists) all_plot_data.push_back(identifier);
    
    y_axis[identifier] = y_axis_num;
}

void Plot::remove_identifier(char identifier){
    // look for the identifier in the identifiers vector
    for (auto i = all_plot_data.begin(); i != all_plot_data.end(); i++){
        if (*i == identifier) {
            all_plot_data.erase(i);
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