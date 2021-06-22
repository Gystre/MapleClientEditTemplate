#pragma once

#include <windows.h>
#include <vector>
#include <string>

namespace Utilities {
	uintptr_t findPattern(uintptr_t dwAddress, uintptr_t dwLen, const char* pat);
	uintptr_t getModuleSize(uintptr_t base);

	std::string getDllPath(HMODULE hModule);

	//can only keep one program open at a time :P
	//keep track of handles to close process later
	extern STARTUPINFO si;
	extern PROCESS_INFORMATION pi;
	bool createProcess(std::string programPath);
	void closeProcess();
}