#pragma once

#include <BetterSerialPlotter/Utility.hpp>

namespace bsp{

class Plot
{
private:
    /* data */
public:
    Plot(/* args */);
    void plot_data();
    std::vector<std::reference_wrapper<ScrollingData>> all_plot_data;
};
    
} // namespace bsp