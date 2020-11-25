#include "Global.h"

BYTE* pMemory;

DWORD dwCrcStart;
DWORD dwCrcEnd;

DWORD dwCrc32 = 0x03C52113;
DWORD dwCrc32_Check = dwCrc32;

DWORD dwCrc32_VMCRC = 0x039B85F5;

__declspec(naked) void Hook_GetCrc32()
{
	__asm
	{
		cmp		ebx, [dwCrcStart]
		jb		Return
		cmp		ebx, [dwCrcEnd]
		ja		Return

		sub		ebx, [dwCrcStart]
		add		ebx, [pMemory]

		Return:
		jmp		dword ptr[dwCrc32]
	}
}

__declspec(naked) void Hook_GetCrc32_VMCRC()
{
	__asm
	{
		push	ecx
		mov		ecx, [dwCrc32_Check]
		sub		ecx, 0x10
		cmp		edi, ecx
		jb		Return
		add		ecx, 0x20
		cmp		edi, ecx
		jb		Bypass

		Bypass :
		sub		edi, [dwCrcStart]
		add		edi, [pMemory]

		Return :
		pop		ecx
		jmp		dword ptr[dwCrc32_VMCRC]
	}
}

void MemoryDump()
{
	DWORD dwModule = (DWORD)GetModuleHandleA(NULL);

	IMAGE_NT_HEADERS* nth = (IMAGE_NT_HEADERS*)(dwModule + PIMAGE_DOS_HEADER(dwModule)->e_lfanew);

	DWORD dwCrcSize = nth->OptionalHeader.SizeOfImage;

	dwCrcStart = nth->OptionalHeader.ImageBase;
	dwCrcEnd = dwCrcStart + dwCrcSize;

	pMemory = new BYTE[dwCrcSize];
	memcpy(pMemory, (void*)dwCrcStart, dwCrcSize);
}

bool HookCRC()
{
	BOOL bResult = TRUE;

	MemoryDump();
	bResult &= SetHook(true, reinterpret_cast<void**>(&dwCrc32), Hook_GetCrc32);
	bResult &= SetHook(true, reinterpret_cast<void**>(&dwCrc32_VMCRC), Hook_GetCrc32_VMCRC);

	return bResult;
}