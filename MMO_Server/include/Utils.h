#pragma once
#include "TieThread.h"

namespace tie
{
namespace make
{
    tie::net::message<GameMsg> MakeMsgChat( const uint32_t& nPlayerID, const std::string& text );
    
    void MakeArrChar( char *arr_dest, const int& arr_dest_limit, const std::string& str_src );
}
class var
{
public:
    uint32_t IDHost = -1u;
    float timerRPS = 0.0f;
    bool timeUpRPS = true;
};
}

struct sPlayerRegisters
{
    bool isChoosedStateRPS = false;
    sRPSGame::Options oRPS = sRPSGame::Options::GiveUp;
};
