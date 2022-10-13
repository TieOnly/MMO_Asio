#pragma once
#include "TieThread.h"
#include <map>

struct sPlayerRegisters
{
	char name[ML_NAME];
    sRPSGame::Options oRPS = sRPSGame::Options::GiveUp;
    bool isLose = false;
};

namespace tie
{
namespace make
{
    tie::net::message<GameMsg> MakeMsgChat( const uint32_t& nPlayerID, const std::string& text );
    tie::net::message<GameMsg> MM_HeaderID( const GameMsg& id );

    void MakeArrChar( char *arr_dest, const int& arr_dest_limit, const std::string& str_src );
}
class var
{
public:
    uint32_t IDHost = -1u;
    float timerRPS = 0.0f;
    bool timeUpRPS = true;
    bool RPSGame_isEven = false;
};
namespace util
{
    std::map<uint32_t, bool> RPSGameCheckRes( std::unordered_map< uint32_t, sPlayerRegisters >& mapPlayerRegister );
}
}

