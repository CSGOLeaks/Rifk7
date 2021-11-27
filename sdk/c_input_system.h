#pragma once

#include "macros.h"

class c_input_system
{
public:
	vfunc(11, enable_input(bool active), void(__thiscall*)(c_input_system*, bool))(active)
	vfunc(39, reset_input_state(), void(__thiscall*)(c_input_system*))()
};

interface_var(c_input_system, input_system, "inputsystem.dll", "InputSystemVersion")
