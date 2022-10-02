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
        class client_interface
        {
        public:
            client_interface() 
            {}

            virtual ~client_interface()
            {
                std::cout << "Destroy" << std::endl;
                Disconnect();
            }
        public:
            bool Connect( const std::string& host, const uint16_t port, const uint32_t limitAmountMesInQueue = 1000 )
            {
                try
                {
                    if( m_context.stopped() ) m_context.restart();
                    asio::ip::tcp::resolver resolver( m_context );
                    asio::ip::tcp::resolver::results_type end_point = 
                        resolver.resolve( host, std::to_string(port) );

                    m_connection = std::make_unique<connection<T>>(
                        connection<T>::owner::client, m_context, asio::ip::tcp::socket(m_context), m_qMessagesIn
                    );
                    m_connection->ConnectToServer( end_point, limitAmountMesInQueue );

                    thrContext = std::thread( [this]() { m_context.run(); } );
                }
                catch(const std::exception& e)
                {
                    std::cerr << "Client Exception: " << e.what() << '\n';
                    return false;
                }
                return true;
            }
            void Disconnect()
            {
                if( IsConnected() )
                    m_connection->Disconnect();

                m_context.stop();
                if( thrContext.joinable() )
                    thrContext.join();
                    
				// Clear data
				m_connection.release();
                m_qMessagesIn.clear();
            }
            bool IsConnected()
            {
                if( m_connection )
                    return m_connection->IsConnected();
                else
                    return false;
            }

        public:
            void Send( const message<T>& msg )
            {
                if( IsConnected() ) m_connection->Send( msg );
            }
            tsqueue<owner_message<T>>& InComing()
            {
                return m_qMessagesIn;
            }
        protected:
            asio::io_context m_context;

            std::thread thrContext;

            std::unique_ptr<connection<T>> m_connection;
        private:
            tsqueue<owner_message<T>> m_qMessagesIn;
        };
    }
}