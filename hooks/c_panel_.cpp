#include "c_panel_.h"
#include "../utils/c_hook.h"
#include "../utils/c_config.h"

void c_panel_::hook()
{
	static c_hook<c_panel> hook(panel());
	_paint_traverse = hook.apply<paint_traverse_t>(41, paint_traverse);
}

void __fastcall c_panel_::paint_traverse(c_panel* panel, uint32_t, uint32_t panel_nr, bool force_repaint, bool allow_force)
{
	if (config.misc.no_scope && fnv1a_rt(panel->get_name(panel_nr)) == fnv1a("HudZoom"))
		return;

	_paint_traverse(panel, panel_nr, force_repaint, allow_force);
}
