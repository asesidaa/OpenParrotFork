#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility\Hooking.Patterns.h"
#include <Xinput.h>

#ifdef _M_AMD64

#define HOOK_DYNAMIC(returnType, callingConvention, functionName, ...)                                                                               \
typedef returnType callingConvention (*functionName) (__VA_ARGS__);                                                                              \
functionName original##functionName = NULL;                                                                                                      \
void *where##functionName           = NULL;                                                                                                      \
returnType callingConvention implOf##functionName (__VA_ARGS__)

#define INSTALL_HOOK(functionName)                                                                                                                   \
{                                                                                                                                                \
MH_Initialize ();                                                                                                                            \
MH_CreateHook ((void *)where##functionName, (void *)implOf##functionName, (void **)(&original##functionName));                               \
MH_EnableHook ((void *)where##functionName);                                                                                                 \
}

#define INSTALL_HOOK_DYNAMIC(functionName, location)                                                                                                 \
{                                                                                                                                                \
where##functionName = (void *)location;                                                                                                      \
INSTALL_HOOK (functionName);                                                                                                                 \
}

static bool btnTestToggle = false;
static bool btnTestLast = false;
static bool btnCoinLast = false;

static bool btnP1RimR = false;
static bool btnP1CenterR = false;
static bool btnP1CenterL= false;
static bool btnP1RimL = false;

static bool btnP2RimR = false;
static bool btnP2CenterR = false;
static bool btnP2CenterL = false;
static bool btnP2RimL = false;

static uint16_t coinCount = 0;
extern int* ffbOffset;

static HWND (WINAPI* CreateWindowExWOri)(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
static HWND WINAPI CreateWindowExWHook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	// Taiko creates a bunch of hidden windows, dont change properties of those
	if (nWidth > 0 && nHeight > 0)
	{
		if (ToBool(config["General"]["Windowed"]))
		{
			dwStyle = WS_VISIBLE | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
			X = (GetSystemMetrics(SM_CXSCREEN) - nWidth) / 2;
			Y = (GetSystemMetrics(SM_CYSCREEN) - nHeight) / 2;
		}
		else
		{
			X = 0;
			Y = 0;
		}

		lpWindowName = L"OpenParrot - Taiko no Tatsujin Nijiiro Ver";
	}

	return CreateWindowExWOri(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

static BOOL(WINAPI* ShowCursorOri)(BOOL bShow);
static BOOL WINAPI ShowCursorHook(BOOL bShow)
{
	if (ToBool(config["General"]["HideCursor"]))
		return ShowCursorOri(false);
	else
		return ShowCursorOri(true);
}

static __int64 bnusio_ClearSram()
{
	//info(true, "bnusio_ClearSram");
	return 0;
}

static __int64 bnusio_Close()
{
	//info(true, "bnusio_Close");
	return 0;
}

static __int64 __fastcall bnusio_Communication(int a1)
{
	//info(true, "bnusio_Communication");
	return 0;
}

static __int64 __fastcall bnusio_DecCoin(int a1, unsigned __int16 a2)
{
	//info(true, "bnusio_DecCoin");
	return 0;
}

static __int64 __fastcall bnusio_DecService(int a1, unsigned __int16 a2)
{
	//info(true, "bnusio_DecService");
	return 0;
}

// Return a random value to simulate the arcade drum
static uint16_t rand16(void) {
	uint16_t r = 0;
	int random;
	int max_value = 20000; // ~ 90 in I/O test menu
	int min_value = 10000; // ~ 30 in I/O test menu

	random = rand() % max_value + min_value;
	r = (unsigned)random;

	return r;
}

static uint16_t __fastcall bnusio_GetAnalogIn(unsigned __int8 a1)
{
	//info(true, "bnusio_GetAnalogIn a1: %u", a1);

	uint16_t rv = 0;

	// Player 1 Drum Rim Left
	if (a1 == 0) {
		bool currentBtn = (bool)(*ffbOffset & 0x40);

		if (currentBtn && btnP1RimL != currentBtn) 
			rv = rand16();
		
		btnP1RimL = currentBtn;
	}

	// Player 1 Drum Center Left
	else if (a1 == 1) {
		bool currentBtn = (bool)(*ffbOffset & 0x80);

		if (currentBtn && btnP1CenterL != currentBtn) 
			rv = rand16();
		
		btnP1CenterL = currentBtn;
	}

	// Player 1 Drum Center Right
	else if (a1 == 2) {
		bool currentBtn = (bool)(*ffbOffset & 0x100);

		if (currentBtn && btnP1CenterR != currentBtn) 
			rv = rand16();

		btnP1CenterR = currentBtn;
	}

	// Player 1 Drum Rim Right
	else if (a1 == 3) {
		bool currentBtn = (bool)(*ffbOffset & 0x200);

		if (currentBtn && btnP1RimR != currentBtn) 
			rv = rand16();

		btnP1RimR = currentBtn;
	}

	// Player 2 Drum Rim Left
	else if (a1 == 4) {
		bool currentBtn = (bool)(*ffbOffset & 0x400);

		if (currentBtn && btnP2RimL != currentBtn) 
			rv = rand16();

		btnP2RimL = currentBtn;
	}

	// Player 2 Drum Center Left
	else if (a1 == 5) {
		bool currentBtn = (bool)(*ffbOffset & 0x800);

		if (currentBtn && btnP2CenterL != currentBtn) 
			rv = rand16();

		btnP2CenterL = currentBtn;
	}

	// Player 2 Drum Center Right
	else if (a1 == 6) {
		bool currentBtn = (bool)(*ffbOffset & 0x1000);

		if (currentBtn && btnP2CenterR != currentBtn)
			rv = rand16();

		btnP2CenterR = currentBtn;
	}

	// Player 2 Drum Rim Right
	else if (a1 == 7) {
		bool currentBtn = (bool)(*ffbOffset & 0x2000);

		if (currentBtn && btnP2RimR != currentBtn) 
			rv = rand16();
		
		btnP2RimR = currentBtn;
	}
	
	return rv;
}

static void* __fastcall bnusio_GetBuffer(unsigned __int16 a1, __int64 a2, __int16 a3)
{
	//info(true, "bnusio_GetBuffer");
	return nullptr;
}

static __int64 __fastcall bnusio_GetCDOut(unsigned __int8 a1)
{
	//info(true, "bnusio_GetCDOut");
	return 0;
}

static uint16_t __fastcall bnusio_GetCoin(int player)
{
	//info(true, "bnusio_GetCoin player: %d", player);
	return (uint16_t)coinCount;
}

static void* __fastcall bnusio_GetCoinError(int player)
{
	//info(true, "bnusio_GetCoinError player: %d", player);
	return nullptr;
}

static __int64 __fastcall bnusio_GetCoinLock(unsigned __int8 a1)
{
	//info(true, "bnusio_GetCoinLock");
	return 0;
}

static unsigned __int64 bnusio_GetEncoder()
{
	//info(true, "bnusio_GetEncoder");
	return 0;
}

static void* bnusio_GetExpansionMode()
{
	//info(true, "bnusio_GetExpansionMode");
	return nullptr;
}

static void* bnusio_GetFirmwareVersion()
{
	//info(true, "bnusio_GetFirmwareVersion");
	return (void*)(uint16_t)126;
}

static char __fastcall bnusio_GetGout(unsigned __int8 a1)
{
	//info(true, "bnusio_GetGout");
	return 0;
}

static __int64 __fastcall bnusio_GetHopOut(unsigned __int8 a1)
{
	//info(true, "bnusio_GetHopOut");
	return 0;
}

static void* bnusio_GetIoBoardName()
{
	//info(true, "bnusio_GetIoBoardName");
	return nullptr;
}

static __int64 __fastcall bnusio_GetRegisterU16(__int16 a1)
{
	//info(true, "bnusio_GetRegisterU16");
	return 0;
}

static char __fastcall bnusio_GetRegisterU8(unsigned __int16 a1)
{
	//info(true, "bnusio_GetRegisterU8");
	return 0;
}

static void* __fastcall bnusio_GetService(int a1)
{
	//info(true, "bnusio_GetService");
	return nullptr;
}

static void* __fastcall bnusio_GetServiceError(int a1)
{
	//info(true, "bnusio_GetServiceError");
	return nullptr;
}

static void* __fastcall bnusio_GetStatusU16(unsigned __int16 a1)
{
	//info(true, "bnusio_GetStatusU16");
	return nullptr;
}

static void* __fastcall bnusio_GetStatusU8(unsigned __int16 a1)
{
	//info(true, "bnusio_GetStatusU8");
	return nullptr;
}

static uint32_t bnusio_GetSwIn()
{
	// Prevent that you need to hold the test key
	bool btnTestCurrent = (bool)(*ffbOffset & 0x04);

	if (btnTestCurrent && !btnTestLast)
		btnTestToggle = !btnTestToggle;

	btnTestLast = btnTestCurrent;

	// Coin
	bool btnCoinCurrent = (bool)(*ffbOffset & 0x01);

	if (btnCoinCurrent && !btnCoinLast)
		coinCount++;

	btnCoinLast = btnCoinCurrent;

	uint32_t temp = 0;
	temp |= ((uint32_t)btnTestToggle) << 7; // Test
	temp |= ((uint32_t)(bool)(*ffbOffset & 0x20)) << 9; // Enter
	temp |= ((uint32_t)(bool)(*ffbOffset & 0x10)) << 12; // Select Down
	temp |= ((uint32_t)(bool)(*ffbOffset & 0x08)) << 13; // Select Up
	temp |= ((uint32_t)(bool)(*ffbOffset & 0x02)) << 14; // Service

	//char buffer[33];
	//itoa(temp, buffer, 2);

	//info(true, "bnusio_GetSwIn switches: %s", buffer);

	return temp;
}

static uint64_t bnusio_GetSwIn64()
{
	//info(true, "bnusio_GetSwIn64");
	return 0;
}

static void* bnusio_GetSystemError()
{
	//info(true, "bnusio_GetSystemError");
	return nullptr;
}

static char bnusio_IsConnected()
{
	//info(true, "bnusio_IsConnected");
	return 0;
}

static char bnusio_IsWideUsio()
{
	//info(true, "bnusio_IsWideUsio");
	return 0;
}

static __int64 bnusio_Open()
{
	//info(true, "bnusio_Open");
	return 0; // 1=busy
}

static __int64 bnusio_ResetCoin()
{
	//info(true, "bnusio_ResetCoin");
	return 0;
}

static BOOL bnusio_ResetIoBoard()
{
	//info(true, "bnusio_ResetIoBoard");
	return 0;
}

static __int64 __fastcall bnusio_SetBuffer(unsigned __int16 a1, int a2, __int16 a3)
{
	//info(true, "bnusio_SetBuffer");
	return 0;
}

static __int64 __fastcall bnusio_SetCDOut(unsigned __int8 a1, char a2)
{
	//info(true, "bnusio_SetCDOut");
	return 0;
}

static __int64 __fastcall bnusio_SetCoinLock(unsigned __int8 a1, char a2)
{
	//info(true, "bnusio_SetCoinLock");
	return 0;
}

static __int64 __fastcall bnusio_SetExpansionMode(__int16 a1)
{
	//info(true, "bnusio_SetExpansionMode");
	return 0;
}

static __int64 __fastcall bnusio_SetGout(unsigned __int8 a1, unsigned __int8 a2)
{
	//info(true, "bnusio_SetGout a1: %u a2: %u", a1, a2);
	return 0; // outputs
}

static __int64 __fastcall bnusio_SetHopOut(unsigned __int8 a1, char a2)
{
	//info(true, "bnusio_SetHopOut");
	return 0;
}

static __int64 __fastcall bnusio_SetHopperLimit(unsigned __int16 a1, __int16 a2)
{
	//info(true, "bnusio_SetHopperLimit");
	return 0;
}

static __int64 __fastcall bnusio_SetHopperRequest(unsigned __int16 a1, __int16 a2)
{
	//info(true, "bnusio_SetHopperRequest");
	return 0;
}

static void* __fastcall bnusio_SetPLCounter(__int16 a1)
{
	//info(true, "bnusio_SetPLCounter a1: %d", a1);
	return nullptr;
}

static __int64 __fastcall bnusio_SetRegisterU16(unsigned __int16 a1, __int16 a2)
{
	//info(true, "bnusio_SetRegisterU16");
	return 0;
}

static __int64 __fastcall bnusio_SetRegisterU8(unsigned __int16 a1, char a2)
{
	//info(true, "bnusio_SetRegisterU8");
	return 0;
}

static __int64 __fastcall bnusio_SetSystemError(__int16 a1)
{
	//info(true, "bnusio_SetSystemError");
	return 0;
}

static __int64 __fastcall bnusio_SramRead(int a1, unsigned __int8 a2, int a3, unsigned __int16 a4)
{
	//info(true, "bnusio_SramRead");
	return 0;
}

static __int64 __fastcall bnusio_SramWrite(int a1, unsigned __int8 a2, int a3, unsigned __int16 a4)
{
	//info(true, "bnusio_SramWrite");
	return 0;
}

static __int64 nbamUsbFinderInitialize()
{
	//info(true, "nbamUsbFinderInitialize");
	return 0;
}

static __int64 nbamUsbFinderRelease()
{
	//info(true, "nbamUsbFinderRelease");
	return 0;
}

static __int64 __fastcall nbamUsbFinderGetSerialNumber(int a1, __int64 a2)
{
	info(true, "nbamUsbFinderGetSerialNumber a1: %u a2: %p", a1, a2);
	auto result = reinterpret_cast<char*>(a2);
	strcpy_s((char*)a2, 13, "284111080001");
	result[12] = '\0';
	info(true, "%s", a2);
	return 0;
}

// XInputs hooks to disable built in XInput screwing up our input emu
static DWORD XInputGetStateHook(DWORD dwUserIndex, XINPUT_STATE* pState)
{
	//info(true, "XInputGetStateHook");
	return ERROR_DEVICE_NOT_CONNECTED;
}

static DWORD XInputSetStateHook(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
{
	//info(true, "XInputSetStateHook");
	return ERROR_DEVICE_NOT_CONNECTED;
}

static DWORD XInputGetCapabilitiesHook(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities)
{
	//info(true, "XInputGetCapabilitiesHook");
	return ERROR_DEVICE_NOT_CONNECTED;
}
static int (*SSL_shutdownOriginal)(void* ssl);
static int SSL_shutdownHook(void *ssl)
{
	SSL_shutdownOriginal(ssl);
	return 1;
}

static InitFunction TaikoV0Func([]()
{
	uintptr_t imageBase = (uintptr_t)GetModuleHandleA(nullptr);
	uintptr_t amBase = (uintptr_t)GetModuleHandleA("AMFrameWork.dll");

	// Skip ExitWindowsEx (reboots pc when debugging)
	injector::MakeNOP(amBase + 0x33C71, 10);

	// Path fixes
	injector::WriteMemoryRaw(imageBase + 0x9D23C8, ".\\Setting2.bin", 15, true); // g:\\Setting2.bin
	injector::WriteMemoryRaw(imageBase + 0x9D23B8, ".\\Setting1.bin", 15, true); // f:\\Setting1.bin
	

	injector::WriteMemory<BYTE>(amBase + 0x321A7, 0xEB, true); // ErrorLogPathA
	injector::WriteMemory<BYTE>(amBase + 0x322FA, 0xEB, true); // ErrorLogPathB
	injector::WriteMemory<BYTE>(amBase + 0x326D9, 0xEB, true); // CommonLogPathA
	injector::WriteMemory<BYTE>(amBase + 0x3282C, 0xEB, true); // CommonLogPathB
	injector::WriteMemory<BYTE>(amBase + 0x32C2A, 0xEB, true); // BackupDataPathA
	injector::WriteMemory<BYTE>(amBase + 0x32D7D, 0xEB, true); // BackupDataPathB
	injector::WriteMemory<BYTE>(amBase + 0x1473F, 0xEB, true); // CreditLogPathA
	injector::WriteMemory<BYTE>(amBase + 0x148AA, 0xEB, true); // CreditLogPathB

	// Skip errors
	injector::WriteMemory<BYTE>(imageBase + 0x1F0A0, 0xC3, true);

	// Respatch (currently doesn't change render resolution)
	if (ToBool(config["General"]["Custom Resolution (Stretches)"]))
	{
		DWORD resWidth = FetchDwordInformation("General", "Resolution Width", 1920);
		DWORD resHeight = FetchDwordInformation("General", "Resolution Height", 1080);

		injector::WriteMemory<DWORD>(imageBase + 0x224B2B, resWidth, true);
		injector::WriteMemory<DWORD>(imageBase + 0x224B32, resHeight, true);
	}

	// Fixes by the Taiko community (thanks Swigz, Samyuu and Mon!)
	if (ToBool(config["General"]["FixAnimationsEnable"]))
	{
		DWORD aniFps = FetchDwordInformation("General", "FixAnimationsFps", 120);

		injector::WriteMemory<float>(imageBase + 0x9A6030, 1.0f / (float)aniFps * 1000.0f, true); // Enso Game Frame Time
		injector::WriteMemory<float>(imageBase + 0x9ECEC4, 0.0166800003498793f / 120.0f * (float)aniFps, true); // Model Animation 60 FPS Frame Time Factor
		injector::WriteMemory<double>(imageBase + 0x9ED078, (double)aniFps, true); // Lua Common.FPS
		injector::WriteMemory<float>(imageBase + 0x9ED140, (float)aniFps, true); // Compressed Animation Frame Rate
	}

	if (ToBool(config["General"]["UnlockAllSongs"]))
	{
		injector::WriteMemoryRaw(imageBase + 0x1F6B78, "\xB0\x01", 2, true); // 32 C0 (XOR AL, AL) -> B0 01 (MOV AL, 1)
	}

	if (ToBool(config["General"]["SharedAudioMode"]))
	{
		injector::WriteMemory<BYTE>(imageBase + 0x552177, 0xEB, true); // 74 (JZ) -> EB (JMP)
	}

	// Hooks
	MH_Initialize();

	MH_CreateHookApi(L"user32.dll", "CreateWindowExW", CreateWindowExWHook, (void**)&CreateWindowExWOri);
	MH_CreateHookApi(L"user32.dll", "ShowCursor", ShowCursorHook, (void**)&ShowCursorOri);

	MH_CreateHookApi(L"bnusio.dll", "bnusio_ClearSram", bnusio_ClearSram, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_Close", bnusio_Close, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_Communication", bnusio_Communication, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_DecCoin", bnusio_DecCoin, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_DecService", bnusio_DecService, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetAnalogIn", bnusio_GetAnalogIn, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetBuffer", bnusio_GetBuffer, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetCDOut", bnusio_GetCDOut, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetCoin", bnusio_GetCoin, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetCoinError", bnusio_GetCoinError, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetCoinLock", bnusio_GetCoinLock, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetEncoder", bnusio_GetEncoder, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetExpansionMode", bnusio_GetExpansionMode, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetFirmwareVersion", bnusio_GetFirmwareVersion, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetGout", bnusio_GetGout, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetHopOut", bnusio_GetHopOut, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetIoBoardName", bnusio_GetIoBoardName, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetRegisterU16", bnusio_GetRegisterU16, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetRegisterU8", bnusio_GetRegisterU8, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetService", bnusio_GetService, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetServiceError", bnusio_GetServiceError, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetStatusU16", bnusio_GetStatusU16, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetStatusU8", bnusio_GetStatusU8, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetSwIn", bnusio_GetSwIn, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetSwIn64", bnusio_GetSwIn64, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetSystemError", bnusio_GetSystemError, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_IsConnected", bnusio_IsConnected, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_IsWideUsio", bnusio_IsWideUsio, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_Open", bnusio_Open, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_ResetCoin", bnusio_ResetCoin, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_ResetIoBoard", bnusio_ResetIoBoard, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SetBuffer", bnusio_SetBuffer, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SetCDOut", bnusio_SetCDOut, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SetCoinLock", bnusio_SetCoinLock, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SetExpansionMode", bnusio_SetExpansionMode, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SetGout", bnusio_SetGout, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SetHopOut", bnusio_SetHopOut, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SetHopperLimit", bnusio_SetHopperLimit, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SetHopperRequest", bnusio_SetHopperRequest, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SetPLCounter", bnusio_SetPLCounter, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SetRegisterU16", bnusio_SetRegisterU16, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SetRegisterU8", bnusio_SetRegisterU8, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SetSystemError", bnusio_SetSystemError, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SramRead", bnusio_SramRead, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SramWrite", bnusio_SramWrite, nullptr);

	MH_CreateHookApi(L"nbamUsbFinder.dll", "nbamUsbFinderInitialize", nbamUsbFinderInitialize, nullptr);
	MH_CreateHookApi(L"nbamUsbFinder.dll", "nbamUsbFinderRelease", nbamUsbFinderRelease, nullptr);
	MH_CreateHookApi(L"nbamUsbFinder.dll", "nbamUsbFinderGetSerialNumber", nbamUsbFinderGetSerialNumber, nullptr);

	MH_CreateHookApi(L"xinput9_1_0.dll", "XInputGetState", XInputGetStateHook, nullptr);
	MH_CreateHookApi(L"xinput9_1_0.dll", "XInputSetState", XInputSetStateHook, nullptr);
	MH_CreateHookApi(L"xinput9_1_0.dll", "XInputGetCapabilities", XInputGetCapabilitiesHook, nullptr);

	MH_EnableHook(nullptr);
}, GameID::TaikoV0);

typedef  uint8_t  (__fastcall *qrVtable1) (uint64_t a1);
qrVtable1 originalqrVtable1 = nullptr;
uintptr_t whereqrVtable1 = NULL;
uint8_t __fastcall implOfqrVtable1 (uint64_t a1) { return 1; }


typedef uint8_t (__fastcall *qrReadFromCOM1) (uint64_t a1);
qrReadFromCOM1 originalqrReadFromCOM1 = nullptr;
uintptr_t whereqrReadFromCOM1 = NULL;
uint8_t __fastcall implOfqrReadFromCOM1 (uint64_t a1) {
	*(uint32_t *)(a1 + 40) = 1;
	*(uint32_t *)(a1 + 16) = 1;
	return 1;
}

static constexpr uintptr_t baseAddress = 0x00007FF7DE5C0000;

static constexpr uintptr_t addresses[] =
	{0x00007FF7DE6AE0A4, 0x00007FF7DE6AE8B5, 0x00007FF7DE6AEDA6, 0x00007FF7DE7E5FB6, 0x00007FF7DE7E6063, 0x00007FF7DE7E609F, 0x00007FF7DE7E6146, 0x00007FF7DE7E6296, 0x00007FF7DE8B39E6, 0x00007FF7DE8B3AB0, 0x00007FF7DE8B3BE4, 0x00007FF7DE8B3CB3, 0x00007FF7DE8C643B, 0x00007FF7DE8C6507, 0x00007FF7DE8C65D3, 0x00007FF7DE8C66FB, 0x00007FF7DE8C67C7, 0x00007FF7DE8C6893, 0x00007FF7DE8C698B, 0x00007FF7DE8C6A2E, 0x00007FF7DE8D3666, 0x00007FF7DE8D3726, 0x00007FF7DE8D39F4, 0x00007FF7DE8D3B04, 0x00007FF7DE8D3C24, 0x00007FF7DE8D3CF4, 0x00007FF7DE8D4059, 0x00007FF7DE8D40C4, 0x00007FF7DE8D47AA, 0x00007FF7DE8D4DCC, 0x00007FF7DE8D4EC9, 0x00007FF7DE8D4F46, 0x00007FF7DE8D4F97, 0x00007FF7DE8D5608, 0x00007FF7DE8F8E2C, 0x00007FF7DE90A7EB};

static InitFunction TaikoV8Func([]()
{
	uintptr_t imageBase = (uintptr_t)GetModuleHandleA(nullptr);
	uintptr_t amBase = (uintptr_t)GetModuleHandleA("AMFrameWork.dll");

	// Skip ExitWindowsEx (reboots pc when debugging)
	injector::MakeNOP(amBase + 0x35AB1, 10);

	// Path fixes
	injector::WriteMemoryRaw(imageBase + 0xB5C538, ".\\Setting2.bin", 15, true); // g:\\Setting2.bin
	injector::WriteMemoryRaw(imageBase + 0xB5C528, ".\\Setting1.bin", 15, true); // f:\\Setting1.bin

	// Unlock song limit
	if (ToBool(config["General"]["UnlockSongLimit"]))
	{
		for (const auto address : addresses)
		{
			const auto offset = address - baseAddress;
			injector::WriteMemory<WORD>(imageBase + offset, 4000, true);
		}
	}

	// Use TLS 1.2
	injector::WriteMemory<BYTE>(imageBase + 0x44B1A9, 0x10, true);
	
	injector::WriteMemory<BYTE>(amBase + 0x33EF7, 0xEB, true); // ErrorLogPathA
	injector::WriteMemory<BYTE>(amBase + 0x3404A, 0xEB, true); // ErrorLogPathB
	injector::WriteMemory<BYTE>(amBase + 0x34429, 0xEB, true); // CommonLogPathA
	injector::WriteMemory<BYTE>(amBase + 0x3457C, 0xEB, true); // CommonLogPathB
	injector::WriteMemory<BYTE>(amBase + 0x3497A, 0xEB, true); // BackupDataPathA
	injector::WriteMemory<BYTE>(amBase + 0x34ACD, 0xEB, true); // BackupDataPathB
	injector::WriteMemory<BYTE>(amBase + 0x148AF, 0xEB, true); // CreditLogPathA
	injector::WriteMemory<BYTE>(amBase + 0x14A1A, 0xEB, true); // CreditLogPathB

	// Skip errors
	injector::WriteMemory<BYTE>(imageBase + 0x239C0, 0xC3, true);

	// Respatch (currently doesn't change render resolution)
	if (ToBool(config["General"]["Custom Resolution (Stretches)"]))
	{
		DWORD resWidth = FetchDwordInformation("General", "Resolution Width", 1920);
		DWORD resHeight = FetchDwordInformation("General", "Resolution Height", 1080);

		injector::WriteMemory<DWORD>(imageBase + 0x35FC5B, resWidth, true);
		injector::WriteMemory<DWORD>(imageBase + 0x35FC62, resHeight, true);
	}

	// Fixes by the Taiko community (thanks Swigz, Samyuu and Mon!)
	if (ToBool(config["General"]["FixAnimationsEnable"]))
	{
		DWORD aniFps = FetchDwordInformation("General", "FixAnimationsFps", 120);

		injector::WriteMemory<float>(imageBase + 0xB1A2D4, 1.0f / (float)aniFps * 1000.0f, true); // Enso Game Frame Time
		injector::WriteMemory<float>(imageBase + 0xB7755C, 0.0166800003498793f / 120.0f * (float)aniFps, true); // Model Animation 60 FPS Frame Time Factor
		injector::WriteMemory<double>(imageBase + 0xB77730, (double)aniFps, true); // Lua Common.FPS
		injector::WriteMemory<float>(imageBase + 0xB77814, (float)aniFps, true); // Compressed Animation Frame Rate
	}

	if (ToBool(config["General"]["UnlockAllSongs"]))
	{
		injector::WriteMemoryRaw(imageBase + 0x314E8D, "\xB0\x01", 2, true); // 32 C0 (XOR AL, AL) -> B0 01 (MOV AL, 1)
	}

	if (ToBool(config["General"]["SharedAudioMode"]))
	{
		injector::WriteMemory<BYTE>(imageBase + 0x692E17, 0xEB, true); // 74 (JZ) -> EB (JMP)
	}
	// Hooks
	MH_Initialize();

	
	 whereqrVtable1 = amBase + 0x1BA00;
		MH_CreateHook (LPVOID(whereqrVtable1), LPVOID(implOfqrVtable1), reinterpret_cast<void**>(&originalqrVtable1));

	whereqrReadFromCOM1 = amBase + 0x1BC20;
	MH_CreateHook (LPVOID(whereqrReadFromCOM1), LPVOID(implOfqrReadFromCOM1), reinterpret_cast<void**>(&originalqrReadFromCOM1));
	
	MH_CreateHookApi(L"user32.dll", "CreateWindowExW", CreateWindowExWHook, (void**)&CreateWindowExWOri);
	MH_CreateHookApi(L"user32.dll", "ShowCursor", ShowCursorHook, (void**)&ShowCursorOri);

	MH_CreateHookApi(L"bnusio.dll", "bnusio_ClearSram", bnusio_ClearSram, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_Close", bnusio_Close, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_Communication", bnusio_Communication, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_DecCoin", bnusio_DecCoin, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_DecService", bnusio_DecService, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetAnalogIn", bnusio_GetAnalogIn, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetBuffer", bnusio_GetBuffer, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetCDOut", bnusio_GetCDOut, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetCoin", bnusio_GetCoin, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetCoinError", bnusio_GetCoinError, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetCoinLock", bnusio_GetCoinLock, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetEncoder", bnusio_GetEncoder, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetExpansionMode", bnusio_GetExpansionMode, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetFirmwareVersion", bnusio_GetFirmwareVersion, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetGout", bnusio_GetGout, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetHopOut", bnusio_GetHopOut, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetIoBoardName", bnusio_GetIoBoardName, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetRegisterU16", bnusio_GetRegisterU16, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetRegisterU8", bnusio_GetRegisterU8, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetService", bnusio_GetService, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetServiceError", bnusio_GetServiceError, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetStatusU16", bnusio_GetStatusU16, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetStatusU8", bnusio_GetStatusU8, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetSwIn", bnusio_GetSwIn, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetSwIn64", bnusio_GetSwIn64, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_GetSystemError", bnusio_GetSystemError, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_IsConnected", bnusio_IsConnected, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_IsWideUsio", bnusio_IsWideUsio, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_Open", bnusio_Open, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_ResetCoin", bnusio_ResetCoin, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_ResetIoBoard", bnusio_ResetIoBoard, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SetBuffer", bnusio_SetBuffer, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SetCDOut", bnusio_SetCDOut, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SetCoinLock", bnusio_SetCoinLock, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SetExpansionMode", bnusio_SetExpansionMode, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SetGout", bnusio_SetGout, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SetHopOut", bnusio_SetHopOut, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SetHopperLimit", bnusio_SetHopperLimit, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SetHopperRequest", bnusio_SetHopperRequest, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SetPLCounter", bnusio_SetPLCounter, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SetRegisterU16", bnusio_SetRegisterU16, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SetRegisterU8", bnusio_SetRegisterU8, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SetSystemError", bnusio_SetSystemError, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SramRead", bnusio_SramRead, nullptr);
	MH_CreateHookApi(L"bnusio.dll", "bnusio_SramWrite", bnusio_SramWrite, nullptr);

	MH_CreateHookApi(L"nbamUsbFinder.dll", "nbamUsbFinderInitialize", nbamUsbFinderInitialize, nullptr);
	MH_CreateHookApi(L"nbamUsbFinder.dll", "nbamUsbFinderRelease", nbamUsbFinderRelease, nullptr);
	MH_CreateHookApi(L"nbamUsbFinder.dll", "nbamUsbFinderGetSerialNumber", nbamUsbFinderGetSerialNumber, nullptr);

	MH_CreateHookApi(L"xinput9_1_0.dll", "XInputGetState", XInputGetStateHook, nullptr);
	MH_CreateHookApi(L"xinput9_1_0.dll", "XInputSetState", XInputSetStateHook, nullptr);
	MH_CreateHookApi(L"xinput9_1_0.dll", "XInputGetCapabilities", XInputGetCapabilitiesHook, nullptr);

	MH_CreateHookApi(L"ssleay32.dll", "SSL_shutdown", SSL_shutdownHook, reinterpret_cast<void**>(&SSL_shutdownOriginal));

	MH_EnableHook(nullptr);
}, GameID::TaikoV8);
#endif