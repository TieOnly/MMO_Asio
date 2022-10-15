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
void tie::make::MakeDescObjDest(
    std::unordered_map< uint32_t, sPlayerDescription >& mapObjects,
    std::unordered_map< uint32_t, sMapObjDesc >& mapDescObjs
)
{
    int i = 1;
    for( auto it = mapDescObjs.begin(); it != mapDescObjs.end(); it++, i++ )
    {
        it->second.dest_name = RectF{ 
            Vec2{ 40.0f, 50.0f + i*30.0f },
            (float)MeasureText( mapObjects[it->first].name, 20 ), 20.0f
        };
        it->second.pos_chat = Vec2{ it->second.dest_name.right + 20.0f, it->second.dest_name.top };
    }
}

void tie::utils::TimerThread(
    std::unordered_map< uint32_t, sPlayerDescription >& mapObjects,
    std::unordered_map< uint32_t, sMapObjDesc >& mapDescObjs,
    float& timerRPS
)
{
    static FrameTime ft;
    float dTime = ft.Mark();

    // static float dura_fps = 0.0f;
    // dura_fps += dTime;
    // if( dura_fps >= 1.0f )
    // {
    //     std::cout << "For 1s: " << dura_fps << std::endl;
    //     dura_fps = 0.0f;
    // }

    if( timerRPS > 0.0f )
    {
        timerRPS -= dTime;
        if( timerRPS < 0.0f ) timerRPS = 0.0f;
    }

    for( auto& p : mapDescObjs )
    {
        if( p.second.timerChat > 0.0f ) 
        {
            p.second.timerChat -= dTime; 
            if( p.second.timerChat < 0.0f ) p.second.timerChat = 0.0f;
        }
        if( p.second.timerNotify > 0.0f ) 
        {
            p.second.timerNotify -= dTime; 
            if( p.second.timerNotify < 0.0f ) p.second.timerNotify = 0.0f;
        }
    }
} 

void tie::utils::CountDown( float& dura )
{
    float count = 0.0f;
    float count_max = dura;
    bool finished = false;
    while( !finished )
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        count += 1.0f;
        dura -= 1.0f;
        if( count >= count_max )
        {
            finished = true;
        }
    }
}
