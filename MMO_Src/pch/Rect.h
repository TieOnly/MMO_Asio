#pragma once
#include "raylib.h"
#include "Vec2.h"

template <typename T>
class Rect_
{
public:
    Rect_() = default;
    Rect_(T left_in, T right_in, T top_in, T bottom_in)
        :
        left(left_in),
        right(right_in),
        top(top_in),
        bottom(bottom_in)
    {}
    Rect_(const Vec2_<T>& topLeft, const Vec2_<T>& bottomRight)
        :
        Rect_(topLeft.x, bottomRight.x, topLeft.y, bottomRight.y)
    {}
    Rect_(const Vec2_<T>& topLeft, const T& width, const T& height)
        :
        Rect_(topLeft, topLeft + Vec2_<T>{width, height})
    {}
    Rect_ operator + ( const Rect_& rhs ) const
    {
        return Rect_{ left + rhs.left, right + rhs.right, top + rhs.top, bottom + rhs.bottom };
    }
    Rect_ operator * ( const T& rate ) const
    {
        return Rect_{ left*rate, right*rate, top*rate, bottom*rate };
    }
    static Rect_ FromCenter(const Vec2_<T>& center, const T& halfWidth, const T& halfHeight)
    {
        return Rect_{ center - Vec2_<T>{ halfWidth, halfHeight }, center + Vec2_<T>{ halfWidth, halfHeight }};
    }
    bool IsOverLapping(const Rect_& other) const
    {
        return right > other.left && bottom > other.top 
            && left < other.right && top < other.bottom;
    }
    bool IsOutSide(const Rect_& other, const int _except = -1) const
    {
        switch (_except)
        {
        case 0:    //Except top
            return right < other.left || left > other.right || top > other.bottom;
        case 1:    //Except right
            return right < other.left || bottom < other.top || top > other.bottom;
        case 2:    //Except bottom
            return right < other.left || bottom < other.top || left > other.right;
        case 3:    //Except left
            return bottom < other.top || left > other.right || top > other.bottom;
        default:
            return right < other.left || bottom < other.top 
                || left > other.right || top > other.bottom;
        }
    }
    bool IsInsideOf(const Rect_& other) const
    {
        return right <= other.right && left >= other.left
            && top >= other.top && bottom <= other.bottom;
    }
    bool IsInsideExceptBottom(const Rect_& other) const
    {
        return right <= other.right && left >= other.left && top >= other.top;
    }
    bool IsCover(const Vec2_<T>& pos) const
    {
        return right >= pos.x && left <= pos.x
            && top <= pos.y && bottom >= pos.y;
    }
    Rectangle rayRect() const
    {
        return Rectangle{ (float)left, (float)top, (float)(right - left), (float)(bottom - top)};
    }
    const Vec2_<T> GetCenterPoint() const
    {
        return Vec2_<T>{ left + width/2, top + height/2 };
    }
public:
    T left = 0;
    T right = 0;
    T top = 0;
    T bottom = 0;
    T width = (right - left);
    T height = (bottom - top);
    Vec2_<T> topleft = Vec2_<T>{left, top};
};

typedef Rect_<int> RectI;
typedef Rect_<float> RectF;
