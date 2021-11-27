#pragma once

#include "macros.h"

class c_panel
{
public:
	vfunc(36, get_name(int panel), const char*(__thiscall*)(void*, int))(panel)
};

interface_var(c_panel, panel, "vgui2.dll", "VGUI_Panel")
