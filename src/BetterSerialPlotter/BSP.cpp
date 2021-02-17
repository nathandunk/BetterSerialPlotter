#include <tchar.h>
#include <codecvt>
#include <locale>
#include <iostream>
#include <BetterSerialPlotter/BSP.hpp>
#include <BetterSerialPlotter/Utility.hpp>
#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>
#include <regex>

namespace bsp{

BSP::BSP(/* args */) : 
    mahi::gui::Application(),
    data_panel(this),
    serial_manager(this),
    plot_monitor(this),
    serial_monitor(this),
    PrintBuffer(200)
{
    all_plots.emplace_back();
    program_clock.restart();
    auto &app_colors = ImGui::GetStyle().Colors;
    // app_colors[2] = ImVec4(0.33f,0.35f,0.39f,1.0f); // whole window bg
    app_colors[ImGuiCol_ChildBg] = PalleteBlueGray; // child window bg
}

BSP::~BSP()
{
}

void BSP::update(){

    constexpr ImGuiWindowFlags padding_flag = ImGuiWindowFlags_AlwaysUseWindowPadding;

    time = static_cast<float>(program_clock.get_elapsed_time().as_seconds());
    ImGui::Begin("Better Serial Plotter", &open, padding_flag);

    data_panel.render();
    
    ImGui::SameLine();
    
    ImGui::BeginGroup();

    serial_manager.render();
    

    if (ImGui::BeginTabBar("MainAreaTabs")){
        if (ImGui::BeginTabItem("Plots")){
            plot_monitor.render();
        }
        if (ImGui::BeginTabItem("SerialMonitor")){
            serial_monitor.render();
        }
        ImGui::EndTabBar();
    }

    ImGui::EndGroup();
    
    ImGui::End();
    
    if(!open) quit();
    if (serial_started) read_serial();
}

bool BSP::begin_serial(){
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

void BSP::close_serial(){
    CloseHandle(hSerial);
}

void BSP::reset_read(){
    curr_number_buff.clear();
    curr_line_buff.clear();                              
    curr_data.clear();
    PrintBuffer.clear();
    all_data.clear();
    read_once = false;
}

void BSP::read_serial(){
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
                        && curr_number_buff.length() < 30){
                            curr_data.push_back(std::stof(curr_number_buff));
                            curr_number_buff = "";
                            if (verbose) std::cout << "\t";
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
                           && curr_number_buff.length() < 30){
                            curr_data.push_back(std::stof(curr_number_buff));
                            PrintBuffer.push_back(curr_line_buff);
                            curr_number_buff.clear();
                            curr_line_buff.clear();
                            append_all_data(curr_data);
                            if (verbose) std::cout << std::endl;
                            
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
                        if (verbose) std::cout << message[i];
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

void BSP::append_all_data(std::vector<float> curr_data){
    auto old_size = all_data.size();
    if (old_size != curr_data.size()){
        // ScrollingData NewScrollingData;
        all_data.resize(curr_data.size(),"data " + std::to_string(all_data.size()+1));
        for (auto i = old_size; i < all_data.size(); i++){
            all_data[i].set_name("data " + std::to_string(i));
            all_data[i].set_identifier(i);
            all_data[i].color = plot_colors[i%plot_colors.size()];
            std::cout << i%plot_colors.size() << std::endl;
        }
    }
    float curr_time = static_cast<float>(program_clock.get_elapsed_time().as_seconds());
    for (auto i = 0; i < curr_data.size(); i++){
        all_data[i].AddPoint(curr_time, curr_data[i]);
    }
}

} // namespace bsp