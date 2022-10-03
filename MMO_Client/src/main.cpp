#include "../../MMO_Src/pch/raylibCpp.h"
#include "../../MMO_Src/network/tie_net.h"
#include "../include/App.h"

int main()
{
    assert(!GetWindowHandle());
    InitWindow(settings::screenW, settings::screenH, "CLIENT");
    InitAudioDevice();

    App app(settings::fps);

    while(!app.AppShouldClose())
    {
        app.Tick();
    }

    return 0;
}

