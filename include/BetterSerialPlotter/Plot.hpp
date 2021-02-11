#pragma once

#include <BetterSerialPlotter/Utility.hpp>

namespace bsp{

class Plot
{
private:
    /* data */
public:
    Plot(/* args */);
    void make_plot(float time, int plot_num, std::vector<ScrollingData> &all_data);
    void plot_data();
    bool has_identifier(char identifier);
    std::vector<std::reference_wrapper<ScrollingData>> all_plot_data;
};
    
} // namespace bsp