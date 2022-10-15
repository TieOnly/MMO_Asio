#include "../../MMO_Src/pch/raylibCpp.h"
#include "Entity.h"

Entity::Entity( const RectF& dest )
    :
    dest( dest )
{}
bool Entity::IsCover( const Vec2& pos ) const
{
    return dest.IsCover( pos );
}
void Entity::ProcessInput()
{
    if( curState != StateEvent::Disable )
    {
        mousePos = rayCpp::GetMousePos();
        if( IsCover( mousePos ) )
        {
            if( isNew || IsMouseButtonUp( MOUSE_BUTTON_LEFT ) )
            {
                isNew = true;
                curState = StateEvent::Hover;
                if( IsMouseButtonDown( MOUSE_BUTTON_LEFT ) )
                {
                    curState = StateEvent::Released;
                    if( !IsValidPosBegin() ) pos_begin = mousePos;
                }
                else if( IsMouseButtonUp( MOUSE_BUTTON_LEFT ) )
                {
                    if( IsValidPosBegin() ) 
                    {
                        isNew = false;
                        pos_begin.x = -1.0f;
                        curState = StateEvent::Clicked;
                    }
                }
            }
        }
        else if( !IsMouseButtonDown( MOUSE_BUTTON_LEFT ) )
        {
            isNew = false;
            pos_begin.x = -1.0f;
            curState = StateEvent::Normal; 
        }
    }
}
void Entity::SetState( const StateEvent& state ) { curState = state; }
const RectF& Entity::GetDest() const { return dest; }
const Entity::StateEvent& Entity::GetStateEvent() const
{ 
    return curState; 
}
const bool Entity::IsValidPosBegin() const { return pos_begin.x != -1.0f; }
const Vec2& Entity::GetPosBegin() const { return pos_begin; }
void Entity::DrawLayout( Color c ) const
{
    rayCpp::DrawLayout( dest, c);
}
void Entity::DrawOutline( Color c ) const
{
    rayCpp::DrawRectThin( dest, c, lineThick );
}
void Entity::DrawFill( Color c ) const
{
    rayCpp::DrawRectFill( dest, c );
}
void Entity::DrawSurfInside( const Texture2D* surface ) const
{
    if( surface != nullptr )
    {
        rayCpp::DrawSprite( *surface, dest.topleft, rayCpp::GetSrcRect(surface), dest.width, dest.height );
    }
}
void Entity::DrawTitle( const std::string& title ) const
{
    
}

// void Entity::Custome()
// {
//     ProcessInput();

//     Vec2 delta = {0.0f, 0.0f};
//     if( curState == State::Released && rayCpp::GetScreenRect().IsCover( mousePos ) )
//     {
//         if( originFirst.x == -1.0f ) originFirst = mousePos;
//         if( mousePos.x != originFirst.x )
//         {
//             delta.x = mousePos.x - originFirst.x;
//         }
//         if( mousePos.y != originFirst.y )
//         {
//             delta.y = mousePos.y - originFirst.y;
//         }
//         dest = dest + RectF{ delta.x, delta.x, delta.y, delta.y };
//         originFirst = mousePos;
//     }
//     else originFirst.x = -1.0f;
    
// }
void Entity::SetDest( const RectF& new_dest ) { dest = new_dest; }
// void Entity::SetOffset()
// {}
// void Entity::SetPadding()
// {}