#include "c_select.h"
#include "c_text.h"
#include "c_button.h"
#include "../../security/string_obfuscation.h"
#include "../../hooks/idirect3ddevice9.h"

c_select::c_select(std::pair<std::string, char> title, const std::vector<std::pair<std::string, char>> options,
				   int& selected, const bool holdable, const c_vector2d size)
	: c_flow_layout(c_vector2d(265, 14), c_vector2d(6, 0), true), options(options), selected(selected), holdable(holdable)
{
	add_child(std::initializer_list<std::shared_ptr<c_drawable>>{
		std::make_shared<c_text>(title, c_color::foreground()),
		std::make_shared<c_button>(__("<"), c_vector2d(6, 2), [&]() {
			std::lock_guard<std::mutex> lock(select_mutex);

			if (this->selected - 1 >= 0)
				this->selected--;

			reinterpret_cast<c_text*>(this->children[2].first.get())
				->set_text(this->options[this->selected]);

			on_change();
		}, [&]()
		{
			if (!this->holdable)
				return;

			static auto time = .0f;
			time += global_vars_base->frametime;

			if (time <= .010f)
				return;

			time = .0f;

			if (this->selected - 1 >= 0)
				this->selected--;
			
			reinterpret_cast<c_text*>(this->children[2].first.get())
				->set_text(this->options[this->selected]);

			on_change();
		}),
		std::make_shared<c_text>(!options.empty() ? options.front() : __(""), c_color::foreground(), fnv1a("pro13"), font_flags, true),
		std::make_shared<c_button>(__(">"), c_vector2d(6, 2), [&]() {
			std::lock_guard<std::mutex> lock(select_mutex);

			if (this->options.size() > this->selected + 1u)
				this->selected++;

			reinterpret_cast<c_text*>(this->children[2].first.get())
				->set_text(this->options[this->selected]);

			on_change();
		}, [&]()
		{
			if (!this->holdable)
				return;

			static auto time = .0f;
			time += global_vars_base->frametime;

			if (time <= .010f)
				return;

			time = .0f;

			if (this->options.size() > this->selected + 1u)
				this->selected++;

			reinterpret_cast<c_text*>(this->children[2].first.get())
				->set_text(this->options[this->selected]);

			on_change();
		})
	});

	this->children[2].first->size = size;
}

// Let's pretend this isn't here.
void c_select::draw(const c_vector2d position)
{
	std::lock_guard<std::mutex> lock(select_mutex);
	reinterpret_cast<c_text*>(this->children[2].first.get())
			->set_text(this->options[this->selected]);
	c_flow_layout::draw(position);
}
