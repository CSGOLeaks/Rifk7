#pragma once

#include <utility>
#include "../utils/intrinsics.h"

class c_vector2d
{
public:
	float x, y;

	c_vector2d() : c_vector2d(0.f, 0.f) { }
	c_vector2d(const float x, const float y) : x(x), y(y) { }
	~c_vector2d() = default;

	c_vector2d operator+(const c_vector2d& v) const
	{
		return c_vector2d(x + v.x, y + v.y);
	}

	c_vector2d operator-(const c_vector2d& v) const
	{
		return c_vector2d(x - v.x, y - v.y);
	}

	c_vector2d operator*(const c_vector2d& v) const
	{
		return c_vector2d(x * v.x, y * v.y);
	}

	c_vector2d operator/(const c_vector2d& v) const
	{
		return c_vector2d(x / v.x, y / v.y);
	}

	c_vector2d operator*(const float v) const
	{
		return c_vector2d(x * v, y * v);
	}

	c_vector2d operator/(const float v) const
	{
		return c_vector2d(x / v, y / v);
	}

	c_vector2d operator+=(const c_vector2d& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	c_vector2d operator-=(const c_vector2d& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	c_vector2d operator*=(const c_vector2d& other)
	{
		x *= other.x;
		y *= other.y;
		return *this;
	}

	c_vector2d operator/=(const c_vector2d& other)
	{
		x /= other.x;
		y /= other.y;
		return *this;
	}

	c_vector2d operator*=(const float other)
	{
		x *= other;
		y *= other;
		return *this;
	}

	c_vector2d operator/=(const float other)
	{
		x /= other;
		y /= other;
		return *this;
	}

	c_vector2d floor()
	{
		x = std::floor(x);
		y = std::floor(y);
		return *this;
	}

	float length() const
	{
		m128 tmp;
		tmp.f[0] = x * x + y * y;
		const auto calc = sqrt_ps(tmp.v);
		return reinterpret_cast<const m128*>(&calc)->f[0];
	}
};

typedef std::pair<c_vector2d, c_vector2d> rectangle;
