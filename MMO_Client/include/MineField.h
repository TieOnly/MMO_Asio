#pragma once
#include "DrawSprites.h"

class MineField
{
private:
    class Tile
    {
    public:
        enum class State
        {
            Hidden,
            Flag,
            Revealed
        };
    public:
        void SetIsBoom();
        void SetIsRevealed();
        void FlagToggle();
        bool HasBoom() const;
        bool IsRevealed() const;
        bool IsFlag() const;
        void CountNeightborBoom();
        bool IsNoBoomNeightbor() const;
        void Draw(const Vei2& screenPos) const;
    private:
        State state = State::Hidden;
        int count = 0;
        bool hasBoom = false;
    };

public:
    MineField(const int width, const int height, const int nBooms);
    ~MineField();
    void DoRevealedClick( const Vei2& gridPos );
    void DoFlagClick( const Vei2& gridPos );
    Vei2 ScreenToGrid( const Vei2& screenPos );
    bool FuckUp();
    bool Done();
    void Draw();
private:
    void Sweeper(const Vei2& gridPos_in);
    bool NoNeightborBoom(const Vei2& gridPos);
    Tile& TileAt( const Vei2& gridPos );
private:
    int width;
    int height;
    int nBooms = 6;
    static constexpr int wTile = 20;
    int widthFeild = width*wTile;
    int heightFeild = height*wTile;
    static constexpr int wallThick = 4;
    Vei2 pos = {settings::screenW / 2 - (widthFeild/2), settings::screenH / 2 - (heightFeild/2)};
    int nFlagUsed = 0;
    int nRevealed = 0;
    bool isFuckUp = false;
    Tile* pTile = nullptr;
};