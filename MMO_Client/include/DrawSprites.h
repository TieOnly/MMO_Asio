#pragma once

namespace DrawSprites
{
    void DrawHidden(const Vei2& screenPos, const int& wTile);
    void DrawFlag(const Vei2& screenPos, const int& wTile);
    void DrawRevealed(const Vei2& screenPos, const int& wTile);
    void DrawRevealedWithNum(const Vei2& screenPos, const int& count, const int& wTile);
    void DrawBoom(const Vei2& screenPos, const int& wTile);
}