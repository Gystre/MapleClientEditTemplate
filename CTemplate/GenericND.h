#pragma once

#include "Hack.h"

class GenericND : public Hack 
{
public:
	GenericND() : Hack(
		0x02BBE5BE, //75 31 83 BD ? ? ? ? 00 75 28 8B ? ? ? 8B ? ? ? ? ? ? 8B
		{ 0x75 },
		{ 0xEB }
	) {}
};