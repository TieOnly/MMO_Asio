#pragma once
#include <cmath>

template <typename T>
class Vec2_
{
public:
    Vec2_() = default;
    constexpr Vec2_(T x_in, T y_in)
        :
        x(x_in),
        y(y_in)
    {}
    template <typename S>
    explicit constexpr Vec2_(const Vec2_<S>& src)
        :
        x( (T)src.x ),
        y( (T)src.y )
    {}
    constexpr T GetX() const {return x;}
    constexpr T GetY() const {return y;}
    constexpr void SetX(T x_in) {x = x_in;}
    constexpr void SetY(T y_in) {y = y_in;}
    
public:
    T x;
    T y;

public:
    constexpr bool operator == (const Vec2_& rhs) const
    {
        return (x == rhs.x && y == rhs.y);
    }
    constexpr bool operator != (const Vec2_& rhs) const
    {
        return !(*this == rhs);
    }
    constexpr bool operator == (const T& rhs) const
    {
        return (x == rhs && y == rhs);
    }
    constexpr bool operator != (const T& rhs) const
    {
        return !(*this == rhs);
    }
    constexpr bool operator < (const Vec2_& rhs ) const
    {
        return (x < rhs.x && y < rhs.y );
    }
    constexpr bool operator < (const T& rhs) const
    {
        return (x < rhs && y < rhs);
    }
    constexpr bool operator <= (const T& rhs) const
    {
        return (x <= rhs && y <= rhs);
    }
    constexpr bool operator > (const Vec2_& rhs ) const
    {
        return (x > rhs.x && y > rhs.y );
    }
    constexpr bool operator > (const T& rhs) const
    {
        return (x > rhs && y > rhs);
    }
    constexpr bool operator >= (const T& rhs) const
    {
        return (x >= rhs && y >= rhs);
    }
    constexpr Vec2_ operator + (const Vec2_& rhs) const
    {
        return {x + rhs.x, y + rhs.y};
    }
    constexpr Vec2_ operator + (const T& rhs) const
    {
        return {x + rhs, y + rhs};
    }
    constexpr Vec2_& operator += (const Vec2_& rhs) 
    {
        return (*this = *this + rhs);
    }
    constexpr Vec2_ operator - (const Vec2_& rhs) const
    {
        return {x - rhs.x, y - rhs.y};
    }
    constexpr Vec2_ operator - (const T& rhs) const
    {
        return {x - rhs, y - rhs};
    }
    constexpr Vec2_& operator -= (const Vec2_& rhs) 
    {
        return (*this = *this - rhs);
    }
    constexpr Vec2_ operator * (const Vec2_& rhs) const
    {
        return {x * rhs.x, y * rhs.y};
    }
    constexpr Vec2_ operator * (const T& rhs) const
    {
        return {x * rhs, y * rhs};
    }
    constexpr Vec2_& operator *= (const Vec2_& rhs) 
    {
        return (*this = *this * rhs);
    }
    constexpr Vec2_ operator / (const Vec2_& rhs) const
    {
        return {x / rhs.x, y / rhs.y};
    }
    constexpr Vec2_ operator / (const T& rhs) const
    {
        return {x / rhs, y / rhs};
    }
    constexpr Vec2_& operator /= (const Vec2_& rhs) 
    {
        return (*this = *this / rhs);
    }
    constexpr T GetLengthSq() const
    {
        return (T)(x * x + y * y);
    }
    constexpr T GetLength() const
    {
        return (T)std::sqrt( GetLengthSq() );
    }
    Vec2_& Nomarlize()
    {
        return *this = GetNormalize();
    }
    Vec2_ GetNormalize() const
    {
        const T len = GetLength();
        if( len != (T)0 )
        {
            return *this * ((T)1 / len);
        }
        return *this;
    }
};

typedef Vec2_<int> Vei2;
typedef Vec2_<float> Vec2;
