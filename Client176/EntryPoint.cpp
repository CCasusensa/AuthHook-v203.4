#include "Global.h"



VOID WINAPI HeaderProc(DWORD dwPID)
{

}


DWORD WINAPI MainProc(PVOID)
{
	DWORD dwCurProcId = GetCurrentProcessId();
	HeaderProc(dwCurProcId);

	bool bInit = true;

	bInit &= HookSockApi();
	bInit &= HookWinApi();
	bInit &= HookNMCO();

	if (bInit)
	{
     //   DiscordLoop();
	}
	else
	{
        Log("[AuthHook] Initialization Failed!!!");
	}

	return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hinstDLL);
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&MainProc, NULL, NULL, NULL);
	}

	return TRUE;
}
