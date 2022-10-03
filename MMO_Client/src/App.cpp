#include "../../MMO_Src/pch/raylibCpp.h"
#include "../../MMO_Src/network/tie_net.h"

#include "App.h"

App::App(const int fps)
{
    SetTargetFPS(fps);

    gui.LoadAsset();
}
App::~App() noexcept
{
    if( gui.GetGameMod().amount_player == GUI::Btn_SeqID::Two_Player )
    {
        tie::net::message<GameMsg> msgUnregis;
        msgUnregis.header.id = GameMsg::Client_UnregisterWithServer;
        Send( msgUnregis );
    }
    assert(GetWindowHandle());
    CloseWindow();
}
bool App::AppShouldClose() const
{
    return WindowShouldClose();
}
void App::Tick() 
{
    BeginDrawing();
    Update();
    Draw();
    EndDrawing();
}

void App::Update()
{
    gui.Update();

    if( GUI::GetLayer() == GUI::Layer::GameMode )
    {
        if( gui.GetGameMod().amount_player == GUI::Btn_SeqID::One_Player )
        {
            if( pField != nullptr ) OnePlayerGame();

            //Reset Game
            if( gui.GetGameMod().isReset )
            {
                DestroyField();
                gui.ResetGameMode();
                GUI::SetLayer( GUI::Layer::ChoseMode );
                IsGameOver = false;
                IsWin = false;
            }
        }
        else if( gui.GetGameMod().amount_player == GUI::Btn_SeqID::Two_Player )
        {
            TwoPlayerGame();
            if( gui.GetGameMod().isReset )
            {
                tie::net::message<GameMsg> msgUnregis;
                msgUnregis.header.id = GameMsg::Client_UnregisterWithServer;
                Send( msgUnregis );
                Disconnect();

                //Reset To ChoseMode
                gui.ResetGameMode();
                ResetPlayer();
                GUI::SetLayer( GUI::Layer::ChoseMode );
            }
        }

    }
    else if( gui.GetGameMod().amount_player != GUI::Btn_SeqID::Non )
    {
        if( gui.GetGameMod().amount_player == GUI::Btn_SeqID::One_Player )
        {
            //Create Game
            switch (gui.GetGameMod().game_size)
            {
            case GUI::Btn_SeqID::Small:
            {
                CreateField( 6, 6, 6 );
                GUI::SetLayer( GUI::Layer::GameMode );
            } break;
            case GUI::Btn_SeqID::Medium:
            {
                CreateField( 10, 10, 15 );
                GUI::SetLayer( GUI::Layer::GameMode );
            } break;
            case GUI::Btn_SeqID::Large:
            {
                CreateField( 15, 15, 20 );
                GUI::SetLayer( GUI::Layer::GameMode );
            } break;
            
            default: break;
            }
        }
        else if( gui.GetGameMod().amount_player == GUI::Btn_SeqID::Two_Player )
        {
            //Connect
            if( IsConnected() )
            {
                GUI::SetLayer( GUI::Layer::GameMode );
            }
            else
            {
                bWaitingForConnect = true;
                //100 message in MessageIncoming queue
                Connect( "192.168.19.107", 60000, 100 );
            }
        }
    }
}
void App::CreateField(const int width, const int height, const int nBooms)
{
    assert( pField == nullptr );
    pField = new MineField( width, height, nBooms );
}
void App::DestroyField()
{
    delete pField;
    pField = nullptr;
}
//========================1 Player=============================//
void App::OnePlayerGame()
{
    if(!IsGameOver && !IsWin)
    {
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            Vei2 mouseGridpos = pField->ScreenToGrid( Vei2{GetMouseX(), GetMouseY()} );
            pField->DoRevealedClick( mouseGridpos );
            if(pField->FuckUp())
            {
                IsGameOver = true;
            }
        }
        else if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            Vei2 mouseGridpos = pField->ScreenToGrid( {GetMouseX(), GetMouseY()} );
            pField->DoFlagClick( mouseGridpos );
        }
        
        if(pField->Done())
        {
            IsWin = true;
        }
    }
}
//========================2 Player=============================//
void App::TwoPlayerGame()
{
    //Flag for something be changed
    descPlayer.isUpdateWithEveryOne = false;

    //Update From Server
    if( !InComing().empty() )
    {
        auto msg = InComing().pop_front().msg;

        switch (msg.header.id)
        {
        case GameMsg::Client_Accepted:
        {
            std::cout << "DuyPro cho phep ban truy cap!" << std::endl;            
            tie::net::message<GameMsg> msg;
            msg.header.id = GameMsg::Client_RegisterWithServer;
            msg << descPlayer;
            Send( msg );
        } break;

        case GameMsg::Client_AssignID:
        {
            msg >> nPlayerID;
            std::cout << "DuyPro dat ten ID cua ban la: " << nPlayerID << std::endl;
        } break;
        
        case GameMsg::Game_AddPlayer:
        {
            sPlayerDescription desc;
            msg >> desc;
            mapObjects.insert_or_assign( desc.nUniqueID, desc );

            if( desc.nUniqueID == nPlayerID ) bWaitingForConnect = false;
        } break;

        case GameMsg::Game_RemovePlayer:
        {
            uint32_t nRemovalID = 0;
            msg >> nRemovalID;
            mapObjects.erase( nRemovalID );
        } break;

        case GameMsg::Game_UpdatePlayer:
        {
            sPlayerDescription desc;
            msg >> desc;
            mapObjects.insert_or_assign( desc.nUniqueID, desc );
            for( auto& p : mapObjects )
            {
                if( p.second.chat != nullptr )
                {
                    std::cout << p.second.name << ": " << p.second.chat << std::endl;
                }
            }
        } break;

        default: break;
        }
    }

    //Update Game
    if( !gui.GetGameMod().input_value.empty() )
    {
        descPlayer.isUpdateWithEveryOne = true;

        // if( (int)gui.GetGameMod().input_value.size() <= 20 )
        // {
        //     std::memcpy( descPlayer.name, &gui.GetGameMod().input_value, sizeof(gui.GetGameMod().input_value.size()) );
        // }
        gui.ClearInputValue();
    }
    if( IsKeyPressed(KEY_A) )
    {
        descPlayer.isUpdateWithEveryOne = true;

        mapObjects[nPlayerID].countPress++;
        std::string text = "Player";
        for( int i = 0; i < (int)text.size(); i++ )
        {
            mapObjects[nPlayerID].name[i] = text[i];
        }
        std::string text_1 = "Hello There";
        std::memcpy( mapObjects[nPlayerID].chat, text_1.c_str(), text.size() );
    }

    //Update Player With EveryOne
    if( descPlayer.isUpdateWithEveryOne )
    {
        tie::net::message<GameMsg> msg;
        msg.header.id = GameMsg::Game_UpdatePlayer;
        msg << mapObjects[nPlayerID];
        Send( msg );
        // mapObjects[nPlayerID].chat = nullptr;
    }
}
void App::ResetPlayer()
{   
    mapObjects.clear();
    bWaitingForConnect = false;
    descPlayer.nUniqueID = 0;
	descPlayer.sRPS = sPlayerDescription::StateRPS::GiveUp;
	descPlayer.isThisTurn = false;
	descPlayer.isFuckUp = false;
	descPlayer.isWinner = false;
	descPlayer.countPress = 0;
}   
//=============================================================//
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
            int i = 1;
            rayCpp::DrawString( "List Player", Vec2{20.0f, 20.0f}, 26, WHITE );
            for( auto it = mapObjects.begin(); it != mapObjects.end(); it++, i++ )
            {
                Color c = PINK;
                if( it->second.nUniqueID == nPlayerID ) c = RED;
                
                std::string text = 
                    std::to_string(it->second.nUniqueID) + 
                    ": " +
                    std::to_string(it->second.countPress);
                
                rayCpp::DrawString( 
                    text,
                    Vec2{ 20.0f, 50.0f + i*30.0f }, 20, c
                );
            }
        }
    }
    if( bWaitingForConnect )
    {
        rayCpp::DrawString( 
            "Conneting...", 
            Vec2{settings::screenW - 200.0f, 50.0f},
            20, WHITE
        );
    }
}