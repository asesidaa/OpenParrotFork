// ReSharper disable CppClangTidyPerformanceNoIntToPtr
// ReSharper disable CppClangTidyCppcoreguidelinesInterfacesGlobalInit
// ReSharper disable CppClangTidyClangDiagnosticMicrosoftCast
#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility/Hooking.Patterns.h"
#include <Xinput.h>

#include "Utils/hook.hpp"

#ifdef _M_AMD64

static bool btnTestToggle = false;
static bool btnTestLast = false;
static bool btnCoinLast = false;

static bool btnP1RimR = false;
static bool btnP1CenterR = false;
static bool btnP1CenterL = false;
static bool btnP1RimL = false;

static bool btnP2RimR = false;
static bool btnP2CenterR = false;
static bool btnP2CenterL = false;
static bool btnP2RimL = false;

static uint16_t coinCount = 0;
extern int* ffbOffset;

static HWND (WINAPI* CreateWindowExWOri)(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth,
                                         int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
static HWND WINAPI CreateWindowExWHook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth,
                                       int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
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

static BOOL (WINAPI* ShowCursorOri)(BOOL bShow);
static BOOL WINAPI ShowCursorHook(BOOL bShow) {
    if (ToBool(config["General"]["HideCursor"]))
        return ShowCursorOri(false);
    else
        return ShowCursorOri(true);
}

static int64_t bnusio_ClearSram() {
    //info(true, "bnusio_ClearSram");
    return 0;
}

static int64_t bnusio_Close() {
    //info(true, "bnusio_Close");
    return 0;
}

static int64_t __fastcall bnusio_Communication(int a1) {
    //info(true, "bnusio_Communication");
    return 0;
}

static int64_t __fastcall bnusio_DecCoin(int a1, uint16_t a2) {
    //info(true, "bnusio_DecCoin");
    return 0;
}

static int64_t __fastcall bnusio_DecService(int a1, uint16_t a2) {
    //info(true, "bnusio_DecService");
    return 0;
}

// Return a random value to simulate the arcade drum
static uint16_t rand16(void) {
    int random;
    int max_value = 20000; // ~ 90 in I/O test menu
    int min_value = 10000; // ~ 30 in I/O test menu

    random = rand() % max_value + min_value; // NOLINT(concurrency-mt-unsafe)
    uint16_t r = static_cast<unsigned>(random); // NOLINT(clang-diagnostic-implicit-int-conversion)

    return r;
}

static uint16_t __fastcall bnusio_GetAnalogIn(uint8_t a1) {
    //info(true, "bnusio_GetAnalogIn a1: %u", a1);

    uint16_t rv = 0;

    // Player 1 Drum Rim Left
    if (a1 == 0)
    {
        bool currentBtn = (bool)(*ffbOffset & 0x40);

        if (currentBtn && btnP1RimL != currentBtn)
            rv = rand16();

        btnP1RimL = currentBtn;
    }

    // Player 1 Drum Center Left
    else if (a1 == 1)
    {
        bool currentBtn = (bool)(*ffbOffset & 0x80);

        if (currentBtn && btnP1CenterL != currentBtn)
            rv = rand16();

        btnP1CenterL = currentBtn;
    }

    // Player 1 Drum Center Right
    else if (a1 == 2)
    {
        bool currentBtn = (bool)(*ffbOffset & 0x100);

        if (currentBtn && btnP1CenterR != currentBtn)
            rv = rand16();

        btnP1CenterR = currentBtn;
    }

    // Player 1 Drum Rim Right
    else if (a1 == 3)
    {
        bool currentBtn = (bool)(*ffbOffset & 0x200);

        if (currentBtn && btnP1RimR != currentBtn)
            rv = rand16();

        btnP1RimR = currentBtn;
    }

    // Player 2 Drum Rim Left
    else if (a1 == 4)
    {
        bool currentBtn = (bool)(*ffbOffset & 0x400);

        if (currentBtn && btnP2RimL != currentBtn)
            rv = rand16();

        btnP2RimL = currentBtn;
    }

    // Player 2 Drum Center Left
    else if (a1 == 5)
    {
        bool currentBtn = (bool)(*ffbOffset & 0x800);

        if (currentBtn && btnP2CenterL != currentBtn)
            rv = rand16();

        btnP2CenterL = currentBtn;
    }

    // Player 2 Drum Center Right
    else if (a1 == 6)
    {
        bool currentBtn = (bool)(*ffbOffset & 0x1000);

        if (currentBtn && btnP2CenterR != currentBtn)
            rv = rand16();

        btnP2CenterR = currentBtn;
    }

    // Player 2 Drum Rim Right
    else if (a1 == 7)
    {
        bool currentBtn = (bool)(*ffbOffset & 0x2000);

        if (currentBtn && btnP2RimR != currentBtn)
            rv = rand16();

        btnP2RimR = currentBtn;
    }

    return rv;
}

static void* __fastcall bnusio_GetBuffer(uint16_t a1, int64_t a2, int16_t a3) {
    //info(true, "bnusio_GetBuffer");
    return nullptr;
}

static int64_t __fastcall bnusio_GetCDOut(uint16_t a1) {
    //info(true, "bnusio_GetCDOut");
    return 0;
}

static uint16_t __fastcall bnusio_GetCoin(int player) {
    //info(true, "bnusio_GetCoin player: %d", player);
    return (uint16_t)coinCount;
}

static void* __fastcall bnusio_GetCoinError(int player) {
    //info(true, "bnusio_GetCoinError player: %d", player);
    return nullptr;
}

static int64_t __fastcall bnusio_GetCoinLock(uint8_t a1) {
    //info(true, "bnusio_GetCoinLock");
    return 0;
}

static uint64_t bnusio_GetEncoder() {
    //info(true, "bnusio_GetEncoder");
    return 0;
}

static void* bnusio_GetExpansionMode() {
    //info(true, "bnusio_GetExpansionMode");
    return nullptr;
}

static void* bnusio_GetFirmwareVersion() {
    //info(true, "bnusio_GetFirmwareVersion");
    return reinterpret_cast<void*>(static_cast<uint16_t>(126)); // NOLINT(performance-no-int-to-ptr)
}

static char __fastcall bnusio_GetGout(uint8_t a1) {
    //info(true, "bnusio_GetGout");
    return 0;
}

static int64_t __fastcall bnusio_GetHopOut(uint8_t a1) {
    //info(true, "bnusio_GetHopOut");
    return 0;
}

static void* bnusio_GetIoBoardName() {
    //info(true, "bnusio_GetIoBoardName");
    return nullptr;
}

static int64_t __fastcall bnusio_GetRegisterU16(int16_t a1) {
    //info(true, "bnusio_GetRegisterU16");
    return 0;
}

static char __fastcall bnusio_GetRegisterU8(uint16_t a1) {
    //info(true, "bnusio_GetRegisterU8");
    return 0;
}

static void* __fastcall bnusio_GetService(int a1) {
    //info(true, "bnusio_GetService");
    return nullptr;
}

static void* __fastcall bnusio_GetServiceError(int a1) {
    //info(true, "bnusio_GetServiceError");
    return nullptr;
}

static void* __fastcall bnusio_GetStatusU16(uint16_t a1) {
    //info(true, "bnusio_GetStatusU16");
    return nullptr;
}

static void* __fastcall bnusio_GetStatusU8(uint16_t a1) {
    //info(true, "bnusio_GetStatusU8");
    return nullptr;
}

static uint32_t bnusio_GetSwIn() {
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

static uint64_t bnusio_GetSwIn64() {
    //info(true, "bnusio_GetSwIn64");
    return 0;
}

static void* bnusio_GetSystemError() {
    //info(true, "bnusio_GetSystemError");
    return nullptr;
}

static char bnusio_IsConnected() {
    //info(true, "bnusio_IsConnected");
    return 0;
}

static char bnusio_IsWideUsio() {
    //info(true, "bnusio_IsWideUsio");
    return 0;
}

static int64_t bnusio_Open() {
    //info(true, "bnusio_Open");
    return 0; // 1=busy
}

static int64_t bnusio_ResetCoin() {
    //info(true, "bnusio_ResetCoin");
    return 0;
}

static BOOL bnusio_ResetIoBoard() {
    //info(true, "bnusio_ResetIoBoard");
    return 0;
}

static int64_t __fastcall bnusio_SetBuffer(uint16_t a1, int a2, int16_t a3) {
    //info(true, "bnusio_SetBuffer");
    return 0;
}

static int64_t __fastcall bnusio_SetCDOut(uint8_t a1, char a2) {
    //info(true, "bnusio_SetCDOut");
    return 0;
}

static int64_t __fastcall bnusio_SetCoinLock(uint8_t a1, char a2) {
    //info(true, "bnusio_SetCoinLock");
    return 0;
}

static int64_t __fastcall bnusio_SetExpansionMode(int16_t a1) {
    //info(true, "bnusio_SetExpansionMode");
    return 0;
}

static int64_t __fastcall bnusio_SetGout(uint8_t a1, uint8_t a2) {
    //info(true, "bnusio_SetGout a1: %u a2: %u", a1, a2);
    return 0; // outputs
}

static int64_t __fastcall bnusio_SetHopOut(uint8_t a1, char a2) {
    //info(true, "bnusio_SetHopOut");
    return 0;
}

static int64_t __fastcall bnusio_SetHopperLimit(uint16_t a1, int16_t a2) {
    //info(true, "bnusio_SetHopperLimit");
    return 0;
}

static int64_t __fastcall bnusio_SetHopperRequest(uint16_t a1, int16_t a2) {
    //info(true, "bnusio_SetHopperRequest");
    return 0;
}

static void* __fastcall bnusio_SetPLCounter(int16_t a1) {
    //info(true, "bnusio_SetPLCounter a1: %d", a1);
    return nullptr;
}

static int64_t __fastcall bnusio_SetRegisterU16(uint16_t a1, int16_t a2) {
    //info(true, "bnusio_SetRegisterU16");
    return 0;
}

static int64_t __fastcall bnusio_SetRegisterU8(uint16_t a1, char a2) {
    //info(true, "bnusio_SetRegisterU8");
    return 0;
}

static int64_t __fastcall bnusio_SetSystemError(int16_t a1) {
    //info(true, "bnusio_SetSystemError");
    return 0;
}

static int64_t __fastcall bnusio_SramRead(int a1, uint8_t a2, int a3, uint16_t a4) {
    //info(true, "bnusio_SramRead");
    return 0;
}

static int64_t __fastcall bnusio_SramWrite(int a1, uint8_t a2, int a3, uint16_t a4) {
    //info(true, "bnusio_SramWrite");
    return 0;
}

static int64_t nbamUsbFinderInitialize() {
    //info(true, "nbamUsbFinderInitialize");
    return 0;
}

static int64_t nbamUsbFinderRelease() {
    //info(true, "nbamUsbFinderRelease");
    return 0;
}

static int64_t __fastcall nbamUsbFinderGetSerialNumber(int a1, int64_t a2) {
    info(true, "nbamUsbFinderGetSerialNumber a1: %u a2: %p", a1, a2);
    auto result = reinterpret_cast<char*>(a2);
    strcpy_s(reinterpret_cast<char*>(a2), 13, "284111080001");
    result[12] = '\0';
    info(true, "%s", a2);
    return 0;
}

// XInputs hooks to disable built in XInput screwing up our input emu
static DWORD XInputGetStateHook(DWORD dwUserIndex, XINPUT_STATE* pState) {
    //info(true, "XInputGetStateHook");
    return ERROR_DEVICE_NOT_CONNECTED;
}

static DWORD XInputSetStateHook(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration) {
    //info(true, "XInputSetStateHook");
    return ERROR_DEVICE_NOT_CONNECTED;
}

static DWORD XInputGetCapabilitiesHook(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities) {
    //info(true, "XInputGetCapabilitiesHook");
    return ERROR_DEVICE_NOT_CONNECTED;
}
static int (*SSL_shutdownOriginal)(void* ssl);
static int SSL_shutdownHook(void* ssl) {
    SSL_shutdownOriginal(ssl);
    return 1;
}

[[maybe_unused]] static InitFunction TaikoV0Func([]()
{
    uintptr_t imageBase = (uintptr_t)GetModuleHandleA(nullptr);
    uintptr_t amBase = (uintptr_t)GetModuleHandleA("AMFrameWork.dll");

    // Skip ExitWindowsEx (reboots pc when debugging)
    injector::MakeNOP(amBase + 0x33C71, 10);

    char settings1[] = ".\\Setting1.bin";
    char settings2[] = ".\\Setting2.bin";
    // Path fixes
    injector::WriteMemoryRaw(imageBase + 0x9D23C8, settings2, 15, true); // g:\\Setting2.bin
    injector::WriteMemoryRaw(imageBase + 0x9D23B8, settings1, 15, true); // f:\\Setting1.bin


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
        injector::WriteMemory<float>(imageBase + 0x9ECEC4, 0.0166800003498793f / 120.0f * (float)aniFps, true);
        // Model Animation 60 FPS Frame Time Factor
        injector::WriteMemory<double>(imageBase + 0x9ED078, (double)aniFps, true); // Lua Common.FPS
        injector::WriteMemory<float>(imageBase + 0x9ED140, (float)aniFps, true); // Compressed Animation Frame Rate
    }

    if (ToBool(config["General"]["UnlockAllSongs"]))
    {
        char unlock[] = "\xB0\x01";
        injector::WriteMemoryRaw(imageBase + 0x1F6B78, unlock, 2, true); // 32 C0 (XOR AL, AL) -> B0 01 (MOV AL, 1)
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

typedef uint8_t (__fastcall *qrVtable1)(uint64_t a1);
qrVtable1 originalqrVtable1 = nullptr;
uintptr_t whereqrVtable1 = NULL;
uint8_t __fastcall implOfqrVtable1(uint64_t a1) {
    return 1;
}


typedef uint8_t (__fastcall *qrReadFromCOM1)(uint64_t a1);
qrReadFromCOM1 originalqrReadFromCOM1 = nullptr;
uintptr_t whereqrReadFromCOM1 = NULL;
uint8_t __fastcall implOfqrReadFromCOM1(uint64_t a1) {
    *(uint32_t*)(a1 + 40) = 1;
    *(uint32_t*)(a1 + 16) = 1;
    return 1;
}

static constexpr uintptr_t baseAddress = 0x00007FF6A8560000;

static constexpr uintptr_t addresses[] =
{
    // 1599
    0x00007FF6A88539E4 + 2,
    0x00007FF6A8853AAE + 2,
    0x00007FF6A8853BE2 + 2,
    0x00007FF6A8866438 + 3,
    0x00007FF6A8866504 + 3,
    0x00007FF6A88665D0 + 3,
    0x00007FF6A88666F8 + 3,
    0x00007FF6A88667C4 + 3,
    0x00007FF6A8866890 + 3,
    0x00007FF6A8866988 + 3,
    0x00007FF6A8873664 + 2,
    0x00007FF6A8873724 + 2,
    0x00007FF6A88739F2 + 2,
    0x00007FF6A8873B02 + 2,
    0x00007FF6A8873C22 + 2,
    0x00007FF6A8873CF2 + 2,
    0x00007FF6A88740C2 + 2,
    0x00007FF6A88747A8 + 2,
    // 1600
    0x00007FF6A8785FB5 + 1,
    0x00007FF6A8786145 + 1,
    0x00007FF6A8874DCA + 2,
    0x00007FF6A8874EC7 + 2,
    0x00007FF6A8898E2A + 2,
    0x00007FF6A864E0A2 + 2,
    0x00007FF6A864E8B3 + 2,
    0x00007FF6A864EDA4 + 2,
    0x00007FF6A8875606 + 2,
    0x00007FF6A88AA7E9 + 2,
    0x00007FF6A8853CB1 + 2,
    0x00007FF6A8874057 + 2,
    0x00007FF6A8786060 + 3,
    0x00007FF6A878609C + 3,
    0x00007FF6A8786293 + 3,
    0x00007FF6A8866A2B + 3,
    0x00007FF6A8874F43 + 3,
    0x00007FF6A8874F94 + 3
};
constexpr int song_data_size = 1024 * 1024 * 64;
static char* song_data = new char[song_data_size]{ };

static void* set_crown_data_stub(uintptr_t jump_back_address, int value) {
    using namespace asmjit::x86;
    return utils::hook::assemble([jump_back_address, value](utils::hook::assembler& a)
    {
        const auto exit = a.newLabel();
        a.mov(rdx, song_data);
        a.mov(rax, ptr(rsp, 0x40));
        a.add(rax, r14);
        a.lea(rcx, ptr(rax, rax, 3));
        a.cmp(dword_ptr(rdx, rcx, 3, 0x300), value);
        a.jge(exit);
        a.mov(dword_ptr(rdx, rcx, 3, 0x300), value);

        a.bind(exit);
        a.jmp(jump_back_address);
    });
}
static void* set_score_rank_stub(uintptr_t jump_back_address, int value) {
    using namespace asmjit::x86;
    return utils::hook::assemble([jump_back_address, value](utils::hook::assembler& a)
    {
        const auto exit = a.newLabel();
        a.mov(rdx, song_data);
        a.lea(rcx, ptr(0x0B, rbx, 2));
        a.add(rcx, rsi);
        a.add(rcx, rbx);
        a.lea(rax, ptr(rcx, rcx, 3));
        a.cmp(dword_ptr(rdx, rax, 3), value);
        a.jge(exit);
        a.mov(dword_ptr(rdx, rax, 3), value);

        a.bind(exit);
        a.jmp(jump_back_address);
    });
}

static void* set_unknown_data_stub(uintptr_t jump_back_address,
                                   const asmjit::x86::Gp& mov,
                                   const asmjit::x86::Gp& lea1, const asmjit::x86::Gp& lea2,
                                   const asmjit::x86::Gp& add1, const asmjit::x86::Gp& add2,
                                   const asmjit::x86::Gp& lea2_1, const asmjit::x86::Gp& lea2_2) {
    using namespace asmjit::x86;
    return utils::hook::assemble(
        [jump_back_address, &mov, &lea1, &lea2, &add1, &add2, &lea2_1, &lea2_2](utils::hook::assembler& a)
        {
            a.mov(mov, song_data);
            a.lea(lea1, ptr(lea2, lea2, 2));
            a.add(add1, add2);
            a.lea(lea2_1, ptr(lea2_2, lea2_2, 3));

            a.jmp(jump_back_address);
        });
}

static void* set_unknown_data_stub_2(uintptr_t jump_back_address) {
    using namespace asmjit::x86;
    return utils::hook::assemble([jump_back_address](utils::hook::assembler& a)
    {
        a.mov(rdx, song_data);
        a.lea(rcx, ptr(rdi, rdi, 2));
        a.lea(rax, ptr(rbx, 0x0B));
        a.add(rcx, rax);
        a.lea(rax, ptr(rcx, rcx, 3));

        a.jmp(jump_back_address);
    });
}

static void* set_crown_related(uintptr_t jump_back_address) {
    using namespace asmjit::x86;
    return utils::hook::assemble([jump_back_address](utils::hook::assembler& a)
    {
        a.mov(r8, song_data);
        a.lea(rdx, ptr(rdi, rdi, 2));
        a.add(rdx, rbx);
        a.lea(rax, ptr(rdx, rdx, 3));
        a.mov(byte_ptr(r8, rax, 3, 0x31C), 1);

        a.jmp(jump_back_address);
    });
}

static void* get_song_data_stub(uintptr_t jump_back_address) {
    using namespace asmjit::x86;
    return utils::hook::assemble([jump_back_address](utils::hook::assembler& a)
    {
        a.mov(r8, song_data);
        a.movsxd(rax, edi);
        a.lea(rdx, ptr(rax, rax, 2));
        a.movsxd(rax, ebx);
        a.add(rdx, rax);
        a.lea(rax, ptr(rdx, rdx, 3));
        a.lea(rbx, ptr(r8, rax, 3));
        a.jmp(jump_back_address);
    });
}

[[maybe_unused]]
static InitFunction TaikoV8Func([]
{
    using namespace asmjit::x86;
    
    uintptr_t imageBase = (uintptr_t)GetModuleHandleA(nullptr);
    uintptr_t amBase = (uintptr_t)GetModuleHandleA("AMFrameWork.dll");

    // Skip ExitWindowsEx (reboots pc when debugging)
    injector::MakeNOP(amBase + 0x35AB1, 10);

    char settings1[] = ".\\Setting1.bin";
    char settings2[] = ".\\Setting2.bin";
    // Path fixes
    injector::WriteMemoryRaw(imageBase + 0xB5C538, settings2, 15, true); // g:\\Setting2.bin
    injector::WriteMemoryRaw(imageBase + 0xB5C528, settings1, 15, true); // f:\\Setting1.bin

    char current_path[] = "./";
    // Move F: cabinet files to current directory and fix string length
    injector::WriteMemoryRaw(imageBase + 0xB1B4B0, current_path, 3, true);
    injector::WriteMemory<BYTE>(imageBase + 0x1C941, 0x02, true);

    linb::ini unlock_config;
    unlock_config.load_file("unlock.ini");
    // Unlock song limit
    if (ToBool(unlock_config["General"]["UnlockSongLimit"]))
    {
        for (const auto address : addresses)
        {
            // Plus 2 is address to immediate value
            const auto offset = address - baseAddress;
            injector::WriteMemory<DWORD>(imageBase + offset, 4000, true);
        }

        // Crowns
        utils::hook::jump(0x2F39FE + imageBase, set_crown_data_stub(imageBase + 0x2F3A27, 2), true);
        utils::hook::jump(0x2F3AC8 + imageBase, set_crown_data_stub(imageBase + 0x2F3AF1, 1), true);
        utils::hook::jump(0x2F3BFC + imageBase, set_crown_data_stub(imageBase + 0x2F3C25, 3), true);


        // Score ranks
        utils::hook::jump(0x306452 + imageBase, set_score_rank_stub(imageBase + 0x30647B, 3), true);
        utils::hook::jump(0x30651E + imageBase, set_score_rank_stub(imageBase + 0x306547, 2), true);
        utils::hook::jump(0x3065EA + imageBase, set_score_rank_stub(imageBase + 0x306613, 1), true);
        utils::hook::jump(0x306712 + imageBase, set_score_rank_stub(imageBase + 0x30673B, 6), true);
        utils::hook::jump(0x3067DE + imageBase, set_score_rank_stub(imageBase + 0x306807, 5), true);
        utils::hook::jump(0x3068AA + imageBase, set_score_rank_stub(imageBase + 0x3068D3, 4), true);
        utils::hook::jump(0x3069A2 + imageBase, set_score_rank_stub(imageBase + 0x3069D0, 7), true);

        // Generic (unknown)
        utils::hook::jump(0x313755 + imageBase,
            set_unknown_data_stub(imageBase + 0x31376A, rdx,
                rcx, rsi,
                rcx, rdi,
                rax,rcx), true);
        utils::hook::jump(0x313A0B + imageBase,
            set_unknown_data_stub(imageBase + 0x313A20, rdx,
                rcx, rdi,
                rcx, rbx,
                rax,rcx), true);
        utils::hook::jump(0x313B4C + imageBase,
            set_unknown_data_stub(imageBase + 0x313B61, rdx,
                rcx, rdi,
                rcx, rbx,
                rax, rcx), true);
        utils::hook::jump(0x313C42 + imageBase,
            set_unknown_data_stub(imageBase + 0x313C57, r8,
            rdx, rdi,
            rdx, rbx,
            rdx, rdx), true);

        // Generic type 2
        utils::hook::jump(0x313D38 + imageBase, set_unknown_data_stub_2(imageBase + 0x313D51), true);

        // Used in crown
        utils::hook::jump(0x3140D7 + imageBase, set_crown_related(imageBase + 0x3140EC), true);
        
        // Get data
        utils::hook::jump(0x31367B + imageBase, get_song_data_stub(imageBase + 0x31369A), true);
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
        injector::WriteMemory<float>(imageBase + 0xB7755C, 0.0166800003498793f / 120.0f * (float)aniFps, true);
        // Model Animation 60 FPS Frame Time Factor
        injector::WriteMemory<double>(imageBase + 0xB77730, (double)aniFps, true); // Lua Common.FPS
        injector::WriteMemory<float>(imageBase + 0xB77814, (float)aniFps, true); // Compressed Animation Frame Rate
    }

    if (ToBool(config["General"]["UnlockAllSongs"]))
    {
        char unlock[] = "\xB0\x01";
        injector::WriteMemoryRaw(imageBase + 0x314E8D, unlock, 2, true); // 32 C0 (XOR AL, AL) -> B0 01 (MOV AL, 1)
    }

    if (ToBool(config["General"]["SharedAudioMode"]))
    {
        injector::WriteMemory<BYTE>(imageBase + 0x692E17, 0xEB, true); // 74 (JZ) -> EB (JMP)
    }
    // Hooks
    MH_Initialize();


    whereqrVtable1 = amBase + 0x1BA00;
    MH_CreateHook(LPVOID(whereqrVtable1), LPVOID(implOfqrVtable1), reinterpret_cast<void**>(&originalqrVtable1));

    whereqrReadFromCOM1 = amBase + 0x1BC20;
    MH_CreateHook(LPVOID(whereqrReadFromCOM1), LPVOID(implOfqrReadFromCOM1), reinterpret_cast<void**>(&originalqrReadFromCOM1));

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