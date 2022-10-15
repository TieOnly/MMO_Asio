#include "../../MMO_Src/network/tie_net.h"
#include "Utils.h"
#include <vector>
#include <map>

tie::net::message<GameMsg> tie::make::MakeMsgChat( const uint32_t& nPlayerID, const std::string& text )
{
    sPlayerText pMes;
    pMes.nUniqueID = nPlayerID;
    tie::make::MakeArrChar( pMes.chat, tie::def::ml_chat, text );

    tie::net::message<GameMsg> msg;
    msg.header.id = GameMsg::Game_MesStr;
    msg << pMes;

    return msg;
}
tie::net::message<GameMsg> tie::make::MM_HeaderID( const GameMsg& _id )
{
    tie::net::message<GameMsg> msg;
    msg.header.id = _id;
    return msg;
}

void tie::make::MakeArrChar( char *arr_dest, const int& arr_dest_limit, const std::string& str_src )
{
    std::memcpy( arr_dest, str_src.c_str(), arr_dest_limit );
}

std::map<uint32_t, bool> tie::util::RPSGameCheckRes( std::unordered_map< uint32_t, sPlayerRegisters >& mapPlayerRegister, int& cur_order )
{
    std::map<uint32_t, bool> vPlayerLose;
    std::vector<uint32_t> rocks;
    std::vector<uint32_t> papers;
    std::vector<uint32_t> scissors;

    for( auto& p : mapPlayerRegister )
    {
        if( p.second.order == cur_order )
        {
            vPlayerLose.insert( {p.first, false} );
            switch (p.second.oRPS)
            {
            case sRPSGame::Options::Rock: rocks.push_back(p.first); break;
            case sRPSGame::Options::Paper: papers.push_back(p.first); break;
            case sRPSGame::Options::Scissor: scissors.push_back(p.first); break;
            default: break;
            }
        }
    }

    int amount_rock = rocks.size();
    int amount_paper = papers.size();
    int amount_scissor = scissors.size();
    if( amount_rock != 0 && amount_paper != 0 && amount_scissor != 0 ) return vPlayerLose;
    else
    {
        if( amount_rock == 0 && amount_scissor != 0 )
        {
            for( auto& id : papers )
            {
                // mapPlayerRegister[id].isLose = true;
                vPlayerLose[id] = true;
            }
        }
        if( amount_paper == 0 && amount_rock != 0 )
        {
            for( auto& id : scissors )
            {
                // mapPlayerRegister[id].isLose = true;
                vPlayerLose[id] = true;
            }
        }
        if( amount_scissor == 0 && amount_paper != 0 )
        {
            for( auto& id : rocks )
            {
                // mapPlayerRegister[id].isLose = true;
                vPlayerLose[id] = true;
            }
        }
    }
    return vPlayerLose;
}

void tie::util::RPSGame_ProcessOrder( 
    std::unordered_map< uint32_t, sPlayerRegisters >& mapPlayerRegister, 
    std::map<uint32_t, bool>& mPlayerLoseThisTurn,
    int& cur_order, int& serch_order 
)
{
    if( cur_order == -1 ) cur_order = (int)mapPlayerRegister.size();
    std::cout << "Cur_order: " << cur_order << std::endl;
    std::cout << "Serch: " << serch_order << std::endl;
    int amountPlayerLose = 0;
    for( auto& p : mPlayerLoseThisTurn )
    {
        if( p.second )
        {
            amountPlayerLose++;
            mapPlayerRegister[p.first].order = cur_order;
            // std::cout << p.first << "(orderLose): " << mapPlayerRegister[p.first].order << std::endl;
        }
    }
    int amountSerchOrderExist = 0;
    for( auto& p : mPlayerLoseThisTurn )
    {
        if( !p.second )
        {
            mapPlayerRegister[p.first].order = cur_order - amountPlayerLose;
            // std::cout << p.first << "(orderWin): " << mapPlayerRegister[p.first].order << std::endl;
        }
        if( mapPlayerRegister[p.first].order == serch_order )
        {
            amountSerchOrderExist++;
        }
    }
    cur_order -= amountPlayerLose;

    if( amountSerchOrderExist == 1 )
    {
        serch_order++;
        int amountNextOrderExist = 0;
        if( cur_order < serch_order ) cur_order = serch_order;
        while( amountNextOrderExist == 0 )
        {
            for( auto& p : mapPlayerRegister )
            {
                if( p.second.order == cur_order )
                {
                    amountNextOrderExist++;
                }
            }
            if( amountNextOrderExist == 1 )
            {
                serch_order++;
                cur_order++;
                if( cur_order <= (int)mapPlayerRegister.size() ) amountNextOrderExist = 0;
            }
            else if( amountNextOrderExist < 1 )
            {
                cur_order++;
            }
        }
    }
}