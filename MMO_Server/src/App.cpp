#include "../../MMO_Src/network/tie_net.h"
#include "Utils.h"
#include "TieThread.h"
#include "FrameTime.h"
#include <iostream>

class CustomServer : public tie::net::server_interface<GameMsg>, public tie::var
{
public:
    CustomServer() = default;
    CustomServer( const uint16_t port )
        : tie::net::server_interface<GameMsg>( port )
    {}
    std::unordered_map< uint32_t, sPlayerDescription > mapPlayerRoster;
    std::unordered_map< uint32_t, sPlayerRegisters > mapPlayerRegister;
    std::vector<uint32_t> vGarbageIDs;

protected:
    bool OnClientConnect(std::shared_ptr<tie::net::connection<GameMsg>> client) override
	{
		// For now we will allow all 
		return true;
	}
    void OnClientValidated(std::shared_ptr<tie::net::connection<GameMsg>> client) override
	{
		// Client passed validation check, so send them a message informing
		// them they can continue to communicate
		tie::net::message<GameMsg> msg;
		msg.header.id = GameMsg::Client_Accepted;
		client->Send(msg);
	}
    void OnClientDisconnect(std::shared_ptr<tie::net::connection<GameMsg>> client) override
    {
        if( client )
        {
            //Exits client in mapPlayerRoster
            if( mapPlayerRoster.find(client->GetId()) != mapPlayerRoster.end() )
            {
                if( client->GetId() == tie::var::IDHost )
                {
                    DisconnectHost( client ); 
                    return;
                }
                std::cout << "[REMOVE]: " << (int)mapPlayerRoster[client->GetId()].nUniqueID << std::endl;
                mapPlayerRoster.erase( client->GetId() );
                mapPlayerRegister.erase( client->GetId() );
                vGarbageIDs.push_back( client->GetId() );
            }
            //
            client.reset();
            m_deqConnections.erase(
                std::remove( m_deqConnections.begin(), m_deqConnections.end(), client ), m_deqConnections.end()
            ); 
        }
    }
    void DisconnectHost( std::shared_ptr<tie::net::connection<GameMsg>> host )
    {
        if( host )
        {
            for( auto& client : m_deqConnections )
            {
                if( client->IsConnected() && client->GetId() != tie::var::IDHost )
                {
                    //Exits client in mapPlayerRoster
                    if( mapPlayerRoster.find(client->GetId()) != mapPlayerRoster.end() )
                    {
                        std::cout << "[REMOVE BY HOST]: " << (int)client->GetId() << std::endl;
                        tie::net::message<GameMsg> mRemove;
                        mRemove.header.id = GameMsg::Server_RemoveHost;
                        MessageClient( client, mRemove );
                    }
                }
            }
            std::cout << "[REMOVE HOST]: " << (int)host->GetId() << std::endl;
            mapPlayerRoster.clear();
            mapPlayerRegister.clear();
            m_deqConnections.clear();
            tie::var::IDHost = -1u;
        }
    }
    void OnMessage( 
        std::shared_ptr<tie::net::connection<GameMsg>> client, 
        tie::net::message<GameMsg>& msg )
    {
        switch ( msg.header.id )
        {
        case GameMsg::Client_RegisterWithServer:
        {
            sPlayerDescription desc;
            msg >> desc;
            desc.nUniqueID = client->GetId();
            tie::make::MakeArrChar( desc.name, tie::def::ml_name, 
                "Player " + std::to_string(desc.nUniqueID) 
            );
            //Check to assign host
            if( tie::var::IDHost == -1u )
            {
                tie::var::IDHost = desc.nUniqueID;
                desc.isHost = true;
                desc.isReady = true;
            }
            else desc.isHost = false;
            //Update mapPlayerRoster, mapPlayerRegister
            mapPlayerRoster.insert_or_assign( desc.nUniqueID, desc );
            sPlayerRegisters desc_r{};
            mapPlayerRegister.insert_or_assign( desc.nUniqueID, desc_r );

            //Assign new client
            tie::net::message<GameMsg> msgSendID;
            msgSendID.header.id = GameMsg::Client_AssignID;
            msgSendID << desc.nUniqueID;
            MessageClient( client, msgSendID );

            //Add new player with its desc
            tie::net::message<GameMsg> msgAddPlayer;
            msgAddPlayer.header.id = GameMsg::Game_AddPlayer;
            msgAddPlayer << desc;
            MessageAllClients( msgAddPlayer );

            //Send to everyone about new client
            for( const auto& p : mapPlayerRoster )
            {
                tie::net::message<GameMsg> msgAddOtherPlayer;
                msgAddOtherPlayer.header.id = GameMsg::Game_AddPlayer;
                msgAddOtherPlayer << p.second;
                MessageClient( client, msgAddOtherPlayer );
            }
        } break;

        case GameMsg::Client_UnregisterWithServer:
        {
            //Disconnect this client
            OnClientDisconnect( client );
            
            if( client->GetId() != tie::var::IDHost )
            {
                //Update rest connects
                sPlayerDescription desc;
                msg >> desc;
                tie::net::message<GameMsg> mRemove;
                mRemove.header.id = GameMsg::Game_RemovePlayer;
                mRemove << desc.nUniqueID;
                MessageAllClients( mRemove );
            }
        } break;

        case GameMsg::Game_UpdatePlayer:
        {
            //Update everyone except incoming client, action before change msg
            MessageAllClients( msg, client );

            //Update MapPlayerRoster
            sPlayerDescription desc;
            msg >> desc;

            //Player want to not ready but they losed
            if( !desc.isReady && mapPlayerRegister[client->GetId()].isLose ) desc.isReady = true;

            //Update mapPlayerRoster, mapPlayerRegister
            mapPlayerRoster[client->GetId()] = desc;
            tie::make::MakeArrChar( 
                mapPlayerRegister[client->GetId()].name, tie::def::ml_name,
                desc.name
            );
        } break;
        
        case GameMsg::Game_MesStr:
        {
            //Update everyone except incoming client, action before change msg
            MessageAllClients( msg, client );
        } break;

        case GameMsg::PR_SendChatTyping: MessageAllClients( msg, client ); break;
        case GameMsg::PR_SendChat: MessageAllClients( msg, client ); break;

        case GameMsg::RPSGame:
        {
            //Client must be host
            if( client->GetId() == tie::var::IDHost )
            {
                sRPSGame rps{};
                tie::net::message<GameMsg> mRPS;
                mRPS.header.id = GameMsg::RPSGame;
                if( mapPlayerRoster.size() < 2u )
                {
                    rps.isAbleStart = false;
                    mRPS << rps;
                    MessageClient( client, mRPS );
                }
                else if( tie::var::timerRPS == 0.0f )
                {
                    rps.isAbleStart = true;
                    for( const auto& p : mapPlayerRoster )
                    {
                        if( !p.second.isReady && !mapPlayerRegister[p.first].isLose )
                        {
                            rps.isAbleStart = false;
                            rps.isAnyOneNotReady = true;
                            mRPS << rps;
                            MessageClient( client, mRPS );
                            break;
                        }
                    }
                    if( rps.isAbleStart )
                    {
                        std::cout << "Start RPSGame" << std::endl;
                        rps.countdown = 5.0f;
                        tie::var::timerRPS = 5.0f;
                        mRPS << rps;
                        MessageAllClients( mRPS );
                    }
                }
            }
        } break;

        case GameMsg::PR_RPSGame_Choose:
        {
            if( mapPlayerRegister.count( client->GetId() ) && !tie::var::timeUpRPS
                && !mapPlayerRegister[client->GetId()].isLose 
            )
            {
                sRPSGame::Options cRPS;
                msg >> cRPS;
                mapPlayerRegister[client->GetId()].oRPS = cRPS;
                std::cout << "[" << client->GetId() << "]: choose " << (int)cRPS << std::endl;
            }
        } break;

        default: break;
        }

        //Update Remove Player
        if( !vGarbageIDs.empty() )
        {
            for( auto id : vGarbageIDs )
            {
                tie::net::message<GameMsg> msg;
                msg.header.id = GameMsg::Game_RemovePlayer;
                msg << id;
                std::cout << "Removing: " << id << std::endl;
                MessageAllClients( msg );
            }
            vGarbageIDs.clear();
        }
    }
public:
    static void TimerFPS( CustomServer& sv )
    {
        static FrameTime ft_timerfps;
        static float dura_fps = 0.0f;
        //Duration of each loop
        float dTime = ft_timerfps.Mark();
        // std::cout << "DTime: " << dTime*1000 << "ms" << std::endl;
        dura_fps += dTime;
        if( dura_fps >= 1.0f )
        {
            std::cout << "For 1s: " << dura_fps << std::endl;
            dura_fps = 0.0f;
        }
        // std::cout << "Dura_FPS: " << dura_fps*1000 << "ms" << std::endl;
        //Update
        if( sv.timerRPS > 0.0f )
        {
            sv.timerRPS -= dTime;
            sv.timeUpRPS = false;
        }
        else if( !sv.timeUpRPS )
        {
            sv.timeUpRPS = true;
            std::cout << "RPSGame TimeUp" << std::endl;
            sv.timerRPS = 0;
            for( auto& p : sv.mapPlayerRegister )
            {
                tie::net::message<GameMsg> msg;
                msg.header.id = GameMsg::RPSGame;
                sRPSGame rps{};
                rps.timeup = true;
                rps.owner_id = p.first;
                rps.option = p.second.oRPS;
                msg << rps;
                sv.MessageAllClients( msg );
            }
            sv.MessageAllClients(tie::make::MM_HeaderID(GameMsg::Server_RPS_DoneUpdate));

            std::cout << "===========Result===========\n";
            for( auto& p : sv.mapPlayerRegister )
            {
                std::string choice = "";
                switch (p.second.oRPS)
                {
                case sRPSGame::Options::Rock: choice = "Rock"; break;
                case sRPSGame::Options::Paper: choice = "Paper"; break;
                case sRPSGame::Options::Scissor: choice = "Scissor"; break;
                default: choice = "Give Up"; break;
                }
                std::cout << "Player " << p.second.name << ": choose " << choice << '\n';
            }

            //Send RPSGame result to everyone
            std::map<uint32_t, bool>&& vPlayerLose = tie::util::RPSGameCheckRes( sv.mapPlayerRegister );
            std::vector<uint32_t> vPlayerShouldRemove;
            std::cout << "Amount Player Lose: " << (int)vPlayerLose.size() << std::endl;
            for( auto& p : vPlayerLose )
            {
                if( p.second )
                {
                    vPlayerShouldRemove.push_back(p.first);
                    tie::net::message<GameMsg> msg;
                    msg.header.id = GameMsg::Server_RPS_IDPlayer_Lose;
                    msg << p.first;
                    sv.MessageAllClients( msg );
                }
            }
            std::cout << "Amount Player Be Removed: " << (int)vPlayerShouldRemove.size() << std::endl;
            for( auto& p : vPlayerShouldRemove ) vPlayerLose.erase( p );
            if( vPlayerLose.size() == 1u )
            {
                tie::net::message<GameMsg> msg;
                msg.header.id = GameMsg::Server_RPS_IDPlayer_Win;
                msg << vPlayerLose.begin()->first;
                sv.MessageAllClients( msg );
            }
        }
    }
};

int main()
{
    CustomServer server( 60000 );
    server.Start();

    tie::thread tie_thread{};
    tie::thread::AddThread( 
        tie::thread::MakeLoop<CustomServer&>, 4000, 
        CustomServer::TimerFPS, std::ref(server)
    );

    while (1)
    {
        server.Update(-1, true);
    }

    return 0;
}
