#pragma once
#include "pch.h"
#include "raylib.h"

#include "Settings.h"
#include "Rect.h"
#include "Vec2.h"
#include "FileIO.h"
#include "Surface.h"

namespace rayCpp
{
    void DrawSprite(const Texture2D& s, const Vec2& pos);
    void DrawSprite(const Texture2D& s, const Vec2& pos, const RectF& srcRect);
    void DrawSprite(const Texture2D& s, const Vec2& pos, const RectF& srcRect, float scale);
    void DrawSprite(const Texture2D& s, const Vec2& pos, const RectF& srcRect, Color color);
    void DrawSprite(const Texture2D& s, const Vec2& pos, const RectF& srcRect, const float wDest, const float hDest);
    void DrawSprite(const Texture2D& s, const Vec2& pos, const RectF& srcRect, const float wDest, const float hDest, Color c);
    void DrawSprite(const Texture2D& s, const Vec2& pos, const RectF& srcRect, const RectF& dest, const Vec2& origin, const float rotation, Color color);
    void DrawSpriteInside(const Texture2D& s, const Vec2& pos, const RectF& srcRect, const RectF& dest, const RectF& boundary );
    void DrawRectThin(const RectF& rect, const Color& color);
    void DrawRectThin(const RectF& rect, const Color& color, const float lineThick);
    void DrawRectPoint(const Vec2& pos, const Color& color);
    void DrawRectFill(const RectF& rect, const Color& color);
    void DrawCircleThin(const Vec2& pos, const float radius, Color color);
    void DrawLayout( const RectF& rectInside, Color c);
    void DrawString( const std::string& str, const Vec2& pos, const int fontSize, Color c);
    void DrawStrCenter( const std::string& str, const RectF& dest, const int fontSize, Color c);

    template <typename V>
    inline void remove_element(std::vector<V>& vec, size_t index )
    {
        std::swap( vec[index], vec.back() );
        vec.pop_back();
    }

    RectF GetScreenRect();
    Vec2 GetMousePos();
    RectF GetSrcRect( const Texture2D* surface );
    Vec2 CreateVecByData( const std::string& data, const char delim = ' ' );
    RectF CreateRectFByData( const std::string& data, const char delim = ' ' );
    Vec2 MakePosNextStr( const Vec2& pos_str, const std::string& str, const int fontsize = 20, const float distance = 20 );
    bool CheckCollidedRectCir( const RectF& rect, const Vec2& pos_cir, const float radius );

    int BinarySerch_IdInVei2( const std::vector<Vei2>& vei, const Vei2 val );
    std::vector<int> BS_PosInBoundary( const std::vector<RectF>& listBox, const RectF& boundary );
}