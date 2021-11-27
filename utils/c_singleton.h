#pragma once

template<typename t>
class c_singleton {
public:
	template<typename... Args>
    static t* instance(Args&&... args);
protected:
	c_singleton() = default;
	~c_singleton() = default;
};

template<typename t>
template<typename... Args>
t* c_singleton<t>::instance(Args&&... args)
{
	static t instance(std::forward<Args>(args)...);
    return &instance;
}
