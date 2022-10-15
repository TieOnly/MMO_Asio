#include "../../MMO_Src/pch/raylibCpp.h"
#include "../../MMO_Src/network/tie_net.h"

#include "App.h"

App::App(const int fps)
{
    SetTargetFPS(fps);

    gui.LoadAsset();
    tie::utils::AddThread(
        tie::utils::MakeLoop<
            std::unordered_map< uint32_t, sPlayerDescription >&,
            std::unordered_map< uint32_t, sMapObjDesc >&,
            float&
        >, 
        4000,tie::utils::TimerThread, 
        std::ref(mapObjects), std::ref(mapDescObjs),
        std::ref(tie::var::timerRPS)
    );
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
                Connect( "127.0.0.1", 60000, 100 );
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
            gui.Btn_UpdateReadyState(false);
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
            
            mapDescObjs.insert_or_assign( desc.nUniqueID, sMapObjDesc{} );
            tie::make::MakeDescObjDest( mapObjects, mapDescObjs );

            if( desc.nUniqueID == nPlayerID ) bWaitingForConnect = false;
        } break;

        case GameMsg::Game_RemovePlayer:
        {
            uint32_t nRemovalID = 0;
            msg >> nRemovalID;
            mapObjects.erase( nRemovalID );
            mapDescObjs.erase( nRemovalID );
            tie::make::MakeDescObjDest( mapObjects, mapDescObjs );
        } break;

        case GameMsg::Game_UpdatePlayer:
        {
            sPlayerDescription desc;
            msg >> desc;
            
            if( mapObjects.count( desc.nUniqueID ) )
            {
                if( std::strcmp(desc.name, mapObjects[desc.nUniqueID].name) != 0 )
                {
                    tie::make::MakeArrChar(
                        mapObjects[desc.nUniqueID].name, tie::def::ml_name,
                        desc.name
                    );
                    mapDescObjs[desc.nUniqueID].pos_chat = rayCpp::MakePosNextStr( 
                        mapDescObjs[desc.nUniqueID].dest_name.topleft,
                        mapObjects[desc.nUniqueID].name
                    );
                    std::cout << mapDescObjs[desc.nUniqueID].pos_chat.x << "-" << mapDescObjs[desc.nUniqueID].pos_chat.y << std::endl;
                }
            }

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
            if( mapObjects.count( pMes.nUniqueID ) )
            {
                if( mapDescObjs[pMes.nUniqueID].timerChat == 0.0f )
                {
                    mapDescObjs[pMes.nUniqueID].timerChat = pMes.timerChat;
                }
                mapDescObjs[pMes.nUniqueID].isTyping = false;
                tie::make::MakeArrChar( mapDescObjs[pMes.nUniqueID].chat, tie::def::ml_chat, pMes.chat );
                std::cout << mapObjects[pMes.nUniqueID].name << ": " << pMes.chat << std::endl;
            }
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

        case GameMsg::RPSGame: App::IPMT_GM_RPSGame( msg ); break;
        
        case GameMsg::Server_RPS_DoneUpdate: App::IPMT_GM_Server_RPS_DoneUpdate( msg ); break;

        // case GameMsg::Server_RPS_AssignOrder: App::IPMT_GM_Server_RPS_AssignOrder( msg ); break;

        // case GameMsg::Server_RPS_IDPlayer_Lose: App::IPMT_GM_Server_RPS_IDPlayer_Lose( msg ); break;
        
        // case GameMsg::Server_RPS_IDPlayer_Win: App::IPMT_GM_Server_RPS_IDPlayer_Win( msg ); break;

        default: break;
        }
    }

    //Update Game
    //Break if nobody connect with server
    if( mapObjects.empty() ) return;

    //Process Input Bar
    ProcessInputBar();
    //Process Buttons
    ProcessBtns();
    //Update During TimerRPS
    UpdateDuringTimerRPSGame();
    //Update By EveryOne
    UpdateByEveryOne();
    //Update Player With EveryOne
    if( descPlayer.isUpdateWithEveryOne )
    {
        tie::net::message<GameMsg> msg;
        msg.header.id = GameMsg::Game_UpdatePlayer;
        msg << mapObjects[nPlayerID];
        Send( msg );
    }
}
//================Update State TimerRPS========================//
//=============================================================//
void App::ResetPlayer()
{   
    mapObjects.clear();
    mapDescObjs.clear();
    bWaitingForConnect = false;
    descPlayer.nUniqueID = 0;
    descPlayer.isHost = false;
	descPlayer.isThisTurn = false;
	descPlayer.isFuckUp = false;
	descPlayer.isWinner = false;
	descPlayer.countPress = 0;

    gui.Btn_UpdateReadyState(false);
    gui.buttons[(int)GUI::Btn_SeqID::ReadyState].isAbleActive = true;
}   
//=======================Process Input Bar=====================//
//=============================================================//
void App::ProcessInputBar()
{
    if( !gui.GetGameMod().input_value.empty() )
    {
        if( gui.GetGameMod().input_owner == GUI::Btn_SeqID::Change_Name )
        {
            tie::make::MakeArrChar( mapObjects[nPlayerID].name, tie::def::ml_name,
                gui.GetGameMod().input_value
            );
            mapDescObjs[nPlayerID].pos_chat = rayCpp::MakePosNextStr( 
                mapDescObjs[nPlayerID].dest_name.topleft,
                mapObjects[nPlayerID].name
            );
            
            std::cout << mapDescObjs[nPlayerID].pos_chat.x << "-" << mapDescObjs[nPlayerID].pos_chat.y << std::endl;
            descPlayer.isUpdateWithEveryOne = true;
        }
        else if( gui.GetGameMod().input_owner == GUI::Btn_SeqID::Chat )
        {
            tie::make::MakeArrChar( mapDescObjs[nPlayerID].chat, tie::def::ml_chat,
                gui.GetGameMod().input_value 
            );
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
}
//=======================Process Buttons=======================//
//=============================================================//
void App::ProcessBtns()
{
    //Event BTN_ReadyState
    if( gui.IsBtnClick( GUI::Btn_SeqID::ReadyState ) )
    {
        // descPlayer.isUpdateWithEveryOne = true;
        mapObjects[nPlayerID].isReady = !mapObjects[nPlayerID].isReady;
        if( !mapObjects[nPlayerID].isHost )
        {
            gui.Btn_UpdateReadyState( mapObjects[nPlayerID].isReady );
            tie::net::message<GameMsg> msg;
            msg.header.id = GameMsg::RPSGame;
            msg << mapObjects[nPlayerID].isReady;
            Send( msg );
        }
    }
}
//=================Update During TimerRPS=======================//
//=============================================================//
void App::UpdateDuringTimerRPSGame()
{
    if( tie::var::timerRPS > 0.0f )
    {
        if( tie::var::timerRPS <= 5.0f )
        {
            gui.rpsGame.sub_desc.time = tie::var::timerRPS;
            if( gui.rpsGame.IsBtnClick( GUI::RPSGame::SeqID_Btn::Rock ) )
            {
                Send( tie::make::MM_PR_RPSGame_Choose( sRPSGame::Options::Rock ) );
                // gui.rpsGame.SetSubDesc("Your choice is Rock");
            }
            else if( gui.rpsGame.IsBtnClick( GUI::RPSGame::SeqID_Btn::Paper ) )
            {
                Send( tie::make::MM_PR_RPSGame_Choose( sRPSGame::Options::Paper ) );
                // gui.rpsGame.SetSubDesc("Your choice is Paper");
            }
            else if( gui.rpsGame.IsBtnClick( GUI::RPSGame::SeqID_Btn::Scissor ) )
            {
                Send( tie::make::MM_PR_RPSGame_Choose( sRPSGame::Options::Scissor ) );
                // gui.rpsGame.SetSubDesc("Your choice is Scissor");
            }
        }
    }
}
//====================Update By EveryOne=======================//
//=============================================================//
void App::UpdateByEveryOne()
{
    for( auto it = mapObjects.begin(); it != mapObjects.end(); it++ )
    {
        //Desc
        // sMapObjDesc& mo_desc = mapDescObjs[it->second.nUniqueID];
        //Update TimerChat
        // if( mo_desc.timerChat > 0.0f ) mo_desc.timerChat -= 0.01666f;
        // if( mo_desc.timerNotify > 0.0f ) mo_desc.timerNotify -= 0.01666f;
    }
    if( mapObjects[nPlayerID].isHost )
    {
        bool isEveryOneReady = true;
        for( auto it = mapObjects.begin(); it != mapObjects.end(); it++ )
        {
            if( !it->second.isReady ) isEveryOneReady = false;
        }
        if( isEveryOneReady ) 
            gui.buttons[(int)GUI::Btn_SeqID::ReadyState].isAbleActive = true;
        else 
            gui.buttons[(int)GUI::Btn_SeqID::ReadyState].isAbleActive = false;
    }
}
//=============================================================//
//=============================================================//
