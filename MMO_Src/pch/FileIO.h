#pragma once
#include <fstream>

class FileIO
{
private:
    void putData( std::ofstream& out, std::string& data )
    {
        for( char c : data )
        {
            out.put( c );
        }
    }
    void getData( std::ifstream& in, std::string& dest )
    {
        for( char c = in.get(); in.good(); c = in.get() )
        {
            dest += c;
        }
    }
public:
    bool SaveToFile( const std::string& fileName )
    {
        std::ofstream out( fileName.c_str() );
        if( !out ) return 0;
        
        putData( out, buff );
        return 1;
    }
    bool LoadFromFile( const std::string& fileName )
    {
        std::ifstream in( fileName.c_str());
        if( !in ) return 0;

        getData( in, buff );
        return 1;
    }
    void NewBuff()
    {
        buff.clear();
    }
    void Append(std::string& text )
    {
        buff += text;
    }
    void Append(const std::string& text )
    {
        buff += text;
    }
    const std::string& GetBuff() const
    {
        return buff;
    }
private:
    std::string buff;
};