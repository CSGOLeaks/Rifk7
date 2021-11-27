#pragma once

#include "recv.h"

class recv_prop_hook
{
public:
	explicit recv_prop_hook(recv_prop* prop)
	{
		property = prop;
	}

	recv_var_proxy_fn hook_prop(const recv_var_proxy_fn proxy_fn)
	{
		original_proxy_fn = property->proxy_fn;
		set_proxy_func(proxy_fn);
		return original_proxy_fn;
	}

	recv_var_proxy_fn get_orig() const
	{
		return original_proxy_fn;
	}

	void set_proxy_func(const recv_var_proxy_fn proxy_fn) const
	{
		property->proxy_fn = proxy_fn;
	}

private:
	recv_prop* property = nullptr;
	recv_var_proxy_fn original_proxy_fn = nullptr;
};
