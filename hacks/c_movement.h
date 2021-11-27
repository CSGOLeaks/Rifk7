#pragma once

#include "../includes.h"

class c_movement
{
public:
	static void run(c_cs_player* local, c_user_cmd* cmd);
private:
    static void bhop( c_cs_player* local, c_user_cmd* cmd );
    static void autostrafe( c_cs_player* local, c_user_cmd* cmd );
};
