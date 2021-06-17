#pragma once

#include "Hack.h"

// 79 ? 8B ? C7 ? ? ? 00 00 00 00 00 00 E8

class NoCatchBreath : public Hack
{
public:
	NoCatchBreath() : Hack(
		0x012A82FA,
		{ 0x79 },
		{ 0x78 }
	) {}
};