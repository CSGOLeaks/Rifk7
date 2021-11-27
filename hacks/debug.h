#pragma once

#include "../sdk/c_user_cmd.h"
#include "../utils/math.h"
#include "c_animation_system.h"

namespace debug
{
	inline void spin_on_key(c_user_cmd* cmd)
	{
		static auto counter = 0.f;
		if (GetAsyncKeyState(VK_XBUTTON1))
		{
			counter = fmod(counter + 80.f, 360.f);
			cmd->viewangles.y = counter;
		}
	}
}
