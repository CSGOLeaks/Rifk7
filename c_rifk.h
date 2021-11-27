#pragma once

#include "includes.h"

struct rifk_loader_info
{
	// not null terminated - use with caution
	char loader_path[MAX_PATH];
	char username[32];
	uint32_t days_remaining;
};
__declspec(dllexport) extern rifk_loader_info loader_info;

class c_rifk : public c_singleton<c_rifk>
{
public:
	c_rifk();
};

#define rifk c_rifk::instance()
