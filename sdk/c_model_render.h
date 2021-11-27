#pragma once

#include "c_vector3d.h"
#include "c_client_entity.h"
#include "matrix.h"
#include "macros.h"
#include "c_color.h"
#include "c_vector2d.h"

struct model_t {
	void* handle;
	char name[255];
};

struct model_render_info_t {
	c_vector3d origin;
	c_qangle angles;
	char pad[0x4];
	void* renderable;
	const model_t* model;
	const matrix3x4* model_to_world;
	const matrix3x4* lighting_offset;
	const c_vector3d* lighting_origin;
	int flags;
	int entity_index;
	int skin;
	int body;
	int hitboxset;
	model_instance_handle_t instance;
};

class c_material_var
{
	vfunc(10, set_vector_internal(const float x, const float y), void(__thiscall*)(void*, float, float))(x, y)
	vfunc(11, set_vector_internal(const float x, const float y, const float z), void(__thiscall*)(void*, float, float, float))(x, y, z)

public:
	vfunc(4, set_float(const float val), void(__thiscall*)(void*, float))(val)
	vfunc(5, set_int(const int val), void(__thiscall*)(void*, int))(val)
	vfunc(6, set_string(char const* val), void(__thiscall*)(void*, char const*))(val)
	vfunc(20, set_matrix(viewmatrix& matrix), void(__thiscall*)(void*, viewmatrix&))(matrix)
	vfunc(26, set_vector_component(const float val, const int comp), void(__thiscall*)(void*, float, int))(val, comp)

	void set_vector(const c_vector2d vector)
	{
		set_vector_internal(vector.x, vector.y);
	}

	void set_vector(const c_vector3d vector)
	{
		set_vector_internal(vector.x, vector.y, vector.z);
	}
};

enum material_var_flags
{
	material_var_debug = 1 << 0,
	material_var_no_debug_override = 1 << 1,
	material_var_no_draw = 1 << 2,
	material_var_use_in_fillrate_mode = 1 << 3,
	material_var_vertexcolor = 1 << 4,
	material_var_vertexalpha = 1 << 5,
	material_var_selfillum = 1 << 6,
	material_var_additive = 1 << 7,
	material_var_alphatest = 1 << 8,
	material_var_znearer = 1 << 10,
	material_var_model = 1 << 11,
	material_var_flat = 1 << 12,
	material_var_nocull = 1 << 13,
	material_var_nofog = 1 << 14,
	material_var_ignorez = 1 << 15,
	material_var_decal = 1 << 16,
	material_var_envmapsphere = 1 << 17,
	material_var_envmapcameraspace = 1 << 19,
	material_var_basealphaenvmapmask = 1 << 20,
	material_var_translucent = 1 << 21,
	material_var_normalmapalphaenvmapmask = 1 << 22,
	material_var_needs_software_skinning = 1 << 23,
	material_var_opaquetexture = 1 << 24,
	material_var_envmapmode = 1 << 25,
	material_var_suppress_decals = 1 << 26,
	material_var_halflambert = 1 << 27,
	material_var_wireframe = 1 << 28,
	material_var_allowalphatocoverage = 1 << 29,
	material_var_alpha_modified_by_proxy = 1 << 30,
	material_var_vertexfog = 1 << 31
};

class c_material
{
	vfunc(11, find_var_internal(const char* name, bool* found), c_material_var*(__thiscall*)(c_material*, const char*, bool*, bool))(name, found, false)
	vfunc(27, alpha_modulate_internal(const float a), void(__thiscall*)(c_material*, float))(a)
	vfunc(28, color_modulate_internal(const float r, const float g, const float b), void(__thiscall*)(c_material*, float, float, float))(r, g, b)

public:
	vfunc(0, get_name(), const char*(__thiscall*)(c_material*))()
	vfunc(29, set_material_var_flag(const material_var_flags flag, const bool on), void(__thiscall*)(c_material*, material_var_flags, bool))(flag, on)
	vfunc(37, refresh(), void(__thiscall*)(c_material*))()

	void modulate(const c_color color)
	{
		color_modulate_internal(color.red / 255.f, color.green / 255.f, color.blue / 255.f);
		alpha_modulate_internal(color.alpha / 255.f);
	}

	c_material_var* find_var(const char* name)
	{
		bool found;
		const auto ret = find_var_internal(name, &found);

		if (found)
			return ret;

		return nullptr;
	}
};

class c_model_render {
public:
	vfunc(1, forced_material_override(c_material* material), void(__thiscall*)(c_model_render*, c_material*, int, int))(material, 0, 0)
};

interface_var(c_model_render, model_render, "engine.dll", "VEngineModel")
