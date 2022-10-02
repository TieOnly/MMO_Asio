#pragma once
#include "raylib.h"
#include <vector>
#include <string>
#include <algorithm>

class Surface
{
private:
    class Entry
    {
    public:
        Entry( Texture2D* pSurf_in, const std::string& key_in )
            :
            key( key_in ),
            pSurf( pSurf_in )
        {}
        std::string key;
        Texture2D* pSurf;
    };
public:
    static Texture2D* Retrieve( const std::string& key_in )
    {
        return Get()._Retrieve( key_in );
    }
    static void Purge()
    {
        Get()._Purge();
    }
private:
    Surface() = default;
    ~Surface()
    {
        for( auto& e : entries )
        {
            delete e.pSurf;
        }
    }
    Texture2D* _Retrieve( const std::string& key_in )
    {
        const auto i = std::find_if( entries.begin(), entries.end(),
            [&key_in]( const Entry& e )
            {
                return e.key == key_in;
            } );

        if(i == entries.end())
        {
            auto p = new Texture2D( LoadTexture( key_in.c_str() ) );
            entries.emplace_back( Entry{ p, key_in } );
            return p;
        }
        else
        {
            return i->pSurf;
        }
    }
    void _Purge()
    {
        for( auto& e : entries )
        {
            delete e.pSurf;
        }
        entries.clear();
    }
    static Surface& Get() 
    {
        static Surface Surface;
        return Surface;
    }
private:
    std::vector<Entry> entries;
}; 