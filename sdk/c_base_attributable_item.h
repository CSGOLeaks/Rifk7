#pragma once

#include "c_client_entity.h"
#include "macros.h"

class c_base_attributable_item : public c_client_entity
{
public:
	netvar(get_item_definition(), short, "CBaseAttributableItem", "m_iItemDefinitionIndex")
};
