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

    auto num_data = all_data.size();
    time = static_cast<float>(program_clock.get_elapsed_time().as_seconds());
    ImGui::Begin("Better Serial Plotter", &open, padding_flag);
    
    {
        ImGui::BeginChild("IncomingData", ImVec2(200, -1), false, padding_flag);
        ImGui::Text("Incoming Data");
        ImGui::Separator();
        if (ImGui::BeginTable("data_table", 2, ImGuiTableFlags_Resizable) && baud_status){
            for (int i = 0; i < num_data; ++i) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Selectable(all_data[i].name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns);
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptBeforeDelivery)) {
                    ImGui::SetDragDropPayload("DND_PLOT", &i, sizeof(int));
                    ImGui::TextUnformatted(all_data[i].name.c_str());
                    ImGui::EndDragDropSource();
                }
                static bool editing = false;
                if (ImGui::BeginPopupContextItem()){
                    static char name[16];
                    if(!editing){
                        strcpy(name,all_data[i].name.c_str());
                    }
                    editing = true;
                    ImGui::Text("Edit name:");
                    ImGui::SameLine();
                    ImGui::PushItemWidth(120);
                    ImGui::InputText(("##edit" + std::to_string(i)).c_str(), name, IM_ARRAYSIZE(name));
                    ImGui::PopItemWidth();
                    ImGui::Text("Edit color:");
                    ImGui::SameLine();
                    ImGui::ColorEdit4("##Color", (float*)&all_data[i].color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                    if (ImGui::Button("Save") || (ImGui::IsKeyPressed(257)) || (ImGui::IsKeyPressed(335))){ // would change for mac/linux
                        all_data[i].set_name(name);
                        ImGui::CloseCurrentPopup();
                        editing = false;
                    }
                    ImGui::EndPopup();
                }
                ImGui::TableNextColumn();
                // ImGui::SameLine();
                char buff[16];
                sprintf(buff, "%4.3f", all_data[i].get_back().y);
                ImGui::Text(buff);
            }
            ImGui::EndTable();
        }
        
        ImGui::EndChild();
    }
    
    ImGui::SameLine();
    
    ImGui::BeginGroup();
    {
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (comport_num > 0) ? (serial_status ? PalleteGreen : PalleteRed) : PalleteGray);
        ImGui::PushStyleColor(ImGuiCol_Header, (comport_num > 0) ? (serial_status ? PalleteGreen : PalleteRed) : PalleteGray);
        ImGui::BeginChild("SerialSetup", ImVec2(-1, 36), false, padding_flag);
        // comport selection
        ImGui::PushItemWidth(200);
        if (ImGui::BeginCombo("##comport_select", ("COM: " + ((comport_num >= 0) ? std::to_string(comport_num) : "")).c_str())){
            std::vector<int> port_names = get_serial_ports();
            for (int i = 0; i < port_names.size(); i++){
                const bool com_is_selected = (comport_num == port_names[i]);
                if (ImGui::Selectable(("COM"+std::to_string(port_names[i])).c_str(), com_is_selected)){
                    // make sure this isn't what we are already connected to
                    if(comport_num != port_names[i]) {
                        if (serial_started){
                            close_serial();
                        }
                        comport_num = port_names[i];
                        serial_started = true;
                        begin_serial();
                        reset_read();
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
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (baud_rate > 0) ? (baud_status ? PalleteGreen : PalleteRed) : PalleteGray);
        ImGui::PushStyleColor(ImGuiCol_Header, (baud_rate > 0) ? (baud_status ? PalleteGreen : PalleteRed) : PalleteGray);
        if (ImGui::BeginCombo("##baud_rate", ("Baud Rate: " + ((baud_rate >= 0) ? std::to_string(baud_rate) : "")).c_str())){
            for (int i = 0; i < baud_rates.size(); i++){
                const bool baud_is_selected = (baud_rate == baud_rates[i]);
                if (ImGui::Selectable(("COM"+std::to_string(baud_rates[i])).c_str(), baud_is_selected)){
                    // make sure this isn't what we are already connected to
                    if(baud_rate != baud_rates[i]) {
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

    if (ImGui::BeginTabBar("MainAreaTabs")){
        if (ImGui::BeginTabItem("Plots")){
            for (auto i = 0; i < all_plots.size(); i++){
                all_plots[i].make_plot(time, i, all_data);
            }
            ImGui::PushStyleColor(ImGuiCol_Button,PalleteBlue);
            if(ImGui::Button("Add Plot")) {
                all_plots.emplace_back();
            }
            ImGui::PopStyleColor();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("SerialMonitor")){
            constexpr ImGuiWindowFlags serial_monitor_flags = ImGuiWindowFlags_HorizontalScrollbar;
            ImGui::Checkbox("Auto-Scroll",&auto_scroll);
            ImGui::BeginChild("Serial Monitor", ImVec2(-1, 586), true, serial_monitor_flags);
            for (size_t i = 0; i < PrintBuffer.size(); i++){
                ImGui::Text(PrintBuffer.get_vector()[i].c_str());
            }
            if (auto_scroll) ImGui::SetScrollY(ImGui::GetScrollMaxY());
            ImGui::EndChild();
            ImGui::EndTabItem();
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