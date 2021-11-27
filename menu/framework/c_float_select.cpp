#include "c_float_select.h"
#include "../../security/string_obfuscation.h"
#include <sstream>
#include "c_text.h"

#undef min
#undef max

c_float_select::c_float_select(const std::pair<std::string, char> title, float& selected, const float max, const float min, const float step)
	: c_select(title, {}, currently_selected, true), min(min), max(max), currently_selected(0), float_selected(selected), step(step)
{
	std::stringstream str;
	constexpr auto key = random::_char<__COUNTER__>::value;

	const auto total = static_cast<int>((max - min) / step);

	for (auto x = 0; x <= total; x++)
	{
		str << std::fixed << std::setprecision(2) << min + x * step;
		
		auto txt = str.str();

		for (auto& c : txt)
			c ^= key;

		options.emplace_back(txt, key);
		str.str(std::string());
	}

	currently_selected = std::clamp(static_cast<int>((selected - min) / step), 0, total);
	reinterpret_cast<c_text*>(this->children[2].first.get())
				->set_text(this->options[currently_selected]);
}

void c_float_select::draw(const c_vector2d position)
{
	currently_selected = std::clamp(static_cast<int>((float_selected - min) / step), 0, static_cast<int>((max - min) / step));
	c_select::draw(position);
}

void c_float_select::on_change()
{
	float_selected = min + currently_selected * step;
}
