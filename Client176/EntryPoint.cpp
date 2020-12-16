//---------------------------------------------------------------------------------------------
// v203.4 Localhost Enabler - Rajan
//---------------------------------------------------------------------------------------------

#include "Global.h"


Application app;

VOID WINAPI HeaderProc(DWORD dwPID)
{

}


void UpdateActivityCallback(void* data, EDiscordResult result)
{
    DISCORD_REQUIRE(result);
}

void DiscordLoop()
{
	Log(__FUNCTION__);

    memset(&app, 0, sizeof(app));
    
    IDiscordActivityEvents activities_events;
    memset(&activities_events, 0, sizeof(activities_events));

    DiscordCreateParams params;
    DiscordCreateParamsSetDefault(&params);
    params.client_id = CLIENT_ID;
    params.flags = DiscordCreateFlags_Default;
    params.event_data = &app;
    params.activity_events = &activities_events;
    DISCORD_REQUIRE(DiscordCreate(DISCORD_VERSION, &params, &app.core));

    app.activities = app.core->get_activity_manager(app.core);


    for (;;) 
    {
        DISCORD_REQUIRE(app.core->run_callbacks(app.core));
        Sleep(16);
    }
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
