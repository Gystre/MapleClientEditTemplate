#pragma once

#include "Hack.h"

// 1st jle from int CAntiRepeat::TryRepeat(long,long)
//7E ? 83 ? ? 7D ? 8B ? ? 2B C2 3D 6A FF FF FF 7E ? 3D 96 00 00 00 7D ? 8B ? ? 3B ? ? 7C ?

class UnlimitedAttack : public Hack
{
public:
	UnlimitedAttack() : Hack(
		0x02B76DC1,
		{ 0x7E },
		{ 0xEB }
	) {}
};