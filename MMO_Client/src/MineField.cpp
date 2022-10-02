#include "raylibCpp.h"
#include "MineField.h"

MineField::MineField(const int width, const int height, const int nBooms)
    :
    width( width ),
    height( height ),
    nBooms( nBooms ),
    pTile( new Tile[width * height] )
{
    std::random_device rd;
    std::mt19937 rng( rd() );
    std::uniform_int_distribution<int> xDist( 0, width - 1 );
    std::uniform_int_distribution<int> yDist( 0, height - 1 );

    Vei2 gridPos;
    for(int i = 0; i < nBooms; i++)
    {
        do {
            gridPos = { xDist( rng ), yDist( rng ) };
        } while ( TileAt( gridPos ).HasBoom() );

        TileAt( gridPos ).SetIsBoom();
    }
}
MineField::~MineField()
{
    delete [] pTile;
    pTile = nullptr;
}
bool MineField::NoNeightborBoom(const Vei2& gridPos)
{
    const int xStart = std::max( 0, gridPos.x - 1 );
    const int yStart = std::max( 0, gridPos.y - 1 );
    const int xEnd = std::min( width - 1, gridPos.x + 1 );
    const int yEnd = std::min( height - 1, gridPos.y + 1 );

    Tile& tile = TileAt( gridPos );

    for(Vei2 gridPos{xStart, yStart}; gridPos.y <= yEnd; gridPos.y++)
    {
        for(gridPos.x = xStart; gridPos.x <= xEnd; gridPos.x++)
        {
            if(TileAt( gridPos ).HasBoom())
            {
                tile.CountNeightborBoom();
            }
        }
    }

    tile.SetIsRevealed();
    nRevealed++;

    return tile.IsNoBoomNeightbor();
}
void MineField::Sweeper(const Vei2& gridPos_in) 
{
    const int xStart = std::max( 0, gridPos_in.x - 1 );
    const int yStart = std::max( 0, gridPos_in.y - 1 );
    const int xEnd = std::min( width - 1, gridPos_in.x + 1 );
    const int yEnd = std::min( height - 1, gridPos_in.y + 1 );

    for(Vei2 gridPos{xStart, yStart}; gridPos.y <= yEnd; gridPos.y++)
    {
        for(gridPos.x = xStart; gridPos.x <= xEnd; gridPos.x++)
        {
            if(!TileAt( gridPos ).IsRevealed())
            {
                if(NoNeightborBoom( gridPos )) Sweeper( gridPos );
            }
        }
    }
}
void MineField::DoRevealedClick( const Vei2& gridPos )
{
    // assert(gridPos.x >= 0 && gridPos.x < width && gridPos.y >= 0 && gridPos.y < height);
    if(gridPos.x >= 0 && gridPos.x < width && gridPos.y >= 0 && gridPos.y < height)
    {
        Tile& tile = TileAt( gridPos );
        if( !tile.IsRevealed() &&  !tile.IsFlag())
        {
            if(tile.HasBoom())
            {
                isFuckUp = true;
            }
            else if (NoNeightborBoom( gridPos ))
            {
                Sweeper( gridPos );
            }
        }
    }
}
void MineField::DoFlagClick( const Vei2& gridPos )
{
    // assert(gridPos.x >= 0 && gridPos.x < width && gridPos.y >= 0 && gridPos.y < height);
    Tile& tile = TileAt( gridPos );
    if(!tile.IsRevealed())
    {
        tile.FlagToggle();
        if(tile.IsFlag())
        {
            nFlagUsed++;
        }
        else 
        {
            nFlagUsed--;
        }
    }
}
Vei2 MineField::ScreenToGrid( const Vei2& screenPos )
{
    Vei2 posConvert = Vei2{ screenPos - pos };
    if(posConvert.x >= 0 && posConvert.y >= 0 && posConvert < Vei2{ widthFeild, heightFeild})
    {
        return posConvert / wTile;
    }
    else
    {
        return Vei2{ -1, -1 };
    }
}
bool MineField::FuckUp()
{
    if(isFuckUp)
    {
        for(Vei2 gridPos{0, 0}; gridPos.y < height; gridPos.y++)
        {
            for(gridPos.x = 0; gridPos.x < width; gridPos.x++)
            {
                TileAt( gridPos ).SetIsRevealed();
            }
        }
        return true;
    }
    else
    {
        return false;
    }
}
bool MineField::Done()
{
    if(nFlagUsed + nRevealed == (width * height) && nFlagUsed == nBooms)
    {
        return true;
    }
    else
    {
        return false;
    }
}
void MineField::Draw()
{
    for(Vei2 gridPos{0, 0}; gridPos.y < height; gridPos.y++)
    {
        for(gridPos.x = 0; gridPos.x < width; gridPos.x++)
        {
            TileAt( gridPos ).Draw( pos + (gridPos * wTile) );
        }
    }
    DrawRectangle(pos.x - wallThick, pos.y - wallThick, widthFeild + 2*wallThick + 2, wallThick, SKYBLUE);
    DrawRectangle(pos.x - wallThick, pos.y, wallThick, heightFeild + 2, SKYBLUE);
    DrawRectangle(pos.x + widthFeild + 2, pos.y, wallThick, heightFeild + 2, SKYBLUE);
    DrawRectangle(pos.x - wallThick, pos.y + heightFeild + 2, widthFeild + 2*wallThick + 2, wallThick, SKYBLUE);
}
MineField::Tile& MineField::TileAt( const Vei2& gridPos )
{
    return pTile[gridPos.y * width + gridPos.x];
}

void MineField::Tile::SetIsBoom()
{
    hasBoom = true;
}
void MineField::Tile::SetIsRevealed()
{
    state = State::Revealed;
}
void MineField::Tile::FlagToggle()
{
    if(state == State::Hidden)
    {
        state = State::Flag;
    }
    else
    {
        state = State::Hidden;
    }
}
bool MineField::Tile::IsRevealed() const
{
    return state == State::Revealed;
}
bool MineField::Tile::IsFlag() const
{
    return state == State::Flag;
}
bool MineField::Tile::HasBoom() const
{
    return hasBoom;
}
bool MineField::Tile::IsNoBoomNeightbor() const
{
    return count == 0;
}
void MineField::Tile::CountNeightborBoom()
{
    ++count;
}
void MineField::Tile::Draw(const Vei2& screenPos) const
{
    switch (state)
    {
    case State::Hidden:
        DrawSprites::DrawHidden(screenPos, wTile);
        break;    
    case State::Flag:
        DrawSprites::DrawFlag(screenPos, wTile);
        break;
    case State::Revealed:
        if(!HasBoom())
        {
            if(count > 0)
            {
                DrawSprites::DrawRevealedWithNum(screenPos, count, wTile);
            }
            else
            {
                DrawSprites::DrawRevealed(screenPos, wTile);
            }
        } 
        else
        {
            DrawSprites::DrawBoom(screenPos, wTile);
        }
        break;
    default:
        break;
    }
}
