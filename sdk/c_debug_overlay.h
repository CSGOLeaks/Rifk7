#pragma once

#include "macros.h"

class c_debug_overlay
{
protected:
	~c_debug_overlay() = default;
public:
	virtual void add_entity_text_overlay(int ent_index, int line_offset, float duration, int r, int g, int b, int a, const char *format, ...) = 0;
	virtual void add_box_overlay(const c_vector3d& origin, const c_vector3d& mins, const c_vector3d& max, c_qangle const& orientation, int r, int g, int b, int a, float duration) = 0;
	virtual void add_sphere_overlay(const c_vector3d& origin, float radius, int theta, int phi, int r, int g, int b, int a, float duration) = 0;
	virtual void add_triangle_overlay(const c_vector3d& p1, const c_vector3d& p2, const c_vector3d& p3, int r, int g, int b, int a, bool no_depth_test, float duration) = 0;
private:
	virtual void a() = 0;
public:
	virtual void add_line_overlay(const c_vector3d& origin, const c_vector3d& dest, int r, int g, int b, bool no_depth_test, float duration) = 0;

	vfunc(24, add_capsule_overlay(c_vector3d& mins, c_vector3d& maxs, float radius, const int r,
		const int g, const int b, const int a, const float duration), void(__thiscall*)(
		c_debug_overlay*, c_vector3d&, c_vector3d&, float&, int, int, int, int, float))(mins, maxs, radius, r, g, b, a, duration)
};

interface_var(c_debug_overlay, debug_overlay, "engine.dll", "VDebugOverlay")
