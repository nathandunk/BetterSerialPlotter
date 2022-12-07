#include <BetterSerialPlotter/SerialManager.hpp>
#include <BetterSerialPlotter/BSP.hpp>
#include <iostream>
#include <codecvt>
#include <regex>
#include <thread>

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
            // make sure custom baud isn't on for selection
            const bool baud_is_selected = (baud_rate == baud_rates[i]) && !use_custom_baud;
            if (ImGui::Selectable((std::to_string(baud_rates[i])).c_str(), baud_is_selected)){
                use_custom_baud = false;
                baud_rate = baud_rates[i];

                close_serial();
                begin_serial();
                reset_read();

            }
        }
        // extra option for custom baud rate
        if (ImGui::Selectable(("Custom: " + std::to_string(custom_baud)).c_str(), use_custom_baud)){
            use_custom_baud = true;
            baud_rate = custom_baud;

            close_serial();
            begin_serial();
            reset_read();
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    // Only show this if the user has selected to use a custom baud rate
    if(use_custom_baud){
        ImGui::Text("Custom Baud: ");
        ImGui::SameLine();
        ImGui::PushItemWidth(150);
        ImGui::InputInt("##custom_baud", &custom_baud_input);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::PushItemWidth(200);
        // Button press is required to actually use the custom baud rate
        if(ImGui::Button("Set Custom Baud")){
            custom_baud = custom_baud_input;
            baud_rate = custom_baud;
            close_serial();
            begin_serial();
            reset_read();
        }
        ImGui::PopItemWidth();
        ImGui::SameLine();
    }
    if (ImGui::Button("Close Comport")){
        close_serial();
        reset_read();
    }

    ImGui::EndChild();
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
    curr_line_buff.clear();                              
    gui->PrintBuffer.clear();
    gui->all_data.clear();       
    gui->mutexed_all_data.clear();
    
    read_once = false;
}

// std::string SerialManager::get_port_name(BspPort port_num){
void SerialManager::read_serial(){
    while(serial_started){
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
}

void SerialManager::parse_buffer(unsigned char* buff, size_t buff_len){
    for (size_t i = 0; i < buff_len; i++){
        // if we got a newline character, (0x0a)
        if (buff[i] == 0x0a){
            // if we haven't run through once, just note that, and 
            // then return to make sure we have clean data
            if (!read_once){
                read_once = true;
                curr_line_buff.clear();
            }
            // if we have run through once, send the full line to be parsed
            else{
                std::vector<float> curr_data = parse_line(curr_line_buff);
                gui->append_all_data(curr_data);

                {
                    std::lock_guard<std::mutex> lock(mtx);
                    gui->PrintBuffer.push_back(curr_line_buff);
                }
                curr_line_buff.clear();
                if (gui->verbose) std::cout << std::endl;
            }
        }
        // otherwise, if we got a normal character then append it to the current line
        else if (read_once && buff[i] != 0x0d){
            curr_line_buff += buff[i];
            if (gui->verbose) std::cout << buff[i];
        }
        else if (buff[i] != 0x0d){
            curr_line_buff.clear();
            read_once = false;
        }
    }
}

std::vector<float> SerialManager::parse_line(std::string line){
    // std::cout << line << "\n";

    std::vector<float> curr_data;

    // if the number is valid, add it to a float vecotr representing the current number set
    // valid numbers include:
    // digits 0-9 which can be a part of any number
    // +/- to denote positive/negativeness
    // E/e to denote scientific notation

    // split by tab and space characters
    static const std::regex re_delims("[\t ]");                                  // regular expression for either tab or space
    static const std::regex re_fp_num("[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?"); // regular expression for floating point number

    std::sregex_token_iterator first{line.begin(), line.end(), re_delims, -1}, last; //the '-1' is what makes the regex split (-1 := what was not matched)
    std::vector<std::string> numbers{first, last};

    // iterate through the separated numbers
    for (const auto &number : numbers){
        if (std::regex_match(number,re_fp_num)){
            try{
                curr_data.push_back(std::stof(number));
            }
            catch(const std::exception &e){
                std::cerr << e.what();
            }
            baud_status = true;
        }
        else{
            // if (gui->verbose) std::cout << "invalid number: " << number << "\n";
        }
    }
    
    return curr_data;
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