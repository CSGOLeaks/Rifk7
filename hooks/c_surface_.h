#pragma once

#include "../sdk/c_surface.h"

class c_surface_
{
	typedef void(__thiscall* lock_cursor_t)(c_surface*);
	typedef void(__thiscall* paint_traverse_t)(c_surface*, uint32_t panel, bool force_repaint, bool allow_force);

public:
	static void hook();

private:
	inline static lock_cursor_t _lock_cursor;
	inline static paint_traverse_t _paint_traverse;

	static void __fastcall lock_cursor(c_surface* surface, uint32_t);
	static void __fastcall paint_traverse(c_surface* surface, uint32_t, uint32_t panel_nr, bool force_repaint, bool allow_force);
};
