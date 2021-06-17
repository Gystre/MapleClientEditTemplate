#include "pch.h"
#include "Hack.h"

#include <memedit.h>

void Hack::writeBytes(std::vector<BYTE> bytes)
{
	for (int i = 0; i < bytes.size(); i++)
	{
		WriteValue(address + i, bytes[i]);
	}
}

void Hack::enable() 
{
	writeBytes(hBytes);
	enabled = true;
}

void Hack::disable() 
{
	writeBytes(oBytes);
	enabled = false;
}