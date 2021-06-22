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

//void __thiscall CUserLocal::FallDown(int this, int bEnforcedPrepareJump)
typedef void(__fastcall* _CUserLocal__FallDown_t)(PVOID pThis, PVOID edx, int bEnforcedPrepareJump);
_CUserLocal__FallDown_t o_CUserLocal__FallDown;

void __fastcall CUserLocal__FallDown_Hook(PVOID pThis, PVOID edx, int bEnforcedPrepareJump)
{
	std::cout << "cuserlocal: 0x" << pThis << std::endl;

	o_CUserLocal__FallDown(pThis, NULL, bEnforcedPrepareJump);
}

// executed after the client is unpacked
VOID MainFunc()
{
	AllocConsole();
	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	SetConsoleTitleA("KYLE SWORDIE HACK v223.3");

	//INITMAPLEHOOK(
	//	o_CUserLocal__FallDown,
	//	_CUserLocal__FallDown_t,
	//	CUserLocal__FallDown_Hook,
	//	0x02BD1BC0
	//)

	//HacksContainer::get().init();

	//HacksContainer::get().fullMapAttackV1->enable();
	//HacksContainer::get().genericND->enable();
	//HacksContainer::get().flashJumpND->enable();
	//HacksContainer::get().unlimitedAttack->enable();

	//create the process and pipe
	PipeHandler::get().initLink(dllPath);

	//doesn't crash but also doesn't work :/
	//auto base = (uintptr_t)GetModuleHandle(0);
	//auto size = Utilities::getModuleSize(base);
	//uintptr_t loc = Utilities::findPattern(base, size, (char*)"7E ?? 83 ?? ?? 7D ?? 8B ?? ?? 2B C2 3D 6A FF FF FF 7E ?? 3D 96 00 00 00 7D ?? 8B ?? ?? 3B ?? ?? 7C ??");
	//std::cout << loc << std::endl;	

	//some sort of packet encryption function from blackcipher
	//auto base = (uintptr_t)GetModuleHandle(0);
	//auto size = Utilities::getModuleSize(base);
	//uintptr_t loc = Utilities::findPattern(base, size, (char*)"8b 47 08 3d 00 ff");
	//std::cout << loc << std::endl;

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