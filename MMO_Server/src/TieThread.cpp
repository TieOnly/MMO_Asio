#include "../../MMO_Src/network/tie_net.h"
#include "TieThread.h"

void tie::thread::CountDown( int& dura )
{
    int count = 0;
    int count_max = dura;
    bool finished = false;
    while( !finished )
    {
        std::cout << "Working..." << dura << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        count++;
        dura--;
        if( count >= count_max )
        {
            std::cout << "Finished" << std::endl;
            finished = true;
        }
    }
}