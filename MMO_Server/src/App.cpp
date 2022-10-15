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
            tie::var::cur_order = -1;
            tie::var::serch_order = 1;
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
            // //Check to assign host
            // if( tie::var::IDHost == -1u )
            // {
            //     tie::var::IDHost = desc.nUniqueID;
            //     desc.isHost = true;
            //     desc.isReady = true;
            // }
            // else desc.isHost = false;
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
            
            // if( client->GetId() != tie::var::IDHost )
            // {
            // }
            //Update rest connects
            sPlayerDescription desc;
            msg >> desc;
            tie::net::message<GameMsg> mRemove;
            mRemove.header.id = GameMsg::Game_RemovePlayer;
            mRemove << desc.nUniqueID;
            MessageAllClients( mRemove );
        } break;

        case GameMsg::Game_UpdatePlayer:
        {
            //Update everyone except incoming client, action before change msg
            MessageAllClients( msg, client );

            //Update MapPlayerRoster
            sPlayerDescription desc;
            msg >> desc;

            //Player want to not ready but they losed
            // if( !desc.isReady && mapPlayerRegister[client->GetId()].isLose ) desc.isReady = true;

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
        case GameMsg::PR_SendChat: MessageAllClients( msg ); break;

        case GameMsg::RPSGame:
        {
            // //Client must be host
            // if( client->GetId() == tie::var::IDHost )
            // {
            // }
            std::cout << "Serch: " << tie::var::serch_order << '\n';
            std::cout << "CurOrder: " << tie::var::cur_order << '\n';
            std::cout << "ClientOrder: " << mapPlayerRegister[client->GetId()].order << '\n';
            bool isClientReady;
            msg >> isClientReady;
            //Check able to join game
            if( mapPlayerRegister[client->GetId()].order == tie::var::cur_order )
            {
                //Update to everyone
                mapPlayerRoster[client->GetId()].isReady = isClientReady;
                std::cout << "ClientReady: " << isClientReady << '\n';
                tie::net::message<GameMsg> msg;
                msg.header.id = GameMsg::Game_UpdatePlayer;
                msg << mapPlayerRoster[client->GetId()];
                MessageAllClients( msg );

                if( isClientReady )
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
                            if( !p.second.isReady && mapPlayerRegister[p.first].order == tie::var::cur_order )
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
                            rps.countdown = 5.0f + 1.0f;
                            tie::var::timerRPS = 5.0f + 1.0f;
                            mRPS << rps;
                            MessageAllClients( mRPS );
                        }
                    }
                }
            }
        } break;

        case GameMsg::PR_RPSGame_Choose:
        {
            if( mapPlayerRegister.count( client->GetId() ) && tie::var::timerRPS <= 5.0f
                && mapPlayerRegister[client->GetId()].order == tie::var::cur_order
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
        //Duration of each loop
        float dTime = ft_timerfps.Mark();
        
        static float dura_fps = 0.0f;
        dura_fps += dTime;
        if( dura_fps >= 1.0f )
        {
            std::cout << "For 1s: " << dura_fps << std::endl;
            dura_fps = 0.0f;
        }
        
        //Update
        if( sv.timerRPS > 0.0f )
        {
            sv.timerRPS -= dTime;
            sv.timeUpRPS = false;
        }
        else if( !sv.timeUpRPS )
        {
            sv.timeUpRPS = true;
            sv.timerRPS = 0;
            std::cout << "RPSGame TimeUp" << std::endl;
            sv.MessageAllClients(tie::make::MM_HeaderID(GameMsg::Server_RPS_TimeUp));


            //Send RPSGame result to everyone
            std::map<uint32_t, bool>&& vPlayerLose = tie::util::RPSGameCheckRes( sv.mapPlayerRegister, sv.cur_order );
            tie::util::RPSGame_ProcessOrder(
                sv.mapPlayerRegister, vPlayerLose,
                std::ref(sv.cur_order), std::ref(sv.serch_order)
            );
            std::cout << "===========Result===========\n";
            for( auto& p : vPlayerLose )
            {
                std::string choice = "";
                switch (sv.mapPlayerRegister[p.first].oRPS)
                {
                case sRPSGame::Options::Rock: choice = "Rock"; break;
                case sRPSGame::Options::Paper: choice = "Paper"; break;
                case sRPSGame::Options::Scissor: choice = "Scissor"; break;
                default: choice = "Give Up"; break;
                }
                std::cout << "Player " << sv.mapPlayerRoster[p.first].name << ": choose " << choice << '\n';
            }
            if( sv.cur_order > (int)sv.mapPlayerRoster.size() )
            {
                sv.cur_order = -1;
                sv.serch_order = 1;
                std::cout << "Done RPS Game" << std::endl;
            }
            // for( auto& p : sv.mapPlayerRegister )
            // {
            //     if( vPlayerLose.count( p.first ) ) continue;
            //     std::cout << p.first << " (order): " << p.second.order << std::endl;
            //     tie::net::message<GameMsg> msg;
            //     msg.header.id = GameMsg::RPSGame;
            //     sRPSGame rps{};
            //     rps.timeup = true;
            //     rps.owner_id = p.first;
            //     rps.option = p.second.oRPS;
            //     rps.order = p.second.order;
            //     rps.cur_order = sv.cur_order;
            //     msg << rps;
            //     sv.MessageAllClients( msg );
            // }
            for( auto& p : vPlayerLose )
            {
                sPlayerRegisters& player = sv.mapPlayerRegister[p.first];
                std::cout << p.first << " (order): " << player.order << std::endl;
                tie::net::message<GameMsg> msg;
                msg.header.id = GameMsg::RPSGame;
                sRPSGame rps{};
                rps.timeup = true;
                rps.owner_id = p.first;
                rps.option = player.oRPS;
                rps.order = player.order;
                rps.cur_order = sv.cur_order;
                msg << rps;
                sv.MessageAllClients( msg );
            }
            sv.MessageAllClients(tie::make::MM_HeaderID(GameMsg::Server_RPS_DoneUpdate));
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
