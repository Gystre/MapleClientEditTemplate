#include "pch.h"
#include "PipeHandler.h"

#include <tchar.h>
#include <iostream>

#include "Utilities.h"

bool PipeHandler::initLink(std::string dllPath)
{
	//open the process
	std::string clientDir = dllPath + "\\GreenGiraffeClient.exe";
	if (!Utilities::createProcess(clientDir))
	{
		std::cout << "Failed to create the GreenGiraffeClient.exe. GetLastError:" << GetLastError() << std::endl;
		return false;
	}

	sendMsg("ALIVE");

	return true;
}

DWORD PipeHandler::sendMsg(std::string msg)
{
	//create the pipe
	HANDLE hPipe = ::CreateNamedPipe(_T("\\\\.\\pipe\\GGClientPipeIN"),
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE,
		PIPE_UNLIMITED_INSTANCES,
		4096,
		4096,
		0,
		NULL);

	if (hPipe == INVALID_HANDLE_VALUE)
	{
		std::cout << "hPipe is INVALID_HANDLE_VALUE" << std::endl;
		return false;
	}

	//connect to spawned process
	if (!ConnectNamedPipe(hPipe, NULL))
	{
		std::cout << "pipe didn't connect??" << std::endl;
		return false;
	}

	//send some data
	LPCSTR data = _T(msg.c_str());
	DWORD bytesWritten = 0;
	WriteFile(hPipe, data, _tcslen(data) * sizeof(TCHAR), &bytesWritten, NULL);

	FlushFileBuffers(hPipe);
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);

	return bytesWritten;
}