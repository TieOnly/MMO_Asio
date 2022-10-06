#pragma once
#include "net_common.h"

namespace tie
{
    namespace net
    {
        template <typename T>
        struct message_header
        {
            T id{};
            uint32_t size;
        };
        
        template <typename T>
        struct message
        {
            //Note: Must default contructor for header, this is relative with asio::buff();
            message_header<T> header{};
            std::vector<uint8_t> body;

            size_t size()
            {
                return body.size();
            }

            friend std::ostream& operator << (std::ostream& os, const message<T>& msg)
            {
                os << "ID: " << int(msg.header.id) << ", Size: " << int(msg.header.size);
                return os;
            }

            template<typename DataType>
            //Extract from DataType to message
            friend message<T>& operator << (message<T>& msg, const DataType& data)
            {
                static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to extract");
                
                size_t i = msg.body.size();

                msg.body.resize( i + sizeof(DataType) );

                std::memcpy( msg.body.data() + i, &data, sizeof(DataType) );

                msg.header.size = msg.size();

                return msg;
            }

            template<typename DataType>
            //Extract to DataType with order from message
            friend message<T>& operator >> (message<T>& msg, DataType& data)
            {
                static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to extract");
                
                size_t i = msg.body.size() - sizeof(DataType);
                if( (int)i >= 0 )
                {
                    std::memcpy( &data, msg.body.data() + i, sizeof(DataType) );

                    msg.body.resize( i );

                    msg.header.size = msg.size();
                }
                
                return msg;
            }
        };

        template <typename T>
        class connection;

        template <typename T>
        struct owner_message
        {
            std::shared_ptr<connection<T>> remote = nullptr;
            message<T> msg;

            friend std::ostream& operator << (std::ostream& os, const owner_message<T>& o_msg)
            {
                os << o_msg.msg;
                return os;
            }
        };
    }
}