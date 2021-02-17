#include <BetterSerialPlotter/SerialManager.hpp>
#include <BetterSerialPlotter/BSP.hpp>
#include <iostream>

namespace bsp{

SerialManager::SerialManager(BSP* gui_): Widget(gui_){}

void SerialManager::render(){
    constexpr ImGuiWindowFlags padding_flag = ImGuiWindowFlags_AlwaysUseWindowPadding;

    ImGui::PushStyleColor(ImGuiCol_FrameBg, (gui->comport_num > 0) ? (gui->serial_status ? gui->PalleteGreen : gui->PalleteRed) : gui->PalleteGray);
    ImGui::PushStyleColor(ImGuiCol_Header, (gui->comport_num > 0) ? (gui->serial_status ? gui->PalleteGreen : gui->PalleteRed) : gui->PalleteGray);
    ImGui::BeginChild("SerialSetup", ImVec2(-1, 36), false, padding_flag);
    // comport selection
    ImGui::PushItemWidth(200);
    if (ImGui::BeginCombo("##comport_select", ("COM: " + ((gui->comport_num >= 0) ? std::to_string(gui->comport_num) : "")).c_str())){
        std::vector<int> port_names = get_serial_ports();
        for (int i = 0; i < port_names.size(); i++){
            const bool com_is_selected = (gui->comport_num == port_names[i]);
            if (ImGui::Selectable(("COM"+std::to_string(port_names[i])).c_str(), com_is_selected)){
                // make sure this isn't what we are already connected to
                if(gui->comport_num != port_names[i]) {
                    if (gui->serial_started){
                        gui->close_serial();
                    }
                    gui->comport_num = port_names[i];
                    gui->serial_started = true;
                    gui->begin_serial();
                    gui->reset_read();
                }
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::SameLine();
    // baud rate select
    ImGui::PushItemWidth(200);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (gui->baud_rate > 0) ? (gui->baud_status ? gui->PalleteGreen : gui->PalleteRed) : gui->PalleteGray);
    ImGui::PushStyleColor(ImGuiCol_Header, (gui->baud_rate > 0) ? (gui->baud_status ? gui->PalleteGreen : gui->PalleteRed) : gui->PalleteGray);
    if (ImGui::BeginCombo("##baud_rate", ("Baud Rate: " + ((gui->baud_rate >= 0) ? std::to_string(gui->baud_rate) : "")).c_str())){
        for (int i = 0; i < gui->baud_rates.size(); i++){
            const bool baud_is_selected = (gui->baud_rate == gui->baud_rates[i]);
            if (ImGui::Selectable(("COM"+std::to_string(gui->baud_rates[i])).c_str(), baud_is_selected)){
                // make sure this isn't what we are already connected to
                if(gui->baud_rate != gui->baud_rates[i]) {
                    gui->baud_rate = gui->baud_rates[i];
                    DCB dcbSerialParams = {0};
                    dcbSerialParams.DCBlength=sizeof(dcbSerialParams);

                    dcbSerialParams.BaudRate=gui->baud_rate;
                    dcbSerialParams.ByteSize=8;
                    dcbSerialParams.StopBits=ONESTOPBIT;
                    dcbSerialParams.Parity=NOPARITY;
                    if(!SetCommState(gui->hSerial, &dcbSerialParams)){
                        std::cout << "could not set com state" << std::endl;
                    }
                    gui->reset_read();
                }
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();
    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
}

} // namespace bsp