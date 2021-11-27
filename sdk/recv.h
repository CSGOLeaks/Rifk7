#pragma once

#include <cstdint>
#include "c_vector3d.h"

class recv_prop;
class recv_table;

enum send_prop_type : int
{
	DPT_Int = 0,
	DPT_Float,
	DPT_Vector,
	DPT_VectorXY,
	DPT_String,
	DPT_Array,
	DPT_DataTable,
	DPT_Int64,
	DPT_NUMSendPropTypes
};

struct d_variant
{
	~d_variant() = delete;

	union
	{
		float m_Float;
		long m_Int;
		char* m_pString;
		void* m_pData;
		c_vector3d m_Vector;
		int64_t m_Int64;
	};

	int m_Type;
};

struct c_recv_proxy_data
{
	~c_recv_proxy_data() = delete;

	const recv_prop* prop;
	d_variant value;
	int element;
	int id;
};

using fnv_t = unsigned;
using recv_var_proxy_fn = void(__cdecl *)(c_recv_proxy_data*, void*, void*);

class recv_prop
{
public:
	char* name;
	send_prop_type type;
	int flags;
	int size;
	int inside_array;
	const void* extra_data;
	recv_prop* prop;
	void* length_proxy;
	recv_var_proxy_fn proxy_fn;
	void* data_table_proxy_fn;
	recv_table* data_table;
	int offset;
	int element_stride;
	int count;
	const char* prop_name;
};

class recv_table
{
public:
	recv_prop* props;
	int count;
	void* decoder;
	char* table_name;
	bool initialized;
	bool in_main_list;
};
