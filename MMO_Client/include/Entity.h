#pragma once

class Entity
{
public:
    enum class StateEvent
    {
        Normal = 0,
        Disable,
        Hover,
        Clicked,
        Released
    };
public:
    Entity( const RectF& dest );
    void ProcessInput();
    
    void SetState( const StateEvent& state );
    bool IsCover( const Vec2& pos ) const;
    const RectF& GetDest() const;
    const StateEvent& GetStateEvent() const;
    const Vec2& GetPosBegin() const;
    const bool IsValidPosBegin() const;

    void DrawLayout( Color c = WHITE_TRANS ) const;
    void DrawOutline( Color c = SKYBLUE ) const;
    void DrawFill( Color c = RAYWHITE ) const;
    void DrawSurfInside( const Texture2D* surface ) const;
    void DrawTitle( const std::string& title ) const;

    void SetDest( const RectF& dest );
    // void SetOffset();
    // void SetPadding();
    // void SetFontSize();

protected:
    int id = 0;
    RectF dest{};
    std::string content = "";
    Vec2 offset = { 0.0f, 0.0f};
    Vec2 padding = { 0.0f, 0.0f };
    Vec2 mousePos = { 0.0f, 0.0f };
    Vec2 pos_begin = {-1.0, 0.0f};

    StateEvent curState = StateEvent::Normal;
    float fontSize = 20.0f;
    float lineThick = 2.0f;
    Color colorLayout = WHITE_TRANS;
    Color colorLine = SKYBLUE;
    Color colorFill = RAYWHITE;

    bool isNew = false;
    Vec2 originFirst{};
};