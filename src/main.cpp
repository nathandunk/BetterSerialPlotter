#include <iostream>
#include <BetterSerialPlotter/BSP.hpp>
#ifdef WIN32
    #include <Windows.h>
#endif

using namespace bsp;

// this makes it so that there is no window
#ifdef WIN32
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
#elif
int main()
#endif
{
    BSP bsp_app;
    bsp_app.run();
    return 0;
}