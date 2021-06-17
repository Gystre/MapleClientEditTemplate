#pragma once

#include <vector>

class Hack
{
private:
	std::vector<BYTE> oBytes;
	std::vector<BYTE> hBytes;
	DWORD address;

	bool enabled;

	void writeBytes(std::vector<BYTE> bytes);
public:
	Hack(DWORD address, std::vector<BYTE> oBytes, std::vector<BYTE> hBytes) : address(address), oBytes(oBytes), hBytes(hBytes), enabled(false) { }

	void enable();
	void disable();
};