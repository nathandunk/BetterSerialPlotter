#include <BetterSerialPlotter/SerialManager.hpp>
#include <BetterSerialPlotter/BSP.hpp>
#include <iostream>
#include <tchar.h>
#include <codecvt>
#include <regex>
// #include <locale>

namespace bsp{

SerialManager::SerialManager(BSP* gui_): Widget(gui_){}

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
                // make sure this isn't what we are already connected to
                // if(comport_num != port_names[i]) {
                    if (serial_started){
                        close_serial();
                    }
                    comport_num = port_names[i];
                    serial_started = true;
                    begin_serial();
                    reset_read();
                // }
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
                // make sure this isn't what we are already connected to
                // if(baud_rate != baud_rates[i]) {
                    baud_rate = baud_rates[i];
                    DCB dcbSerialParams = {0};
                    dcbSerialParams.DCBlength=sizeof(dcbSerialParams);

                    dcbSerialParams.BaudRate=baud_rate;
                    dcbSerialParams.ByteSize=8;
                    dcbSerialParams.StopBits=ONESTOPBIT;
                    dcbSerialParams.Parity=NOPARITY;
                    if(!SetCommState(hSerial, &dcbSerialParams)){
                        std::cout << "could not set com state" << std::endl;
                    }
                    reset_read();
                // }
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
    std::wstring com_prefix = L"\\\\.\\COM";
    std::wstring com_suffix = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(std::to_string(comport_num));
    std::wstring comID = com_prefix + com_suffix;

    hSerial = CreateFileW(comID.c_str(),
                            GENERIC_READ | GENERIC_WRITE,
                            0,
                            0,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            0);

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength=sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        std::cout << "Could not get com state" << std::endl;
        return false;
    }

    dcbSerialParams.BaudRate=CBR_9600;
    dcbSerialParams.ByteSize=8;
    dcbSerialParams.StopBits=ONESTOPBIT;
    dcbSerialParams.Parity=NOPARITY;
    if(!SetCommState(hSerial, &dcbSerialParams)){
        std::cout << "could not set com state" << std::endl;
        return false;
    }

    COMMTIMEOUTS timeouts={0};
    timeouts.ReadIntervalTimeout=MAXDWORD;
    timeouts.ReadTotalTimeoutConstant=0;
    timeouts.ReadTotalTimeoutMultiplier=0;
    
    if(!SetCommTimeouts(hSerial, &timeouts)){
        std::cout << "could not set timeouts" << std::endl;
        return false;
    }

    PurgeComm(hSerial,PURGE_TXABORT);
    PurgeComm(hSerial,PURGE_RXABORT);
    PurgeComm(hSerial,PURGE_RXCLEAR);
	PurgeComm(hSerial,PURGE_TXCLEAR);

    std::cout << "connected to comport " << comport_num << "!\n";
    return true;
}

void SerialManager::close_serial(){
    CloseHandle(hSerial);
}

void SerialManager::reset_read(){
    curr_number_buff.clear();
    curr_line_buff.clear();                              
    curr_data.clear();
    gui->PrintBuffer.clear();
    gui->all_data.clear();
    read_once = false;
}

void SerialManager::read_serial(){
    DWORD dwBytesRead = 0;
    bool line_done = false;
    try{
    
    do{
        if(!ReadFile(hSerial, message, packet_size, &dwBytesRead, NULL)){
            // std::cout << "error reading from serial" << std::endl;
        }
        else{
            if(dwBytesRead > 0 ){
                // std::cout << "bytesread: " << dwBytesRead << "\n";
                // GETTING TO NEWLINE BEFORE DOING ANYTHING
                for (size_t i = 0; i < dwBytesRead; i++){
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
        }
    } while (!line_done && dwBytesRead > 0);
    }
    catch(const std::exception& e){
        std::cerr << e.what() << '\n';
        std::cout << curr_number_buff << std::endl;
    }

}

} // namespace bsp