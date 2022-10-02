#pragma once

namespace settings
{
    inline constexpr int scale = 3;
    inline constexpr int screenW = 300 * scale;
    inline constexpr int screenH = screenW / 16 * 9;
    inline constexpr int fps = 60;

    inline constexpr float camZoom = 1.0f;  
    inline constexpr float vision = 180.0f / camZoom;

    inline constexpr float tileSize = 42.0f;
    inline constexpr float deltaTopTile = 7.0f;
}