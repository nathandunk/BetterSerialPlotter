#include <BetterSerialPlotter/SerialManager.hpp>
#include <BetterSerialPlotter/BSP.hpp>
#include <iostream>
#include <codecvt>
#include <regex>

namespace bsp{

SerialManager::SerialManager(BSP* gui_): 
    Widget(gui_),
    serial_port()
    {
        // std::cout << "SerialManager non-default constructor: " << comport_num << std::endl;
    }

SerialManager::SerialManager(): 
    serial_port()
    {
        // std::cout << "SerialManager default constructor: " << comport_num << std::endl;
    }

SerialManager::SerialManager(const SerialManager& serial_manager):
    serial_port(),
    comport_num(serial_manager.comport_num),
    baud_rate(serial_manager.baud_rate)
    {
        // std::cout << "SerialManager copy constructor: " << comport_num << std::endl;
    }

SerialManager& SerialManager::operator=(const SerialManager& serial_manager){
    comport_num = serial_manager.comport_num;
    baud_rate = serial_manager.baud_rate;
    // std::cout << "SerialManager assignment operator: " << comport_num << std::endl;
    return *this;
}

void SerialManager::render(){
    constexpr ImGuiWindowFlags padding_flag = ImGuiWindowFlags_AlwaysUseWindowPadding;

    ImGui::PushStyleColor(ImGuiCol_FrameBg, (comport_valid()) ? (serial_status ? gui->PalleteGreen : gui->PalleteRed) : gui->PalleteGray);
    ImGui::PushStyleColor(ImGuiCol_Header, (comport_valid()) ? (serial_status ? gui->PalleteGreen : gui->PalleteRed) : gui->PalleteGray);
    ImGui::BeginChild("SerialSetup", ImVec2(-1, 36), false, padding_flag);
    // comport selection
    ImGui::PushItemWidth(200);
    if (ImGui::BeginCombo("##comport_select", ("Serial Port: " + ((comport_valid()) ? get_port_name(comport_num) : "")).c_str())){
        auto port_names = get_serial_ports();
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
    serial_started = false;

    try{
#ifdef __APPLE__
        serial_started = serial_port.open(comport_num,baud_rate);
#else
        serial_started = serial_port.open((mahi::com::Port)(comport_num),baud_rate);
#endif
        serial_port.flush_RXTX();
    }
    catch(const std::exception& e){
        std::cerr << e.what() << '\n';
    }
    // std::thread read_thread(&BSP::read_serial,this);
    if (serial_started){
        // std::cout << "starting new thread\n";
        std::thread read_thread( [this] { read_serial(); } );
        read_thread.detach();
    }
    
    return serial_started;
}

void SerialManager::close_serial(){
    std::lock_guard<std::mutex> lock(mtx);
    if (serial_port.is_open()) serial_port.close();
    serial_started = false;
}

void SerialManager::reset_read(){
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "resetting read\n";
    curr_number_buff.clear();
    curr_line_buff.clear();                              
    curr_data.clear();
    gui->PrintBuffer.clear();
    gui->all_data.clear();       
    gui->mutexed_all_data.clear();
    
    read_once = false;
}

void SerialManager::read_serial(){
    while(serial_started){
        int BytesRead = 0;
        bool line_done = false;
        try{
            {
                // std::cout << "trying to read\n";
                std::lock_guard<std::mutex> lock(mtx);
                BytesRead = serial_port.receive_data(message, packet_size);
            }
            
            if(BytesRead > 0 ){
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
                            mahi::util::Clock timing_clock;
                            curr_data.push_back(std::stof(curr_number_buff));
                            {
                                std::lock_guard<std::mutex> lock(mtx);
                                gui->PrintBuffer.push_back(curr_line_buff);
                            }
                            gui->append_all_data(curr_data);
                            curr_number_buff.clear();
                            curr_line_buff.clear();
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
        }
        catch(const std::exception& e){
            std::cerr << e.what() << '\n';
        }
    }
}

std::string SerialManager::get_port_name(BspPort port_num){
#if defined(WIN32)
    return "COM" + std::to_string(port_num + 1);
#elif defined(__APPLE__)
    return port_num;
#else
    if (port_num >= 16 && port_num <= 21){
        return "ttyUSB" + std::to_string(port_num - 16);
    } 
    else if (port_num >= 24 && port_num <= 25){
        return "ttyACM" + std::to_string(port_num - 24);
    }
#endif

}

bool SerialManager::comport_valid(){
#if defined(__APPLE__)
    return comport_num != "";
#else
    return comport_num > -1;
#endif
}

} // namespace bsp