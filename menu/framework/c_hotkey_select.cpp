#include "c_hotkey_select.h"
#include "c_text.h"
#include "c_button.h"
#include "../../utils/c_hotkeys.h"

c_hotkey_select::c_hotkey_select(std::pair<std::string, char> title, int& selected)
	: c_flow_layout(c_vector2d(254, 14), c_vector2d(6, 0), true), selected(selected)
{
	add_child(std::initializer_list<std::shared_ptr<c_drawable>>{
		std::make_shared<c_text>(title, c_color::foreground()),
		std::make_shared<c_button>(__("< NONE >"), c_vector2d(2, 2), [this]() -> void
		{
			hotkeys->clear_updates(this);
			hotkeys->register_update(this, [this](const uint32_t code) -> void
			{
				if (!children[1].first->held)
					return;

				if (code == VK_INSERT || code == VK_ESCAPE)
					this->selected = 0;
				else
					this->selected = code;
			});
		})
	});

	this->children[1].first->size = c_vector2d(100, 10);
}

c_hotkey_select::~c_hotkey_select()
{
	hotkeys->clear_updates(this);
}

void c_hotkey_select::draw(const c_vector2d position)
{
	reinterpret_cast<c_button*>(this->children[1].first.get())
		->set_text(c_hotkeys::keycode_to_string(selected));

	c_flow_layout::draw(position);
}
