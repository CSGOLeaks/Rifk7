#pragma once

#include "framework/c_flow_layout.h"

class c_conf : public c_flow_layout
{
public:
	explicit c_conf(c_vector2d size = c_vector2d(600, 400));
};
