#include <tchar.h>
#include <codecvt>
#include <locale>
#include <iostream>
#include <BetterSerialPlotter/BSP.hpp>
#include <BetterSerialPlotter/Utility.hpp>
#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>

namespace bsp{

BSP::BSP(/* args */) : 
    mahi::gui::Application(),
    PrintBuffer(10)
{
    all_plots.emplace_back();
    program_clock.restart();
}

BSP::~BSP()
{
}

void BSP::update(){
    auto num_data = all_data.size();
    time = static_cast<float>(program_clock.get_elapsed_time().as_seconds());
    ImGui::Begin("Better Serial Plotter", &open);

    if (ImGui::TreeNode("Comport Selection:")){
        std::vector<int> port_names = get_serial_ports();
        static int selected = -1;
        for (int n = 0; n < port_names.size(); n++)
        {
            if (ImGui::Selectable(("COM"+std::to_string(port_names[n])).c_str(), selected == n)){
                if(comport_num != port_names[n]) {
                    if (serial_started){
                        //close_serial();
                    }
                    comport_num = port_names[n];
                    serial_started = true;
                    begin_serial();
                }
                selected = n;
            }
        }
        ImGui::TreePop();
    }
    
    // drag and droppable notes
    for (int i = 0; i < num_data; ++i) {
        ImGui::Selectable(all_data[i].name.c_str(), false, 0, ImVec2(100, 0));
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptBeforeDelivery)) {// try ImGuiDragDropFlags_AcceptBeforeDelivery
            ImGui::SetDragDropPayload("DND_PLOT", &i, sizeof(int));
            ImGui::TextUnformatted(all_data[i].name.c_str());
            ImGui::EndDragDropSource();
        }
        static bool editing = false;
        if (ImGui::BeginPopupContextItem())
        {
            static char name[32];
            if(!editing){
                strcpy(name,all_data[i].name.c_str());
            }
            editing = true;
            ImGui::Text("Edit name:");
            ImGui::InputText(("##edit" + std::to_string(i)).c_str(), name, IM_ARRAYSIZE(name));
            if (ImGui::Button("Save")){
                all_data[i].set_name(name);
                ImGui::CloseCurrentPopup();
                editing = false;
            }
            ImGui::EndPopup();
        }
        ImGui::SameLine();
        ImGui::InputFloat(("##"+all_data[i].name).c_str(),&all_data[i].get_back().y);
        // std::cout << all_data[i].Data.back().y << std::endl;
    }
    // std::cout << std::endl;
    
    for (auto i = 0; i < all_plots.size(); i++){
        all_plots[i].make_plot(time, i, all_data);
    }

    if(ImGui::Button("Add Plot")) {
        all_plots.emplace_back();
    }
    
    ImGui::End();
    
    if(!open) quit();
    if (serial_started) read_serial();
    // std::cout << "after serial";
}

void BSP::begin_serial(){
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
    }

    dcbSerialParams.BaudRate=CBR_9600;
    dcbSerialParams.ByteSize=8;
    dcbSerialParams.StopBits=ONESTOPBIT;
    dcbSerialParams.Parity=NOPARITY;
    if(!SetCommState(hSerial, &dcbSerialParams)){
        std::cout << "could not set com state" << std::endl;
    }

    COMMTIMEOUTS timeouts={0};
    timeouts.ReadIntervalTimeout=MAXDWORD;
    timeouts.ReadTotalTimeoutConstant=0;
    timeouts.ReadTotalTimeoutMultiplier=0;
    
    if(!SetCommTimeouts(hSerial, &timeouts)){
        std::cout << "could not set timeouts" << std::endl;
    }

    PurgeComm(hSerial,PURGE_TXABORT);
    PurgeComm(hSerial,PURGE_RXABORT);
    PurgeComm(hSerial,PURGE_RXCLEAR);
	PurgeComm(hSerial,PURGE_TXCLEAR);
}

void BSP::read_serial(){
    DWORD dwBytesRead = 0;
    bool line_done = false;
    do{
        if(!ReadFile(hSerial, message, packet_size, &dwBytesRead, NULL)){
            // std::cout << "error reading from serial" << std::endl;
        }
        else{
            if(dwBytesRead > 0 ){
                    // std::cout << "bytesread: " << dwBytesRead << "\n";
                    // GETTING TO NEWLINE BEFORE DOING ANYTHING
                    for (size_t i = 0; i < dwBytesRead; i++){
                        // std::cout << "int: " << (int)message[i] << "\n";
                        // std::cout << "char: " << message[i] << "\n";
                        // if tab (0x09) or space (0x20)
                        if ((message[i] == 0x09 || message[i] == 0x20) && read_once){
                            curr_data.push_back(std::stof(curr_number_buff));
                            curr_number_buff = "";
                            if (verbose) std::cout << "\t";
                        }
                        // if new line
                        else if (message[i] == 0x0a && read_once){
                            curr_data.push_back(std::stof(curr_number_buff));
                            curr_number_buff = "";
                            append_all_data(curr_data);
                            // std::cout << std::endl << "size: " << curr_data.size();
                            // for (auto &&i : curr_data)
                            // {
                            //     std::cout << i;
                            // }
                            if (verbose) std::cout << std::endl;
                            
                            curr_data.clear();
                            line_done = true;
                            read_once = true;
                        }
                        else if ((message[i] >= '0' && message[i] <= '9' || message[i] == '.' || message[i] == '-') && read_once){
                            curr_number_buff += message[i];
                            if (verbose) std::cout << message[i];
                        }
                        else if (message[i] != 0x0d && read_once) {
                            std::cout << "invalid character: " << message[i] << std::endl;
                        }
                        else if (message[i] == 0x0a){
                            read_once = true;
                        }
                }
            }
        }
    } while (!line_done && dwBytesRead > 0);
    
    // if (!output.empty()) PrintBuffer.push_back(output);
}

void BSP::append_all_data(std::vector<float> curr_data){
    auto old_size = all_data.size();
    if (old_size != curr_data.size()){
        // ScrollingData NewScrollingData;
        all_data.resize(curr_data.size(),"data " + std::to_string(all_data.size()+1));
        for (auto i = old_size; i < all_data.size(); i++){
            all_data[i].set_name("data " + std::to_string(i));
            all_data[i].set_identifier(i);
        }
        
    }
    float curr_time = static_cast<float>(program_clock.get_elapsed_time().as_seconds());
    for (auto i = 0; i < curr_data.size(); i++){
        all_data[i].AddPoint(curr_time, curr_data[i]);
    }
}

} // namespace bsp