#pragma once

#include "Singleton.h"

#include <string>

class PipeHandler : public Singleton<PipeHandler>
{
private:
	DWORD sendMsg(std::string msg);
public:
	//opens pipe, creates process, and connects pipe to process
	bool initLink(std::string dllPath);
};