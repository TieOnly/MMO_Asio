#include "../../MMO_Src/network/tie_net.h"
#include <iostream>

class CustomServer : public tie::net::server_interface<GameMsg>
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
            mapPlayerRoster.insert_or_assign( desc.nUniqueID, desc );

            tie::net::message<GameMsg> msgSendID;
            msgSendID.header.id = GameMsg::Client_AssignID;
            msgSendID << desc.nUniqueID;
            MessageClient( client, msgSendID );

            tie::net::message<GameMsg> msgAddPlayer;
            msgAddPlayer.header.id = GameMsg::Game_AddPlayer;
            msgAddPlayer << desc;
            MessageAllClients( msgAddPlayer );

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
            mapPlayerRoster[desc.nUniqueID] = desc;
        } break;
        
        case GameMsg::Game_MesStr:
        {
            
        } break;

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
