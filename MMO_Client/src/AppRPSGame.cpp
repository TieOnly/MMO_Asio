#include "../../MMO_Src/pch/raylibCpp.h"
#include "../../MMO_Src/network/tie_net.h"

#include "App.h"

void App::IPMT_GM_Server_RPS_TimeUp( tie::net::message<GameMsg>& msg )
{
    std::cout << "Time Up!" << std::endl;

    tie::var::timerRPS = 0.0f;
    gui.rpsGame.ResetSub();
}
void App::IPMT_GM_RPSGame( tie::net::message<GameMsg>& msg )
{
    sRPSGame rps;
    msg >> rps;
    if( rps.isAbleStart )
    {
        if( mapDescObjs[nPlayerID].order == tie::var::cur_order )
        {
            tie::var::timerRPS = rps.countdown;
        }
        gui.buttons[(int)GUI::Btn_SeqID::ReadyState].isAbleActive = false;
        gui.rpsGame.StyleAllBtnNormal();
    }
    else if( !rps.timeup )
    {
        if( rps.isAnyOneNotReady ) std::cout << "Anyone is not ready" << std::endl;
        else std::cout << "Not enough player" << std::endl;
    }
    else
    {
        tie::var::cur_order = rps.cur_order;
        if( mapDescObjs.count( rps.owner_id ) )
        {
            mapDescObjs[rps.owner_id].oRPS = rps.option;
            mapDescObjs[rps.owner_id].order = rps.order;
            mapDescObjs[rps.owner_id].timerNotify = 5.0f;

            std::cout << mapObjects[rps.owner_id].name << "Now order: " << mapDescObjs[rps.owner_id].order << '\n';
        }
        else std::cout << "Not exist this player: " << rps.owner_id << '\n';
    }
}
void App::IPMT_GM_Server_RPS_DoneUpdate( tie::net::message<GameMsg>& msg )
{
    
    if( mapDescObjs[nPlayerID].order == tie::var::cur_order ) 
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
        std::cout << mapObjects[p.first].name << ": choose " << choice << " - " << mapDescObjs[p.first].order << '\n';
        p.second.oRPS_str = std::move(choice);
    }
}
// void App::IPMT_GM_Server_RPS_AssignOrder( tie::net::message<GameMsg>& msg )
// {
//     uint32_t nIDPlayer;
//     msg >> nIDPlayer;
//     if( mapObjects.count(nIDPlayer) )
//     {
//         msg >> mapDescObjs[nIDPlayer].order;
//         std::cout << mapObjects[nIDPlayer].name << " has new order: " << mapDescObjs[nIDPlayer].order << std::endl;
//     }
// }
// void App::IPMT_GM_Server_RPS_IDPlayer_Lose( tie::net::message<GameMsg>& msg )
// {
//     uint32_t nIDPlayerLose;
//     msg >> nIDPlayerLose;
//     if( mapObjects.count( nIDPlayerLose ) )
//     {
//         msg >> tie::var::cur_order >> mapDescObjs[nIDPlayerLose].order;
//         mapDescObjs[nIDPlayerLose].isLose = true;
//         std::cout << mapObjects[nIDPlayerLose].name << " LOSED this rps game with order: " << mapDescObjs[nIDPlayerLose].order << std::endl;
//         if( nIDPlayerLose == nPlayerID )
//         {
//             if( !mapObjects[nPlayerID].isHost )
//             {
//                 gui.Btn_UpdateReadyState(true);
//                 gui.buttons[(int)GUI::Btn_SeqID::ReadyState].isAbleActive = false;
//             }
//         }
//     }
// }
// void App::IPMT_GM_Server_RPS_IDPlayer_Win( tie::net::message<GameMsg>& msg )
// {
//     uint32_t nIDPlayerLose;
//     msg >> nIDPlayerLose;
//     if( mapObjects.count( nIDPlayerLose ) )
//     {
//         msg >> tie::var::cur_order >> mapDescObjs[nIDPlayerLose].order;
//         std::cout << mapObjects[nIDPlayerLose].name << " WON this rps game with order: " << mapDescObjs[nIDPlayerLose].order << std::endl;
//     }
// }