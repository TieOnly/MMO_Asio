#include "../../MMO_Src/pch/raylibCpp.h"
#include "../../MMO_Src/network/tie_net.h"

#include "App.h"

void App::Draw()
{
    ClearBackground(BLACK);
    gui.Draw();
    if( GUI::GetLayer() == GUI::Layer::GameMode )
    {
        if( gui.GetGameMod().amount_player == GUI::Btn_SeqID::One_Player )
        {
            if( pField != nullptr ) pField->Draw();
            if(IsGameOver)
            {
                rayCpp::DrawStrCenter( "GAME OVER", rayCpp::GetScreenRect(), 50, ORANGE );
            }
            else if(IsWin)
            {
                rayCpp::DrawStrCenter( "WIN", rayCpp::GetScreenRect(), 50, ORANGE );
            }
        }
        else if( gui.GetGameMod().amount_player == GUI::Btn_SeqID::Two_Player )
        {
            rayCpp::DrawString( "List Player", Vec2{20.0f, 20.0f}, 30, WHITE );
            for( auto it = mapObjects.begin(); it != mapObjects.end(); it++ )
            {
                //Desc
                const sMapObjDesc& mo_desc = mapDescObjs[it->second.nUniqueID];
                //Theme
                Color c = PINK;
                if( it->second.nUniqueID == nPlayerID ) c = RED;
                
                //Name, info
                // const std::string name = it->second.name;
                rayCpp::DrawString( it->second.name, mo_desc.dest_name.topleft, 20, c );

                //Chat
                if( mo_desc.isTyping ) 
                {
                    rayCpp::DrawString( "...", mo_desc.pos_chat, 20, WHITE );
                    mapDescObjs[it->second.nUniqueID].isTyping = false;
                }
                else if( mo_desc.timerChat > 0.0f && mo_desc.timerNotify == 0.0f )
                {
                    rayCpp::DrawString( mo_desc.chat, mo_desc.pos_chat, 20, WHITE );
                }
                //Desc Notify
                if( mo_desc.timerNotify > 0.0f )
                {
                    rayCpp::DrawString( mo_desc.oRPS_str, mo_desc.pos_chat, 20, SKYBLUE );
                }
                //Ready State
                std::string text = (mo_desc.order == -1) ? "o" : std::to_string(mo_desc.order);
                if( it->second.isReady )
                {
                    rayCpp::DrawString( text, mo_desc.dest_name.topleft + Vec2{-18.0f, 0.0f }, 20, GREEN );
                }
                else
                {
                    rayCpp::DrawString( text, mo_desc.dest_name.topleft + Vec2{-18.0f, 0.0f }, 20, WHITE_TRANS );
                }
                if( tie::var::timerRPS > 5.0f )
                {
                    rayCpp::DrawStrCenter( "Let's go", rayCpp::GetScreenRect(), 50, ORANGE );
                }
            }
        }
    }
    if( bWaitingForConnect )
    {
        rayCpp::DrawString( "Conneting...", 
            Vec2{settings::screenW - 200.0f, 50.0f}, 20, WHITE
        );
    }
}