#include <BetterSerialPlotter/Utility.hpp>
#include <Mahi/Gui.hpp>

void plot_data(ScrollingData &data){
    ImPlot::PlotLine(data.name.c_str(), &data.Data[0].x, &data.Data[0].y, data.Data.size(), data.Offset, 2 * sizeof(float));  
}
