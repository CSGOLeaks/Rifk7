#pragma once

#include "c_flow_layout.h"
#include "c_image.h"
#include "../../hooks/idirect3ddevice9.h"

class c_navbar : public c_flow_layout
{
public:
	explicit c_navbar(std::shared_ptr<c_image> logo, std::vector<std::pair<std::string, char>> items,
					  c_vector2d size = c_vector2d(renderer->get_center().x * 2.f, 50));

	void draw(c_vector2d position) override;

	uint8_t& get_active();
private:
	uint8_t active_item;
};
