#include "../../MMO_Src/pch/raylibCpp.h"
#include "../../MMO_Src/network/tie_net.h"

#include "Utils.h"

tie::net::message<GameMsg> tie::make::MM_PR_ChatTyping( const uint32_t& nPlayerID )
{
    tie::net::message<GameMsg> msg;
    msg.header.id = GameMsg::PR_SendChatTyping;
    msg << nPlayerID;

    return msg;
}
tie::net::message<GameMsg> tie::make::MM_PR_Chat( const uint32_t& nPlayerID, const std::string& text )
{
    sPlayerText pMes;
    pMes.nUniqueID = nPlayerID;
    pMes.timerChat = 5.0f;
    tie::make::MakeArrChar( pMes.chat, tie::def::ml_chat, text );

    tie::net::message<GameMsg> msg;
    msg.header.id = GameMsg::PR_SendChat;
    msg << pMes;

    return msg;
}
tie::net::message<GameMsg> tie::make:: MM_PR_RPSGame_Choose( const sRPSGame::Options& option )
{
    tie::net::message<GameMsg> msg;
    msg.header.id = GameMsg::PR_RPSGame_Choose;
    msg << option;

    return msg;
}

void tie::make::MakeArrChar( char *arr_dest, const int& arr_dest_limit, const std::string& str_src )
{
    std::memcpy( arr_dest, str_src.c_str(), arr_dest_limit );
}

