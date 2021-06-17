#pragma once

#include "Hack.h"

class FullMapAttackV1  : public Hack 
{
public:
	FullMapAttackV1() : Hack(
		0x01C577B6, //75 ? E9 ? ? FF FF 8B ? ? E8 ? ? 00 00 8B ? ? ?
		{ 0x75 },
		{ 0xEB }
	) {}
};