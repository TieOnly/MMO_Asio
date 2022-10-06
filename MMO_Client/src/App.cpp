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
            if( mapObjects[nPlayerID].isHost ) std::cout << "Ban la Host" << std::endl;
            
            sMapObjDesc mo_desc;
            mo_desc.dest = RectF{ 
                Vec2{ 20.0f, 50.0f + (int)mapObjects.size()*30.0f }, 
                (float)MeasureText( desc.name, 20 ), 20.0f
            };
            mo_desc.pos_chat = Vec2{ mo_desc.dest.right + 60.0f,mo_desc.dest.top };
            mapDescObjs.insert_or_assign( desc.nUniqueID, mo_desc );

            if( desc.nUniqueID == nPlayerID ) bWaitingForConnect = false;
        } break;

        case GameMsg::Game_RemovePlayer:
        {
            uint32_t nRemovalID = 0;
            msg >> nRemovalID;
            mapObjects.erase( nRemovalID );
            mapDescObjs.erase( nRemovalID );
        } break;

        case GameMsg::Game_UpdatePlayer:
        {
            sPlayerDescription desc;
            msg >> desc;
            mapObjects.insert_or_assign( desc.nUniqueID, desc );
            
            std::cout 
                << mapObjects[desc.nUniqueID].name << ": Update! " <<
            std::endl;
        } break;

        case GameMsg::PR_SendChatTyping:
        {
            uint32_t nID; msg >> nID;
            mapDescObjs[nID].isTyping = true;
        } break;

        case GameMsg::PR_SendChat:
        {
            sPlayerText pMes;
            msg >> pMes;
            mapDescObjs[pMes.nUniqueID].timerChat = pMes.timerChat;
            mapDescObjs[pMes.nUniqueID].isTyping = false;
            tie::make::MakeArrChar( mapDescObjs[pMes.nUniqueID].chat, tie::def::ml_chat, pMes.chat );
            std::cout << mapObjects[pMes.nUniqueID].name << ": " << pMes.chat << std::endl;
        } break;

        case GameMsg::Server_RemoveHost:
        {
            if( !mapObjects[nPlayerID].isHost )
            {
                std::cout << "Host is not exist" << std::endl;
            }
            else std::cout << "You out and You are host =((" << std::endl;
            //Reset To ChoseMode
            Disconnect();
            gui.ResetGameMode();
            ResetPlayer();
            GUI::SetLayer( GUI::Layer::ChoseMode );
            return;
        } break;

        default: break;
        }
    }

    //Update Game
    if( !gui.GetGameMod().input_value.empty() )
    {
        if( gui.GetGameMod().input_owner == GUI::Btn_SeqID::Change_Name )
        {
            tie::make::MakeArrChar( mapObjects[nPlayerID].name, tie::def::ml_name,
                gui.GetGameMod().input_value
            );
            descPlayer.isUpdateWithEveryOne = true;
        }
        else if( gui.GetGameMod().input_owner == GUI::Btn_SeqID::Chat )
        {
            tie::make::MakeArrChar( mapDescObjs[nPlayerID].chat, tie::def::ml_chat,
                gui.GetGameMod().input_value 
            );
            mapDescObjs[nPlayerID].timerChat = 6.0f;
            Send( tie::make::MM_PR_Chat( nPlayerID, gui.GetGameMod().input_value ) );
        }
        gui.ClearInputValue();
    }
    else if( gui.GetGameMod().input_owner == GUI::Btn_SeqID::Chat )
    {
        if( gui.GetGameMod().isTypeChating && mapDescObjs[nPlayerID].timerChat <= 0.0f )
        {
            Send( tie::make::MM_PR_ChatTyping( nPlayerID ) );
        }
    }

    for( auto it = mapDescObjs.begin(); it != mapDescObjs.end(); it++ )
    {
        //Update TimerChat
        if( it->second.timerChat > 0.0f ) it->second.timerChat -= 0.01666f;
    }

    if( IsKeyPressed( KEY_UP ) )
    {
        descPlayer.isUpdateWithEveryOne = true;
    }

    //Update Player With EveryOne
    if( descPlayer.isUpdateWithEveryOne )
    {
        tie::net::message<GameMsg> msg;
        msg.header.id = GameMsg::Game_UpdatePlayer;
        msg << mapObjects[nPlayerID];
        Send( msg );
    }
}
void App::ResetPlayer()
{   
    mapObjects.clear();
    mapDescObjs.clear();
    bWaitingForConnect = false;
    descPlayer.nUniqueID = 0;
	descPlayer.stateRPS = sPlayerDescription::StateRPS::GiveUp;
    descPlayer.isHost = false;
	descPlayer.isThisTurn = false;
	descPlayer.isFuckUp = false;
	descPlayer.isWinner = false;
	descPlayer.countPress = 0;
}   
//=============================================================//
