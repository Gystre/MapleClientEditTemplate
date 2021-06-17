#pragma once

#include "Hack.h"

//  call above is get_flash_jump_count
//  7C ? 33 C0 E9 ? ? 00 00 8B 4D EC E8 ? ? ? ? 85 C0 75 ?

class FlashJumpND : public Hack
{
public:
	FlashJumpND() : Hack(
		0x02CE0112,
		{ 0x7C },
		{ 0xEB }
	) {}
};