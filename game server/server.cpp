#pragma once

#include "MainIOCP.h"

int main()
{
    MainIOCP mainIOCP;;

    if (mainIOCP.Initialize())
    {
        mainIOCP.ServerStart();
    }
    return 0;
}
