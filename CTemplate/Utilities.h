#pragma once

#include <windows.h>
#include <vector>

namespace Utilities {
	uint8_t* patternScan(void* module, const char* signature);
}