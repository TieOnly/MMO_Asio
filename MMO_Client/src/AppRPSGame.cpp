#include "../../MMO_Src/pch/raylibCpp.h"
#include "../../MMO_Src/network/tie_net.h"

#include "App.h"

void App::IPMT_GM_RPSGame( tie::net::message<GameMsg>& msg )
{
    sRPSGame rps;
    msg >> rps;
    if( rps.isAbleStart )
    {
        std::cout << "Start! Choose your option in 5s" << std::endl;
        tie::var::timerRPS = rps.countdown;
        gui.buttons[(int)GUI::Btn_SeqID::ReadyState].isAbleActive = false;
    }
    else if( !rps.timeup )
    {
        if( rps.isAnyOneNotReady ) std::cout << "Anyone is not ready" << std::endl;
        else std::cout << "Not enough player" << std::endl;
    }
    else
    {
        tie::var::timerRPS = 0.0f;
        if( mapDescObjs.count( rps.owner_id ) )
        {
            mapDescObjs[rps.owner_id].oRPS = rps.option;
            std::cout << "Update Every Descript(choose): "<< mapObjects[rps.owner_id].name << " choose " << (int)rps.option << std::endl;
        }
        else std::cout << "Not exist this player: " << rps.owner_id << '\n';
    }
}

void App::IPMT_GM_Server_RPS_DoneUpdate( tie::net::message<GameMsg>& msg )
{
    std::cout << "Time Up!" << std::endl;
    
    if( !mapDescObjs[nPlayerID].isLose ) 
        gui.buttons[(int)GUI::Btn_SeqID::ReadyState].isAbleActive = true;
    
    std::cout << "===========Result===========\n";
    for( auto& p : mapDescObjs )
    {
        std::string choice = "";
        switch (p.second.oRPS)
        {
        case sRPSGame::Options::Rock: choice = "Rock"; break;
        case sRPSGame::Options::Paper: choice = "Paper"; break;
        case sRPSGame::Options::Scissor: choice = "Scissor"; break;
        default: choice = "Give Up"; break;
        }
        std::cout << mapObjects[p.first].name << ": choose " << choice << '\n';
    }
}
void App::IPMT_GM_Server_RPS_IDPlayer_Lose( tie::net::message<GameMsg>& msg )
{
    uint32_t nIDPlayerLose;
    msg >> nIDPlayerLose;
    if( mapObjects.count( nIDPlayerLose ) )
    {
        mapDescObjs[nIDPlayerLose].isLose = true;
        std::cout << mapObjects[nIDPlayerLose].name << " LOSED this rps game." << std::endl;
        if( nIDPlayerLose == nPlayerID )
        {
            if( !mapObjects[nPlayerID].isHost )
            {
                gui.Btn_UpdateReadyState(true);
                gui.buttons[(int)GUI::Btn_SeqID::ReadyState].isAbleActive = false;
            }
        }
    }
}
void App::IPMT_GM_Server_RPS_IDPlayer_Win( tie::net::message<GameMsg>& msg )
{
    uint32_t nIDPlayerLose;
    msg >> nIDPlayerLose;
    if( mapObjects.count( nIDPlayerLose ) )
    {
        std::cout << mapObjects[nIDPlayerLose].name << " WON this rps game." << std::endl;
    }
}