#pragma once
#include "net_common.h"
#include "net_message.h"
#include "net_tsqueue.h"
#include "net_connection.h"

namespace tie
{
    namespace net
    {
        template <typename T>
        class server_interface
        {
        public:
            server_interface( uint16_t port ) 
                :m_asioAcceptor( m_asioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port) )
            {}
            virtual ~server_interface() 
            {
                Stop();
            }
        public:
            bool Start()
            {
                try
                {
                    WaitForClientConnection();

                    m_threadContext = std::thread( [this]() { m_asioContext.run(); } );
                }
                catch(const std::exception& e)
                {
                    std::cerr << "[SERVER] exception: " << e.what() << '\n';
                    return false;
                }
                
                std::cout<<"[SERVER] Started!"<<std::endl;
                return true;
            }
            void Stop()
            {
                m_asioContext.stop();

                if( m_threadContext.joinable() ) m_threadContext.join();
                std::cout<<"[SERVER] stopped!"<<std::endl;
            }

            void WaitForClientConnection()
            {
                m_asioAcceptor.async_accept(
                    [this]( std::error_code ec, asio::ip::tcp::socket socket )
                    {
                        if( !ec )
                        {
                            std::cout << "[SERVER] New connect client: " << socket.remote_endpoint() << '\n';
                            std::shared_ptr<connection<T>> newconn = 
                                std::make_shared<connection<T>>(
                                    connection<T>::owner::server, m_asioContext, std::move(socket), m_qMessagesIn
                                );
                            if( OnClientConnect( newconn ) )
                            {
                                m_deqConnections.push_back(std::move(newconn));
                                m_deqConnections.back()->ConnectToClient( this, nIdCount++ );

                                std::cout<< "[" << m_deqConnections.back()->GetId() << "]: Connect Approved!" <<std::endl;
                            }
                            else
                            {
                                std::cout<< "[------] Connection denied! " <<std::endl;
                            }
                        }
                        else
                        {
                            std::cout<< "[SERVER] New connect client error: "<< ec.message() <<std::endl;
                        }
                        WaitForClientConnection();
                    }
                );
            }
            void MessageClient( std::shared_ptr<connection<T>> client, const message<T>& msg )
            {
                if( client && client->IsConnected() )
                {
                    client->Send(msg);
                }
                else
                {
                    std::cout << "Message connect fail (id): " << client->GetId() << std::endl;
                    OnClientDisconnect( client );
                    client.reset();
                    m_deqConnections.erase(
                        std::remove( m_deqConnections.begin(), m_deqConnections.end(), client ), m_deqConnections.end()
                    );    
                }
            }
            void MessageAllClients( const message<T>& msg, std::shared_ptr<connection<T>> pIgnoreClient = nullptr )
            {
                bool bInvalidClientExist = false;   //Ensure q_deqConnections will not be changed whilst loop through it
                for( auto& client : m_deqConnections )
                {
                    if( client && client->IsConnected() )
                    {
                        if( client != pIgnoreClient ) client->Send(msg);
                    }
                    else
                    {
                        std::cout << "Message connect fail (id): " << client->GetId() << std::endl;
                        OnClientDisconnect( client );
                        client.reset();
                        bInvalidClientExist = true;
                    }
                }
                if( bInvalidClientExist )
                {
                    //Erase nullptr in list shared_ptr m_deqConnections
                    m_deqConnections.erase(
                        std::remove( m_deqConnections.begin(), m_deqConnections.end(), nullptr ), m_deqConnections.end()
                    );    
                }
            }

            void Update( size_t nMaxMessage = -1, bool bWait = false )
            {
                if( bWait ) m_qMessagesIn.wait();

                size_t nMessageCount = 0;
                while( nMessageCount < nMaxMessage && !m_qMessagesIn.empty() )
                {
                    auto msg = m_qMessagesIn.pop_front();

                    OnMessage( msg.remote, msg.msg );

                    nMessageCount++;
                }
            }
        protected:
            virtual bool OnClientConnect( std::shared_ptr<connection<T>> client )
            {
                return false;
            }
            virtual void OnClientDisconnect( std::shared_ptr<connection<T>> client )
            {

            }
            virtual void OnMessage( std::shared_ptr<connection<T>> client, message<T>& msg )
            {

            }
        public:
            virtual void OnClientValidated( std::shared_ptr<connection<T>> client )
            {

            }
            virtual void UpdateGame()
            {
                
            }
        protected:
            tsqueue<owner_message<T>> m_qMessagesIn;

            std::deque<std::shared_ptr<connection<T>>> m_deqConnections;

            asio::io_context m_asioContext;
            std::thread m_threadContext;

            asio::ip::tcp::acceptor m_asioAcceptor;

            uint32_t nIdCount = 10000;
        };
    }
}