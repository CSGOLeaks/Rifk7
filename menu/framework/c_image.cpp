#include "c_image.h"
#include "../../hooks/idirect3ddevice9.h"

c_image::c_image(const c_vector2d size, void* image_data, const uint32_t image_size, const float scale)
	: c_drawable(size), image(nullptr), sprite(nullptr), image_data(image_data), image_size(image_size), scale(scale)
{
	renderer->register_reset_handler([&]() -> void
	{
		safe_release(image);
		safe_release(sprite);
	});
}

void c_image::draw(const c_vector2d position)
{
	if (!image)
		renderer->create_texture(image_data, image_size, &image);

	if (!sprite)
		renderer->create_sprite(&sprite);

	renderer->image(position, image, sprite, scale, c_color::get_alpha_override() / 255.f);
}
