#pragma once

namespace tie
{
namespace make
{
    tie::net::message<GameMsg> MM_PR_ChatTyping( const uint32_t& nPlayerID );
    tie::net::message<GameMsg> MM_PR_Chat( const uint32_t& nPlayerID, const std::string& text );
    
    void MakeArrChar( char *arr_dest, const int& arr_dest_limit, const std::string& str_src );
}
}

struct sMapObjDesc
{
    RectF dest{};
    Vec2 pos_chat{};
    char chat[100];
    bool isTyping = false;
    float timerChat = 0.0f;
};
