
#include <thread>
#include <numeric>
#include "idirect3ddevice9.h"
#include "../renderer/c_renderer.h"
#include "../hacks/c_esp.h"
#include "../menu/c_menu.h"
#include "../hacks/c_hitmarker.h"
#include "../hacks/c_chams.h"
#include "../renderer/c_indicators.h"
#include "../utils/c_memory.h"

idirect3ddevice9::idirect3ddevice9() : default_renderer(std::make_unique<c_renderer>(direct_device()))
{
	static c_hook<IDirect3DDevice9> hook(direct_device());
	_reset = hook.apply<reset_t>(16, reset);
	_end_scene = hook.apply<end_scene_t>(42, end_scene);
}

c_renderer* idirect3ddevice9::get_renderer() const
{
	return default_renderer.get();
}

IDirect3DDevice9* idirect3ddevice9::direct_device()
{
	static const auto direct_device = **reinterpret_cast<IDirect3DDevice9***>(sig("shaderapidx9.dll", "A1 ? ? ? ? 50 8B 08 FF 51 0C") + 1);
	return direct_device;
}

HRESULT idirect3ddevice9::reset(IDirect3DDevice9* dev, D3DPRESENT_PARAMETERS* param)
{
	instance()->get_renderer()->invalidate_device_objects();
	const auto result = instance()->_reset(dev, param);
	instance()->get_renderer()->init_device_objects(dev);
	return result;
}

HRESULT idirect3ddevice9::end_scene(IDirect3DDevice9* dev)
{
	static const auto base_shader = int32_t(memory::get_module_handle(fnv1a("shaderapidx9.dll")));
	const auto diff = std::abs(int32_t(_ReturnAddress()) - base_shader);
	
	if (diff > 1000000)
		return instance()->_end_scene(dev);
	
	c_chams::instance()->latch_timer();

	IDirect3DVertexDeclaration9* decl = nullptr;
	IDirect3DVertexShader9* shader = nullptr;
	IDirect3DStateBlock9* block = nullptr;
	dev->GetVertexDeclaration(&decl);
	dev->GetVertexShader(&shader);
	dev->CreateStateBlock(D3DSBT_PIXELSTATE, &block);
	
	instance()->get_renderer()->setup_render_state();
	c_esp::draw();
	c_hitmarker::draw();
	c_indicators::draw();
	menu->draw();

	block->Apply();
	block->Release();
	dev->SetVertexShader(shader);
	dev->SetVertexDeclaration(decl);

	return instance()->_end_scene(dev);
}
