#pragma once

#include "Singleton.h"

#include <string>

class PipeHandler : public Singleton<PipeHandler>
{
public:
	HANDLE hPipe;

	//opens pipe, creates process, and connects pipe to process
	void initLink(std::string dllPath);
};