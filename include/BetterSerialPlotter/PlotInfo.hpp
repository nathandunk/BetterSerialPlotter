#include <vector>

struct PlotInfo
{
    std::vector<char> data_ids;
    char x_axis;
    std::unordered_map<char,int> y_axis;
};
