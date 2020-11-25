#include "Global.h"
#include "CWvsContext.h"
#include <intrin.h>
#include <map>
#include <string>

std::map<int, std::string>	g_mStringPool;

extern Application app;


typedef void(__cdecl* set_stage_t)(void* pThis, void* pParam);
auto set_stage = reinterpret_cast<set_stage_t>(0x01AA4610);

typedef int(__cdecl* ZtlSecureFuse_int_t)(int* at, unsigned int uCS);
auto ZtlSecureFuse_int = reinterpret_cast<ZtlSecureFuse_int_t>(0x00872530);

typedef ZXString<char>* (__fastcall* StringPool__GetString_t)(void* ecx, void* edx, ZXString<char>* result, unsigned int nIdx, char formal);
auto StringPool__GetString = reinterpret_cast<StringPool__GetString_t>(0x00C4C980);

typedef void(__fastcall* ZXString_char__Assign_t)(void* pThis, void* edx, const char* s, int n);
auto ZXString_char__Assign = reinterpret_cast<ZXString_char__Assign_t>(0x00823160);


bool Hook_StringPool__GetString(bool enable)
{

	StringPool__GetString_t Hook = [](void* pThis, void* edx, ZXString<char>* result, unsigned int nIdx, char formal) -> ZXString<char>*
	{
		if (g_mStringPool.count(nIdx) > 0)
		{
			auto szEntry = g_mStringPool[nIdx];

			result->m_pStr = 0;
			ZXString_char__Assign(result, edx, szEntry.c_str(), -1);

			return result;
		}

		return StringPool__GetString(pThis, edx, result, nIdx, formal);
	};

	return SetHook(enable, reinterpret_cast<void**>(&StringPool__GetString), Hook);
}

void InitStringPool()
{
	//g_mStringPool[4716] = std::string("Ver. %d.%d"); //version
	//g_mStringPool[12750] = std::string("Super GM"); //supergm
	//g_mStringPool[14520] = std::string("Comic Sans MS"); //font
}



void __cdecl SetStage_Hook(void* pStage, void* pParam)
{	
	set_stage(pStage, pParam);

	auto pData = CWvsContext::GetInstance()->GetCharacterData();

	if (pData)
	{
		auto szName = pData->characterStat.sCharacterName;
		auto nLevel = ZtlSecureFuse_int(pData->characterStat._ZtlSecureTear_nLevel, pData->characterStat._ZtlSecureTear_nLevel_CS);

		DiscordActivity activity;
		memset(&activity, 0, sizeof(activity));

		activity.type = DiscordActivityType_Playing;
		activity.timestamps.start = time(0);

		sprintf_s(activity.assets.large_text, "MapleStory");
		sprintf_s(activity.assets.large_image, "owo");

		sprintf_s(activity.state, "%s - Training", szName);
		sprintf_s(activity.details, "Level %d", nLevel);

		Log("Posting Update: %d %s",nLevel,szName);
		app.activities->update_activity(app.activities, &activity, &app, UpdateActivityCallback);
	}
}


void FuckMaple()
{
	PatchRetZero(0x029EC300); //NGS Bypass

	InitStringPool();
	Hook_StringPool__GetString(true);

	SetHook(TRUE, reinterpret_cast<void**>(&set_stage), SetStage_Hook);

}
