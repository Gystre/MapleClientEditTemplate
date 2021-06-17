#include "pch.h"

// BE AWARE ===v
// in order to reference other projects you need to add:
// $(SolutionDir)Common;%(AdditionalIncludeDirectories)
// to project properties -> c/c++ -> additional include directories
#include "ExampleHooks.h"
#include <ZXString.h>
#include <Common.h>
#include <hooker.h>
#include <memedit.h>
#include <iostream>
#include <memory>
#include <shellapi.h>

#include "Utilities.h"
#include "HacksContainer.h"
#include "PipeHandler.h"

// BE AWARE ===v
// in order to fix the detours.lib link error you need to replace
// project properties -> vc++ directories -> library directories 
// with $(SolutionDir)Common;$(LibraryPath)

/// ================ \\\

//will be initialized once the dll is injected
std::string dllPath;

typedef LRESULT(CALLBACK* _CWvsApp__WindowProc_t)(HWND hWndInsertAfter, UINT msg, WPARAM wParam, LPARAM lParam);
_CWvsApp__WindowProc_t o_CWvsApp__WindowProc;

LRESULT CALLBACK WindowProc_Hook(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
)
{
	//Log("WindowProc msg: %d", uMsg);
	//std::cout << uMsg << std::endl;
	std::cout << o_CWvsApp__WindowProc << std::endl;

	//call original function
	return o_CWvsApp__WindowProc(hWnd, uMsg, wParam, lParam);
}

// executed after the client is unpacked
VOID MainFunc()
{
	AllocConsole();
	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	SetConsoleTitleA("KYLE SWORDIE HACK v221.1");

	//HacksContainer::get().init();

	//HacksContainer::get().fullMapAttackV1->enable();
	//HacksContainer::get().genericND->enable();
	//HacksContainer::get().flashJumpND->enable();
	//HacksContainer::get().unlimitedAttack->enable();

	//HWND window = FindWindowA("MapleStoryClass", nullptr);
	//o_CWvsApp__WindowProc = WNDPROC(SetWindowLongPtrA(window, GWLP_WNDPROC, LONG_PTR(WindowProc_Hook)));

	//create the process and pipe
	PipeHandler::get().initLink(dllPath);

	//figure this out later lmao
	while (true)
		std::this_thread::sleep_for(std::chrono::milliseconds(50));

	//disable all hacks
	for (std::shared_ptr<Hack> hack : HacksContainer::get().hacks)
		hack->disable();

	fclose((FILE*)stdin); 
	fclose((FILE*)stdout);
	FreeConsole();

	return;

	// below hooks only serve as examples -- they will not do anything as-is

	INITMAPLEHOOK(
		_ExampleFunc_cdecl, // pointer to original function
		_ExampleFunc_cdecl_t, // function type
		MapleHooks::ExampleCDecl_Hook, // function to detour to
		0x0 // maple address
	);

	INITMAPLEHOOK(
		_ExampleFunc_thiscall, // pointer to original function
		_ExampleFunc_thiscall_t, // function type
		MapleHooks::ExampleFunc_thiscall2, // function to detour to
		0x0 // maple address
	);

	// edit memory

	WriteValue<BYTE>(0x0, 0xEB); // address to write to, value to write
	WriteValue<DWORD>(0x0, 0x42069);
	WriteValue<double>(0x0, 420.69);

	PatchNop(0x0, 6); // address to write to, number of nops

	PatchRetZero(0x0); // function start address to return zero at
}

// dll entry point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	//quickly snatch dat hModule 2 get path
	dllPath = Utilities::getDllPath(hModule);

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		Log("DLL_PROCESS_ATTACH");

		if (HANDLE thread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&MainFunc, NULL, NULL, NULL))
			CloseHandle(thread);
		break;
	}
	case DLL_PROCESS_DETACH:
	{
		Log("DLL_PROCESS_DETACH");
		Common::GetInstance()->~Common();
		break;
	}
	}
	return TRUE;
}

#pragma endregion