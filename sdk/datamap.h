#pragma once

#include <cstdint>

struct inputdata_t;
typedef enum fieldtypes
{
	field_void = 0,
	field_float,
	field_string,
	field_vector,
	field_quaternion,
	field_integer,
	field_boolean,
	field_short,
	field_character,
	field_color32,
	field_embedded,
	field_custom,
	field_classptr,
	field_ehandle,
	field_edict,
	field_position_vector,
	field_time,
	field_tick,
	field_modelname,
	field_soundname,
	field_input,
	field_function,
	field_vmatrix,
	field_vmatrix_worldspace,
	field_matrix3_x4_worldspace,
	field_interval,
	field_modelindex,
	field_materialindex,
	// ReSharper disable once CppInconsistentNaming
	field_vector2d,
	field_typecount
} fieldtype_t;

class c_save_restore_ops;
class c_base_entity;
typedef void (c_base_entity::*inputfunc_t)(inputdata_t &data);

struct datamap_t;
class typedescription_t;

enum
{
	td_offset_normal = 0,
	td_offset_packed = 1,
	td_offset_count
};

class typedescription_t
{
public:
	int32_t field_type;
	char* field_name;
	int field_offset[td_offset_count];
	int16_t field_size_unknwn;
	int16_t flags_unkwn;
	char pad_0014[12];
	datamap_t* td;
	char pad_0024[24];
};

struct datamap_t
{
	typedescription_t* data_desc;
	int data_num_fields;
	char const* data_class_name;
	datamap_t* base_map;

	bool chains_validated;
	bool packed_offsets_computed;
	int packed_size;
};
