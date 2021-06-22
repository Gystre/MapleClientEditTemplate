#include "pch.h"
#include "Utilities.h"
#include "ntdll.h"

namespace Utilities {
	bool bCompare(const char* pData, const char* bMask, const char* szMask)
	{
		for (; *szMask; ++szMask, ++pData, ++bMask)
			if (*szMask == 'x' && *pData != *bMask)
				return false;
		return (*szMask) == NULL;
	}

	uintptr_t findPattern(uintptr_t dwAddress, uintptr_t dwLen, const char* bMask, const char* szMask)
	{
		if (dwAddress == 0)
		{
			//Log( "Start address is null" );
			return 0;
		}

		for (uintptr_t i = 0; i < dwLen; i++)
			if (bCompare((char*)(dwAddress + i), bMask, szMask))
				return (uintptr_t)(dwAddress + i);

		return 0;
	}

	uintptr_t findPattern(uintptr_t dwAddress, uintptr_t dwLen, const char* pat)
	{
		std::string pattern(pat), bytePattern, byteMask;
		// AA BB CC DD EE ?? FF GG ? ZZ
		// do this past each space. strtol will stop at that point.
		auto loc = 0;
		while (loc != pattern.npos)
		{
			// case for ?? / ?
			if (*(pattern.c_str() + loc) == '?')
				bytePattern += '\x00', byteMask += '?';
			else
				bytePattern += strtol(pattern.c_str() + loc, 0, 16), byteMask += 'x';

			auto f = pattern.find(' ', loc);
			loc = f != pattern.npos ? f + 1 : f; //npos or space + 1
		}

		return findPattern(dwAddress, dwLen, bytePattern.c_str(), byteMask.c_str());
	}

	uintptr_t getModuleSize(uintptr_t base)
	{
		MEMORY_BASIC_INFORMATION region = { 0 };
		ULONG out_size = 0;
		auto stat = NtQueryVirtualMemory((HANDLE)-1, (void*)base, MemoryRegionInformation, &region, sizeof(region), &out_size);
		if (stat != 0)
		{
			if (stat == 0xC0000004)
			{
				Log("Actual size is %d not %d", out_size, sizeof(region));
			}

			Log("NTQVM ERROR: %X", stat);
			return 0;
		}

		return region.RegionSize;
	}

	std::string getDllPath(HMODULE hModule) {
		//thanks stack overflow <3
		std::vector<char> pathBuf;
		DWORD copied = 0;
		do {
			pathBuf.resize(pathBuf.size() + MAX_PATH);
			copied = GetModuleFileName(hModule, &pathBuf.at(0), pathBuf.size());
		} while (copied >= pathBuf.size());

		pathBuf.resize(copied);

		std::string path(pathBuf.begin(), pathBuf.end());

		// \LEN.dll = 8 chars
		//cut off the LEN.dll
		path = path.substr(0, path.size() - 8);

		return path;
	}

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	bool createProcess(std::string programPath)
	{
		// set the size of the structures
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		// start the program up
		return CreateProcessA(programPath.c_str(),   // the path
			NULL,        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
		);

		////wait and see if it created correctly
		//WaitForSingleObject(pi.hProcess, INFINITE);
	}

	void closeProcess()
	{
		// Close process and thread handles. 
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
}