#include "pch.h"
#include "PipeHandler.h"

#include <tchar.h>
#include <iostream>

#include "Utilities.h"

void PipeHandler::initLink(std::string dllPath)
{
	//create the pipe
	hPipe = ::CreateNamedPipe(_T("\\\\.\\pipe\\GGClientPipe"),
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE,
		PIPE_UNLIMITED_INSTANCES,
		4096,
		4096,
		0,
		NULL);

	//open the process
	std::string clientDir = dllPath + "\\GreenGiraffeClient.exe";
	if (!Utilities::createProcess(clientDir))
	{
		std::cout << "Failed to create the GreenGiraffeClient.exe. GetLastError:" << GetLastError() << std::endl;
		return;
	}

	//connect to spawned process
	ConnectNamedPipe(hPipe, NULL);

	//send some data
	LPCSTR data = _T("alive");
	DWORD bytesWritten = 0;
	WriteFile(hPipe, data, _tcslen(data) * sizeof(TCHAR), &bytesWritten, NULL);
	CloseHandle(hPipe);
}