#include <BetterSerialPlotter/Plot.hpp>
#include <Mahi/Gui.hpp>

namespace bsp{

Plot::Plot(/* args */)
{
}

void Plot::plot_data(){
    for (auto i = 0; i < all_plot_data.size(); i++)
    {
        char id[64];
        sprintf(id,"%s###%i",all_plot_data[i].get().name.c_str(),i);
        ImPlot::PlotLine(id, &all_plot_data[i].get().Data[0].x, &all_plot_data[i].get().Data[0].y, all_plot_data[i].get().Data.size(), all_plot_data[i].get().Offset, 2 * sizeof(float));  
    }    
}

}