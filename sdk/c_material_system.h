#pragma once

#include "c_model_render.h"

class c_material_system
{
public:
	vfunc(84, find_material(const char* material, const char* group = nullptr), c_material*(__thiscall*)(c_material_system*, const char*, const char*, bool, const char*))(material, group, true, nullptr)
};

interface_var(c_material_system, material_system, "materialsystem.dll", "VMaterialSystem")

#define find_mat(name) material_system()->find_material(_(name))
