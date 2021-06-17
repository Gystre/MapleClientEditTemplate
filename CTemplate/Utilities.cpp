#include "pch.h"
#include "Utilities.h"

namespace Utilities {
	std::uint8_t* patternScan(void* module, const char* signature) {
		static auto pattern_to_byte = [](const char* pattern) {
			auto bytes = std::vector<int>{};
			auto start = const_cast<char*>(pattern);
			auto end = const_cast<char*>(pattern) + strlen(pattern);

			for (auto current = start; current < end; ++current) {
				if (*current == '?') {
					++current;
					if (*current == '?')
						++current;
					bytes.push_back(-1);
				}
				else {
					bytes.push_back(strtoul(current, &current, 16));
				}
			}
			return bytes;
		};

		auto dos_headers = (PIMAGE_DOS_HEADER)module;
		auto nt_headers = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dos_headers->e_lfanew);

		auto size_of_image = nt_headers->OptionalHeader.SizeOfImage;
		auto pattern_bytes = pattern_to_byte(signature);
		auto scan_bytes = reinterpret_cast<std::uint8_t*>(module);

		auto s = pattern_bytes.size();
		auto d = pattern_bytes.data();

		for (auto i = 0ul; i < size_of_image - s; ++i) {
			bool found = true;
			for (auto j = 0ul; j < s; ++j) {
				if (scan_bytes[i + j] != d[j] && d[j] != -1) {
					found = false;
					break;
				}
			}
			if (found) {
				return &scan_bytes[i];
			}
		}
		return nullptr;
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