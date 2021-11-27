#pragma once

#include <optional>
#include <vector>
#include <memory>
// ReSharper disable once CppUnusedIncludeDirective
#include "macros.h"
#include "../../sdk/c_vector2d.h"

class c_drawable
{
public:
	explicit c_drawable(const c_vector2d size) : size(size), children({ }) { }
	virtual ~c_drawable() = default;
	virtual void draw(c_vector2d position) = 0;

	virtual c_vector2d get_size()
	{
		return size;
	}

	virtual void animate()
	{
		for (auto& child : children)
			child.first->animate();
	}

	std::vector<std::pair<std::shared_ptr<c_drawable>, rectangle>>& get_children()
	{
		return children;
	}

	void add_child(const std::shared_ptr<c_drawable> drawable)
	{
		children.push_back({ drawable, { } });
	}
	
	void add_child(std::initializer_list<std::shared_ptr<c_drawable>> drawables)
	{
		for (const auto& drawable : drawables)
			add_child(drawable);
	}

	virtual void on_mouse_enter()
	{
		hovered = true;
	}

	virtual void on_mouse_leave()
	{
		hovered = false;
	}

	virtual bool is_hovered()
	{
		return hovered && is_enabled;
	}

	virtual void on_click()
	{
		held = true;
	}

	virtual void on_unclick()
	{
		held = false;
	}

	virtual void on_hold() { }

	virtual void set_enabled(const bool enabled)
	{
		is_enabled = enabled;
	}

	virtual std::optional<c_drawable*> get_foreground_element_by_position(c_vector2d)
	{
		return std::optional<c_drawable*>(this);
	}

	c_vector2d size;
	std::vector<std::pair<std::shared_ptr<c_drawable>, rectangle>> children;
	bool hovered = false, held = false, is_enabled = true;
};
