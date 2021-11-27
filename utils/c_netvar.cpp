#include "c_netvar.h"
#include "../sdk/c_base_client.h"

#undef netvars

c_netvar::c_netvar()
{ 
	for (auto data = base_client()->get_all_classes(); data; data = data->next)
		write_prop_to_map(data->network_name, data->recv_table);
}

void c_netvar::write_prop_to_map(const char* name, recv_table* table, const size_t offset)
{
	static auto is_valid_prop = [](recv_prop* prop) -> bool {
		return prop->type == DPT_Float
			|| prop->type == DPT_Int
			|| prop->type == DPT_String
			|| prop->type == DPT_Vector
			|| prop->type == DPT_VectorXY;
	};

	for (auto i = 0; i < table->count; ++i)
	{
		const auto current = &table->props[i];

		if (isdigit(current->name[0]))
			continue;

		if (current->data_table != nullptr && current->data_table->table_name[0] == 'D')
			write_prop_to_map(name, current->data_table, current->offset + offset);

		if (netvars[fnv1a_rt(name)][fnv1a_rt(current->name)].offset <= 0 && is_valid_prop(current))
			netvars[fnv1a_rt(name)][fnv1a_rt(current->name)] = { static_cast<size_t>(current->offset) + offset, current };
	}
}
