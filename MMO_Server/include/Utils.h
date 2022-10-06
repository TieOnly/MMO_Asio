#pragma once

namespace tie
{
namespace make
{
    tie::net::message<GameMsg> MakeMsgChat( const uint32_t& nPlayerID, const std::string& text );
    
    void MakeArrChar( char *arr_dest, const int& arr_dest_limit, const std::string& str_src );
}
class var
{
protected:
    uint32_t IDHost = -1u;
};
}
