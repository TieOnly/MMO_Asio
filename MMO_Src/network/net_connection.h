#pragma once
#include "net_common.h"
#include "net_message.h"
#include "net_tsqueue.h"

namespace tie
{
    namespace net
    {
        template <typename T>
        class server_interface;

        template <typename T>
        class connection : public std::enable_shared_from_this<connection<T>>
        {
        public:
            enum class owner
            {
                client,
                server
            };

            connection( owner parent, asio::io_context& asioContext, asio::ip::tcp::socket socket, tsqueue<owner_message<T>>& qIn)
                : m_context(asioContext), m_socket(std::move(socket)), m_qMessagesIn(qIn)
            {
                // m_socket.open();
                owner_type = parent;

                if( owner_type == owner::server )
                {
                    //server -> client, Random Data to validate client
                    m_nHandshakeOut = uint64_t(std::chrono::system_clock::now().time_since_epoch().count());
                    m_nHandshakeCheck = scramble( m_nHandshakeOut );
                }
                else
                {
                    //client -> server, nothing to define
                    m_nHandshakeOut = 0;
                    m_nHandshakeIn = 0;
                }
            }
            virtual ~connection() {}

            uint32_t GetId() const { return id; }
        public:
            void ConnectToClient( tie::net::server_interface<T>* server , uint32_t uId = 0 )
            {
                if( owner_type == owner::server )
                {
                    if( m_socket.is_open() )
                    {
                        id = uId;
                        //Send something to client attemp to connect
                        WriteValidation();

                        //Waiting expect result from client 
                        ReadValidation( server );
                    }
                }
            }
            void ConnectToServer( const asio::ip::tcp::resolver::results_type& end_point, const uint32_t nCapacity )
            {
                if( owner_type == owner::client )
                {
                    asio::async_connect( m_socket, end_point, 
                        [this, nCapacity]( std::error_code ec, asio::ip::tcp::endpoint endpoint )
                        {
                            if( !ec )
                            {
                                //Limit capacity of queue messagein
                                m_qMesInCapacity = nCapacity;
                                //Wait some thing to handshake with server
                                ReadValidation();
                            }
                            else
                            {
                                std::cout << "socket(ConnectToSer) is close: " << m_socket.is_open() << '\n';
                            }
                        }
                    );
                }
            }
            void Disconnect()
            {
                if( IsConnected() )
                {
                    asio::post( m_context, [this]() { m_socket.close(); } );
                }
            }
            bool IsConnected() const { return m_socket.is_open(); }

            void Send(const message<T>& msg)
            {
                asio::post( m_context, 
                    [this, msg] ()
                    {
                        bool bWritingMessage = !m_qMessagesOut.empty();
                        m_qMessagesOut.push_back( msg );
                        if( !bWritingMessage )
                        {
                            WriteHeader();
                        }
                    }
                );
            }
            
        private:
            void ReadHeader()
            {
                asio::async_read( m_socket, asio::buffer( &m_msgTempIn.header, sizeof(message_header<T>) ),
                    [this] ( std::error_code ec, std::size_t lenght )
                    {
                        if( !ec )
                        {
                            if( m_msgTempIn.header.size > 0 )
                            {
                                m_msgTempIn.body.resize(m_msgTempIn.header.size);
                                ReadBody();
                            }
                            else
                            {
                                if( IsConnected() ) AddToIncommingMessageQueue();
                            }
                        }
                        else
                        {
                            if( owner_type == owner::server )
                            {
                                std::cout<< "[" << id << "]: Read header faile" << std::endl;
                            }
                            m_socket.close();
                        }
                    }
                );
            }
            void ReadBody()
            {
                asio::async_read( m_socket, asio::buffer( m_msgTempIn.body.data(), m_msgTempIn.body.size() ),
                    [this] ( std::error_code ec, std::size_t lenght )
                    {
                        if( !ec )
                        {
                            if( IsConnected() ) AddToIncommingMessageQueue();
                        }
                        else
                        {
                            if( owner_type == owner::server )
                            {
                                std::cout<< "[" << id << "]: Read body faile" << std::endl;
                            }
                            m_socket.close();
                        }
                    }
                );
            }

            void WriteHeader()
            {
                asio::async_write( m_socket, asio::buffer( &m_qMessagesOut.front().header, sizeof(message_header<T>) ),
                    [this] ( std::error_code ec, std::size_t lenght )
                    {
                        if( !ec )
                        {
                            if( m_qMessagesOut.front().body.size() > 0 )
                            {
                                WriteBody();
                            }
                            else
                            {
                                m_qMessagesOut.pop_front();
                                if( !m_qMessagesOut.empty() )
                                {
                                    WriteHeader();
                                }
                            }
                        }
                        else
                        {
                            if( owner_type == owner::server )
                            {
                                std::cout<< "[" << id << "]: Write header faile" << std::endl;
                            }
                            m_socket.close();
                        }
                    }
                );
            }
            void WriteBody()
            {
                asio::async_write( m_socket, asio::buffer( m_qMessagesOut.front().body.data(), m_qMessagesOut.front().body.size() ),
                    [this] ( std::error_code ec, std::size_t lenght )
                    {
                        if( !ec )
                        {
                            m_qMessagesOut.pop_front();
                            if( !m_qMessagesOut.empty() )
                            {
                                WriteHeader();
                            }                            
                        }
                        else
                        {
                            if( owner_type == owner::server )
                            {
                                std::cout<< "[" << id << "]: Write body faile" << std::endl;
                            }
                            m_socket.close();
                        }
                    }
                );
            }

            void AddToIncommingMessageQueue()
            {
                if( owner_type == owner::server )
                {
                    m_qMessagesIn.push_back( { this -> shared_from_this(), m_msgTempIn } );
                }
                else
                {
                    //Limit 1000 message in queue
                    if( m_qMessagesIn.amount() < m_qMesInCapacity )
                    {
                        m_qMessagesIn.push_back( { nullptr, m_msgTempIn } );
                    }
                }
                
                ReadHeader();
            }

        private:
            uint64_t scramble( uint64_t nInput )
			{
				uint64_t out = nInput ^ 0xDEADBEEFC0DECAFE;
				out = (out & 0xF0F0F0F0F0F0F0) >> 4 | (out & 0x0F0F0F0F0F0F0F) << 4;
				return out ^ 0xC0DEFACE12345678;
			}

            void WriteValidation()
            {
                asio::async_write( m_socket, asio::buffer( &m_nHandshakeOut, sizeof( uint64_t ) ),
                    [this] ( std::error_code ec, std::size_t lenght )
                    {
                        if( !ec )
                        {
                            if( owner_type == owner::client )
                            {
                                ReadHeader();
                            }
                        }
                        else
                        {
                            m_socket.close();
                        }
                    }
                );
            }

            void ReadValidation( tie::net::server_interface<T>* server = nullptr )
            {
                asio::async_read( m_socket, asio::buffer( &m_nHandshakeIn, sizeof(uint64_t) ),
                    [this, server] ( std::error_code ec, std::size_t lenght )
                    {
                        if( !ec )
                        {
                            if( owner_type == owner::server )
                            {
                                if( m_nHandshakeIn == m_nHandshakeCheck )
                                {
                                    std::cout << "Client Validated" << std::endl;
                                    server->OnClientValidated( this->shared_from_this() );
                                
                                    //Waiting something send from client
                                    ReadHeader();
                                }
                                else
                                {
                                    std::cout << "Client Disconnect (Fail Validation)" << std::endl;
                                    m_socket.close();
                                }   
                            }
                            else
                            {
                                m_nHandshakeOut = scramble( m_nHandshakeIn );

                                //Write result to validate with server
                                WriteValidation();
                            }
                        }
                        else
                        {
                            std::cout << "ReadValidate_ERR: " << '\n';
                            m_socket.close();
                        }
                    }
                );
            }

        private:
            //Just one context for asio connect
            asio::io_context& m_context;

            asio::ip::tcp::socket m_socket;

            tsqueue<message<T>> m_qMessagesOut;

            //owner_message come from client or server will be reference
            tsqueue<owner_message<T>>& m_qMessagesIn; 
            uint32_t m_qMesInCapacity;

            message<T> m_msgTempIn;

            owner owner_type = owner::server;
            uint32_t id = 0;

            //Handshake
            uint64_t m_nHandshakeOut = 0;
            uint64_t m_nHandshakeIn = 0;
            uint64_t m_nHandshakeCheck = 0;
        };
    }
}
