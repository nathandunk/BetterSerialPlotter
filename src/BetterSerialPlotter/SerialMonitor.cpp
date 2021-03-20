#include <BetterSerialPlotter/SerialMonitor.hpp>
#include <BetterSerialPlotter/BSP.hpp>

namespace bsp{

SerialMonitor::SerialMonitor(BSP* gui_): Widget(gui_){}

void SerialMonitor::render(){
    constexpr ImGuiWindowFlags serial_monitor_flags = ImGuiWindowFlags_HorizontalScrollbar;
    ImGui::Checkbox("Auto-Scroll",&auto_scroll);
    ImGui::BeginChild("Serial Monitor", ImVec2(-1, -1), true, serial_monitor_flags);
    for (size_t i = 0; i < gui->PrintBuffer.size(); i++){
        ImGui::Text(gui->PrintBuffer.get_vector()[i].c_str());
    }
    if (auto_scroll) ImGui::SetScrollY(ImGui::GetScrollMaxY());
    ImGui::EndChild();
}

} // namespace bsp