#pragma once

#include "Hack.h"

// inside CUserLocal::FallDown
//74 ? 8B CB C7 83 ? ? ? ? 01 00 00 00 E8 ? ? ? ? 85 C0 75 ? 89 B3 ? ? ? ? EB ? 8B CB E8 ? ? ? ? 89 83 ? ? ? ? 6A 01

class JumpDownAnywhere : public Hack
{
public:
	JumpDownAnywhere() : Hack(
		0x02B76DC1,
		{ 0x74, 0x5A },
		{ 0x90, 0x90 }
	) {}
};