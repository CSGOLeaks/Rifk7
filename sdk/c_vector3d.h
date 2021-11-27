#pragma once

#include <cmath>
#include "../utils/intrinsics.h"

class c_vector3d
{
public:
	float x, y, z;

	__forceinline c_vector3d() : c_vector3d(0.f, 0.f, 0.f) { }

	__forceinline c_vector3d(const float x, const float y, const float z) : x(x), y(y), z(z) { }

	__forceinline c_vector3d operator+(const c_vector3d& v) const
	{
		return c_vector3d(x + v.x, y + v.y, z + v.z);
	}

	__forceinline c_vector3d operator-(const c_vector3d& v) const
	{
		return c_vector3d(x - v.x, y - v.y, z - v.z);
	}

	__forceinline c_vector3d operator*(const c_vector3d& v) const
	{
		return c_vector3d(x * v.x, y * v.y, z * v.z);
	}

	__forceinline c_vector3d operator/(const c_vector3d& v) const
	{
		return c_vector3d(x / v.x, y / v.y, z / v.z);
	}

	__forceinline c_vector3d operator*(const float v) const
	{
		return c_vector3d(x * v, y * v, z * v);
	}

	__forceinline c_vector3d operator/(const float v) const
	{
		return c_vector3d(x / v, y / v, y / v);
	}

	__forceinline c_vector3d operator+=(const c_vector3d& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	__forceinline c_vector3d operator-=(const c_vector3d& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	__forceinline c_vector3d operator*=(const c_vector3d& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}

	__forceinline c_vector3d operator/=(const c_vector3d& other)
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
		return *this;
	}

	__forceinline c_vector3d operator*=(const float other)
	{
		x *= other;
		y *= other;
		z *= other;
		return *this;
	}

	__forceinline c_vector3d operator/=(const float other)
	{
		x /= other;
		y /= other;
		z /= other;
		return *this;
	}

	__forceinline float length() const
	{
		m128 tmp;
		tmp.f[0] = x * x + y * y + z * z;
		const auto calc = sqrt_ps(tmp.v);
		return reinterpret_cast<const m128*>(&calc)->f[0];
	}

	__forceinline float length2d() const
	{
		m128 tmp;
		tmp.f[0] = x * x + y * y;
		const auto calc = sqrt_ps(tmp.v);
		return reinterpret_cast<const m128*>(&calc)->f[0];
	}

	__forceinline float length2dsqr() const
	{
		return x * x + y * y;
	}

	__forceinline float dot(const c_vector3d& other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	__forceinline float dot(const float* other) const
	{
		return x * other[0] + y * other[1] + z * other[2];
	}

	__forceinline c_vector3d cross(const c_vector3d& other) const
	{
		return c_vector3d(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		);
	}

	__forceinline c_vector3d normalize()
	{
		const auto l = length();

		if (l > 0)
		{
			x /= l;
			y /= l;
			z /= l;
		}

		return *this;
	}

	__forceinline bool is_valid() const
	{
		return std::isfinite(this->x) && std::isfinite(this->y) && std::isfinite(this->z);
	}
};

class c_vector3d_aligned : public c_vector3d
{
	float w{};	
};

using c_qangle = c_vector3d;
