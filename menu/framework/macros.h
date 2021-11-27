#pragma once

#include "../../sdk/c_global_vars_base.h"
#include <algorithm>

#undef min
#undef max

static auto linear_fade = [] (int32_t& current, const int32_t min, const int32_t max, const float frequency, const bool direction) -> void
{
	if (current < max && direction)
		current += static_cast<int>(std::ceil(frequency * global_vars_base->frametime));
	else if (current > min && !direction)
		current -= static_cast<int>(std::ceil(frequency * global_vars_base->frametime));
	
	current = std::clamp(current, min, max);
};

#define make_child(type, ...) std::static_pointer_cast<c_drawable>(std::make_shared<type>(__VA_ARGS__))
