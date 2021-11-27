#pragma once

#include "../sdk/c_panel.h"

class c_panel_
{
	typedef void(__thiscall* paint_traverse_t)(c_panel*, uint32_t panel, bool force_repaint, bool allow_force);

public:
	static void hook();

private:
	inline static paint_traverse_t _paint_traverse;

	static void __fastcall paint_traverse(c_panel* panel, uint32_t, uint32_t panel_nr, bool force_repaint, bool allow_force);
};
