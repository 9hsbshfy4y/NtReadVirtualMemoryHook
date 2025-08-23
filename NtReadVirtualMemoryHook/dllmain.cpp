#include "includes.h"

typedef NTSYSAPI NTSTATUS(NTAPI* tNtReadVirtualMemory)(
    HANDLE processHandle, PVOID baseAddress, PVOID buffer, 
    ULONG bytesToRead, PULONG readedBytes);

tNtReadVirtualMemory oNtReadVirtualMemory;

int hook(uintptr_t address, void** original, void* function) {
    void* target = (void*)address;
    if (MH_CreateHook(target, function, original) != MH_OK || MH_EnableHook(target) != MH_OK) {
        return -1;
    }
    return 1;
}

NTSTATUS NTAPI NtReadVirtualMemory(HANDLE processHandle, PVOID baseAddress, PVOID buffer, ULONG bytesToRead, PULONG readedBytes) {
    ULONG a = 8;

    ULONG reducedSize = bytesToRead / a;
    if (reducedSize == 0 && bytesToRead > 0) {
        reducedSize = 1;
    }

    NTSTATUS status = oNtReadVirtualMemory(processHandle, baseAddress, buffer, reducedSize, readedBytes);

    if (NT_SUCCESS(status) && readedBytes) {
        *readedBytes = reducedSize;
    }

    return status;
}

DWORD WINAPI MainThread(LPVOID lpReserved) {
    MH_Initialize();
    HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    if (ntdll) {
        uintptr_t target = (uintptr_t)GetProcAddress(ntdll, "NtReadVirtualMemory");
        if (target) {
            hook(target, (void**)&oNtReadVirtualMemory, NtReadVirtualMemory);
        }
    }
    return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        MH_DisableHook(MH_ALL_HOOKS);
        break;
    }
    return TRUE;
}
