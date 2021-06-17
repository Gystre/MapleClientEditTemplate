#pragma once

#include <windows.h>
#include <vector>
#include <string>

namespace Utilities {
	uint8_t* patternScan(void* module, const char* signature);

	std::string getDllPath(HMODULE hModule);

	//can only keep one program open at a time :P
	//keep track of handles to close process later
	extern STARTUPINFO si;
	extern PROCESS_INFORMATION pi;
	bool createProcess(std::string programPath);
	void closeProcess();
}