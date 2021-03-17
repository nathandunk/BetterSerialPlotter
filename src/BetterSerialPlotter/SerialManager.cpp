#include <BetterSerialPlotter/SerialManager.hpp>
#include <BetterSerialPlotter/BSP.hpp>
#include <iostream>
#include <tchar.h>
#include <codecvt>
#include <regex>
// #include <locale>

namespace bsp{

SerialManager::SerialManager(BSP* gui_): 
    Widget(gui_),
    serial_port()
    {}

SerialManager::SerialManager(): 
    serial_port()
    {}

SerialManager::SerialManager(const SerialManager& serial_manager):
    serial_port(),
    comport_num(serial_manager.comport_num),
    baud_rate(serial_manager.baud_rate)
    {}

SerialManager SerialManager::operator=(const SerialManager& serial_manager){
    SerialManager new_serial_manager;
    new_serial_manager.comport_num = serial_manager.comport_num;
    new_serial_manager.baud_rate = serial_manager.baud_rate;
    return new_serial_manager;
}

void SerialManager::render(){
    constexpr ImGuiWindowFlags padding_flag = ImGuiWindowFlags_AlwaysUseWindowPadding;

    ImGui::PushStyleColor(ImGuiCol_FrameBg, (comport_num > 0) ? (serial_status ? gui->PalleteGreen : gui->PalleteRed) : gui->PalleteGray);
    ImGui::PushStyleColor(ImGuiCol_Header, (comport_num > 0) ? (serial_status ? gui->PalleteGreen : gui->PalleteRed) : gui->PalleteGray);
    ImGui::BeginChild("SerialSetup", ImVec2(-1, 36), false, padding_flag);
    // comport selection
    ImGui::PushItemWidth(200);
    if (ImGui::BeginCombo("##comport_select", ("COM: " + ((comport_num >= 0) ? std::to_string(comport_num) : "")).c_str())){
        std::vector<int> port_names = get_serial_ports();
        for (int i = 0; i < port_names.size(); i++){
            const bool com_is_selected = (comport_num == port_names[i]);
            if (ImGui::Selectable(("COM"+std::to_string(port_names[i])).c_str(), com_is_selected)){
                if (serial_started){
                    close_serial();
                }
                comport_num = port_names[i];
                begin_serial();
                reset_read();
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
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (baud_rate > 0) ? (baud_status ? gui->PalleteGreen : gui->PalleteRed) : gui->PalleteGray);
    ImGui::PushStyleColor(ImGuiCol_Header, (baud_rate > 0) ? (baud_status ? gui->PalleteGreen : gui->PalleteRed) : gui->PalleteGray);
    if (ImGui::BeginCombo("##baud_rate", ("Baud Rate: " + ((baud_rate >= 0) ? std::to_string(baud_rate) : "")).c_str())){
        for (int i = 0; i < baud_rates.size(); i++){
            const bool baud_is_selected = (baud_rate == baud_rates[i]);
            if (ImGui::Selectable((std::to_string(baud_rates[i])).c_str(), baud_is_selected)){

                baud_rate = baud_rates[i];

                close_serial();
                begin_serial();
                reset_read();

            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();
    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
}

bool SerialManager::begin_serial(){
    mahi::util::print("opening comport {}", comport_num);
    serial_started = serial_port.open((mahi::com::Port)(comport_num-1),baud_rate);
    serial_port.flush_RXTX();
    return serial_started;
}

void SerialManager::close_serial(){
    if (serial_port.is_open()) serial_port.close();
    serial_started = false;
}

void SerialManager::reset_read(){
    curr_number_buff.clear();
    curr_line_buff.clear();                              
    curr_data.clear();
    gui->PrintBuffer.clear();
    for (auto &data : gui->all_data){
        data.Data.clear();
    }
    
    // gui->all_data.clear();
    read_once = false;
}

void SerialManager::read_serial(){
    int BytesRead = 0;
    bool line_done = false;
    try{
    
    do{
        BytesRead = serial_port.receive_data(message, packet_size);
        // if(!ReadFile(hSerial, message, packet_size, &dwBytesRead, NULL)){
        //     // std::cout << "error reading from serial" << std::endl;
        // }
        // else{
        if(BytesRead > 0 ){
            // std::cout << "bytesread: " << dwBytesRead << "\n";
            // GETTING TO NEWLINE BEFORE DOING ANYTHING
            for (size_t i = 0; i < BytesRead; i++){
                curr_line_buff += message[i];

                // if tab (0x09) or space (0x20)
                if ((message[i] == 0x09 || message[i] == 0x20) && read_once){
                    if(std::regex_match(curr_number_buff,std::regex("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)"))
                    ){
                        curr_data.push_back(std::stof(curr_number_buff));
                        curr_number_buff = "";
                        if (gui->verbose) std::cout << "\t";
                        baud_status = true;
                    }
                    else{
                        curr_number_buff.clear();
                        curr_line_buff.clear();                              
                        curr_data.clear();
                        baud_status = false;
                    }
                }
                // if new line
                else if (message[i] == 0x0a && read_once){
                    if(std::regex_match(curr_number_buff,std::regex("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)"))
                        ){
                        curr_data.push_back(std::stof(curr_number_buff));
                        gui->PrintBuffer.push_back(curr_line_buff);
                        curr_number_buff.clear();
                        curr_line_buff.clear();
                        gui->append_all_data(curr_data);
                        if (gui->verbose) std::cout << std::endl;
                        
                        curr_data.clear();
                        line_done = true;
                        read_once = true;
                        baud_status = true;
                    }
                    else{
                        curr_number_buff.clear();
                        curr_line_buff.clear();                              
                        curr_data.clear();
                        baud_status = false;
                    }
                }
                else if ((message[i] >= '0' && message[i] <= '9' || message[i] == '.' || message[i] == '-') && read_once){
                    curr_number_buff += message[i];
                    if (gui->verbose) std::cout << message[i];
                }
                else if (message[i] != 0x0d && read_once) {
                    baud_status = false;
                    // std::cout << "invalid character: " << message[i] << std::endl;
                }
                else if (message[i] == 0x0a){
                    read_once = true;
                }
            }
            cycles_waited = 0;
            serial_status = true;
        }
        else{
            if (++cycles_waited > cycle_timeout) serial_status = false;
        }
        // }
    } while (!line_done && BytesRead > 0);
    }
    catch(const std::exception& e){
        std::cerr << e.what() << '\n';
        std::cout << curr_number_buff << std::endl;
    }

}

} // namespace bsp