#include <BetterSerialPlotter/DataPanel.hpp>
#include <BetterSerialPlotter/BSP.hpp>

namespace bsp{

DataPanel::DataPanel(BSP* gui_): Widget(gui_){}

void DataPanel::render(){
    constexpr ImGuiWindowFlags padding_flag = ImGuiWindowFlags_AlwaysUseWindowPadding;

    ImGui::BeginChild("IncomingData", ImVec2(200, -1), false, padding_flag);
    ImGui::Text("Incoming Data");
    ImGui::Separator();
    if (ImGui::BeginTable("data_table", 2, ImGuiTableFlags_Resizable) && gui->serial_manager.baud_status){
        for (int i = 0; i < gui->all_data.size(); ++i) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Selectable(gui->all_data[i].name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns);
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptBeforeDelivery)) {
                ImGui::SetDragDropPayload("DND_PLOT", &i, sizeof(int));
                ImGui::TextUnformatted(gui->all_data[i].name.c_str());
                ImGui::EndDragDropSource();
            }
            static bool editing = false;
            if (ImGui::BeginPopupContextItem()){
                static char name[16];
                if(!editing){
                    strcpy(name,gui->all_data[i].name.c_str());
                }
                editing = true;
                ImGui::Text("Edit name:");
                ImGui::SameLine();
                ImGui::PushItemWidth(120);
                ImGui::InputText(("##edit" + std::to_string(i)).c_str(), name, IM_ARRAYSIZE(name));
                ImGui::PopItemWidth();
                ImGui::Text("Edit color:");
                ImGui::SameLine();
                ImGui::ColorEdit4("##Color", (float*)&gui->all_data[i].color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                if (ImGui::Button("Save") || (ImGui::IsKeyPressed(257)) || (ImGui::IsKeyPressed(335))){ // would change for mac/linux
                    gui->all_data[i].set_name(name);
                    ImGui::CloseCurrentPopup();
                    editing = false;
                }
                ImGui::EndPopup();
            }
            ImGui::TableNextColumn();
            // ImGui::SameLine();
            char buff[16];
            sprintf(buff, "%4.3f", gui->all_data[i].get_back().y);
            ImGui::Text(buff);
        }
        ImGui::EndTable();
    }
    
    ImGui::EndChild();
}

} // namespace bsp