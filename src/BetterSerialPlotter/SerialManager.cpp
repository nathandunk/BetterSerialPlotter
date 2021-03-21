#include <BetterSerialPlotter/SerialManager.hpp>
#include <BetterSerialPlotter/BSP.hpp>
#include <iostream>
#include <codecvt>
#include <regex>

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

SerialManager& SerialManager::operator=(const SerialManager& serial_manager){
    comport_num = serial_manager.comport_num;
    baud_rate = serial_manager.baud_rate;
    return *this;
}

void SerialManager::render(){
    constexpr ImGuiWindowFlags padding_flag = ImGuiWindowFlags_AlwaysUseWindowPadding;

    ImGui::PushStyleColor(ImGuiCol_FrameBg, (comport_num > 0) ? (serial_status ? gui->PalleteGreen : gui->PalleteRed) : gui->PalleteGray);
    ImGui::PushStyleColor(ImGuiCol_Header, (comport_num > 0) ? (serial_status ? gui->PalleteGreen : gui->PalleteRed) : gui->PalleteGray);
    ImGui::BeginChild("SerialSetup", ImVec2(-1, 36), false, padding_flag);
    // comport selection
    ImGui::PushItemWidth(200);
    if (ImGui::BeginCombo("##comport_select", ("Serial Port: " + ((comport_num >= 0) ? get_port_name(comport_num) : "")).c_str())){
        std::vector<int> port_names = get_serial_ports();
        for (int i = 0; i < port_names.size(); i++){
            const bool com_is_selected = (comport_num == port_names[i]);
            if (ImGui::Selectable(get_port_name(port_names[i]).c_str(), com_is_selected)){
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
    try{
        serial_started = serial_port.open((mahi::com::Port)(comport_num),baud_rate);
        serial_port.flush_RXTX();
    }
    catch(const std::exception& e){
        std::cerr << e.what() << '\n';
        serial_started = false;
    }
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
    
    read_once = false;
}

void SerialManager::read_serial(){
    int BytesRead = 0;
    static unsigned char message[packet_size];
    
    do{
        BytesRead = serial_port.receive_data(message, packet_size);
        // if we got something from serial, parse it, and indicate that serial is functioning
        if(BytesRead > 0 ){
            parse_buffer(message, BytesRead);
            cycles_waited = 0;
            serial_status = true;
        }
        // if we have waited too many cycles, indicate that serial is not functioning
        else{
            if (++cycles_waited > cycle_timeout) serial_status = false;
        }
    } while (BytesRead > 0);
}

void SerialManager::parse_buffer(unsigned char* buff, size_t buff_len){
    for (size_t i = 0; i < buff_len; i++){
        curr_line_buff += buff[i];
        // if the current char is valid, add it to a string buff representing the current number
        // valid chars include:
        // digits 0-9 which can be a part of any number
        // +/- to denote positive/negativeness
        // E/e to denote scientific notation
        if (std::regex_match(std::string(1, buff[i]),std::regex("[0-9Ee\\.+-]")) && read_once){
            curr_number_buff += buff[i];
            if (gui->verbose) std::cout << buff[i];
        }
        // if tab (0x09) or space (0x20) or newline (0x0a)
        else if ((buff[i] == 0x09 || buff[i] == 0x20 || buff[i] == 0x0a) && read_once){

            // add the current number to the data for the current line
            try{
                curr_data.push_back(std::stof(curr_number_buff));
            }
            catch(const std::exception& e){
                std::cerr << e.what() << '\n';
            }
            curr_number_buff = "";
            if (gui->verbose) std::cout << "\t";
            baud_status = true;

            // if this is a newline character, then do further cleaning of line information variables 
            if (buff[i] == 0x0a){
                gui->PrintBuffer.push_back(curr_line_buff);
                curr_line_buff.clear();
                gui->append_all_data(curr_data);
                if (gui->verbose) std::cout << std::endl;
                curr_data.clear();
                read_once = true;
            }
        }
        // if we got an unexpected value (return character (0x0d) gets ignored), set baud status to false
        else if (buff[i] != 0x0d && read_once) {
            baud_status = false;
        }
        // get through a full line once so that we get the beginning of a line without any half-numbers
        else if (buff[i] == 0x0a){
            read_once = true;
        }
    }
}

std::string SerialManager::get_port_name(int port_num){
#ifdef WIN32
    return "COM" + std::to_string(port_num + 1);
#else
    if (port_num >= 16 && port_num <= 21){
        return "ttyUSB" + std::to_string(port_num - 16);
    } 
    else if (port_num >= 24 && port_num <= 25){
        return "ttyACM" + std::to_string(port_num - 24);
    }
#endif

}

} // namespace bsp