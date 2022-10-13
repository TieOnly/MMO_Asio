#pragma once

namespace tie
{
namespace make
{
    tie::net::message<GameMsg> MM_PR_ChatTyping( const uint32_t& nPlayerID );
    tie::net::message<GameMsg> MM_PR_Chat( const uint32_t& nPlayerID, const std::string& text );
    tie::net::message<GameMsg> MM_PR_RPSGame_Choose( const sRPSGame::Options& option );
    

    void MakeArrChar( char *arr_dest, const int& arr_dest_limit, const std::string& str_src );
}
class var
{
protected:
    float timerRPS = 0.0f;
    int count_order = 0;
};
}

struct sMapObjDesc
{
    RectF dest{};

    //Chat
    Vec2 pos_chat{};
    char chat[100];
    bool isTyping = false;
    float timerChat = 0.0f;

    //RPS Game
    sRPSGame::Options oRPS = sRPSGame::Options::GiveUp;
    bool isLose = false;
    int order = -1;
};
