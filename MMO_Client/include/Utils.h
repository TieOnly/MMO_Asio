#pragma once
#include "FrameTime.h"

#define FLOAT_0 0.0f

struct sMapObjDesc
{
    RectF dest_name{};

    //Chat
    Vec2 pos_chat{};
    char chat[100];
    bool isTyping = false;
    float timerChat = 0.0f;

    //RPS Game
    sRPSGame::Options oRPS = sRPSGame::Options::GiveUp;
    std::string oRPS_str = "Give Up";
    float timerNotify = 0.0f;
    bool isLose = false;
    int order = -1;
};

namespace tie
{
namespace make
{
    tie::net::message<GameMsg> MM_PR_ChatTyping( const uint32_t& nPlayerID );
    tie::net::message<GameMsg> MM_PR_Chat( const uint32_t& nPlayerID, const std::string& text );
    tie::net::message<GameMsg> MM_PR_RPSGame_Choose( const sRPSGame::Options& option );
    
    void MakeArrChar( char *arr_dest, const int& arr_dest_limit, const std::string& str_src );
    void MakeDescObjDest( 
        std::unordered_map< uint32_t, sPlayerDescription >& mapObjects,
        std::unordered_map< uint32_t, sMapObjDesc >& mapDescObjs
    );
}
class var
{
protected:
    float timerRPS = 0.0f;
    int count_order = 0;
    int cur_order = -1;
};
namespace utils
{
    template <typename _Func, typename... _Args>
    static void AddThread(_Func &&func, _Args &&...args)
    {
        std::thread newThread(std::forward<_Func>(func), std::forward<_Args>(args)...);
        if (newThread.joinable()) newThread.detach();
    }

    template<typename... _Args>
    static void MakeLoop( int fps, void (*func)(_Args...), _Args&&... args )
    {
        static FrameTime ft_ml;
        auto next_frame = std::chrono::steady_clock::now();
        while (true)
        {
            next_frame += std::chrono::microseconds(1000000 / fps);
            
            func( std::forward<_Args>(args)... );

            std::this_thread::sleep_until(next_frame);
        }
    }

    void TimerThread(
        std::unordered_map< uint32_t, sPlayerDescription >& mapObjects,
        std::unordered_map< uint32_t, sMapObjDesc >& mapDescObjs,
        float& timerRPS
    );

    void CountDown( float& dura );
}
}
