
#include "c_global_vars_base.h"
#include "../utils/c_memory.h"

c_global_vars_base* c_global_vars_base::get()
{
	static const auto globals = **reinterpret_cast<c_global_vars_base***>(sig("client_panorama.dll", "A1 ? ? ? ? 5E 8B 40 10") + 1);
	return globals;
}
