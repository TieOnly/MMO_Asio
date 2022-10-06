#include "../../MMO_Src/network/tie_net.h"
#include "Utils.h"

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
void tie::make::MakeArrChar( char *arr_dest, const int& arr_dest_limit, const std::string& str_src )
{
    std::memcpy( arr_dest, str_src.c_str(), arr_dest_limit );
}
