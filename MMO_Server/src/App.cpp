#include "../../MMO_Src/network/tie_net.h"
#include "Utils.h"
#include <iostream>

class CustomServer : public tie::net::server_interface<GameMsg>, public tie::var
{
public:
    CustomServer( const uint16_t port )
        : tie::net::server_interface<GameMsg>( port )
    {}
    std::unordered_map< uint32_t, sPlayerDescription > mapPlayerRoster;
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
                desc.isHost = true;
                tie::var::IDHost = desc.nUniqueID;
            }
            else desc.isHost = false;
            //Update mapPlayerRoster
            mapPlayerRoster.insert_or_assign( desc.nUniqueID, desc );

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
            mapPlayerRoster[desc.nUniqueID] = desc;
        } break;
        
        case GameMsg::Game_MesStr:
        {
            //Update everyone except incoming client, action before change msg
            MessageAllClients( msg, client );
        } break;

        case GameMsg::PR_SendChatTyping: MessageAllClients( msg, client ); break;
        case GameMsg::PR_SendChat: MessageAllClients( msg, client ); break;

        default: break;
        }

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

        std::cout << "Amount Player: " << mapPlayerRoster.size() << std::endl;
    }
};

int main()
{
    CustomServer server( 60000 );
    server.Start();

    while (1)
    {
        server.Update(-1, true);
    }

    return 0;
}
