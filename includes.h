#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <Windowsx.h>
#include <cstdint>
#include <algorithm>
#include <memory>
#include <cctype>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <optional>
#include <intrin.h>

#include "d3d9/d3d9.h"
#include "d3d9/d3dx9.h"

#include "macros.h"

#include "sounds/sounds.h"

#include "renderer/c_font.h"

#include "menu/framework/c_drawable.h"

#include "sdk/c_client_entity_list.h"
#include "sdk/c_base_client.h"
#include "sdk/c_color.h"
#include "sdk/c_vector2d.h"
#include "sdk/c_vector3d.h"
#include "sdk/c_user_cmd.h"
#include "sdk/c_cs_player.h"
#include "sdk/c_game_event_manager.h"

#include "security/fnv1a.h"
#include "security/string_obfuscation.h"
#include "security/erase_function.h"

#include "utils/c_hook.h"
#include "utils/c_singleton.h"
#include "utils/c_config.h"
#include "utils/c_log.h"
#include "utils/c_hotkeys.h"
#include "utils/c_memory.h"
