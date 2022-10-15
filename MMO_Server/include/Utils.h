#pragma once
#include "TieThread.h"
#include <map>

struct sPlayerRegisters
{
	char name[ML_NAME];
    sRPSGame::Options oRPS = sRPSGame::Options::GiveUp;
    bool isLose = false;
    int order = -1;
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
    bool isFisrtRPSGame = true;
    int serch_order = 1;
    int cur_order = -1;
};
namespace util
{
    std::map<uint32_t, bool> RPSGameCheckRes( std::unordered_map< uint32_t, sPlayerRegisters >& mapPlayerRegister, int& cur_order );
    void RPSGame_ProcessOrder( 
        std::unordered_map< uint32_t, sPlayerRegisters >& mapPlayerRegister, 
        std::map<uint32_t, bool>& mPlayerLoseThisTurn,
        int& cur_order, int& serch_order 
    );
}
}

