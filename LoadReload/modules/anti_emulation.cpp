#include <windows.h>
#include <winnt.h>
# include "anti_emulation.h"
#include "../utils/logging.h"


int GetKernelTimeMs() {
    // KUSER_SHARED_DATA addresses (see WinDbg: dt nt!_KUSER_SHARED_DATA)
    const ULONG* TickCountMultiplier = (const ULONG*)0x7ffe0004;
    const LONG* High1Time = (const LONG*)0x7ffe0324;
    const ULONG* LowPart = (const ULONG*)0x7ffe0320;

	// Rebuild the kernel tick count (raw, similar to Windows internals) and convert to milliseconds
    DWORD kernelTime = (*TickCountMultiplier) * ((*High1Time << 8)) +
        ((static_cast<unsigned __int64>(*LowPart) * (*TickCountMultiplier)) >> 24);

    return static_cast<int>(kernelTime);
}

void BusySleepMs(DWORD sleepMs) {
    int start = GetKernelTimeMs();
    // Busy-loop until the specified interval has elapsed
    while ((GetKernelTimeMs() - start) < static_cast<int>(sleepMs)) {

		_mm_pause(); // does nothing, just a hint to CPU to reduce power consumption, might slow down aggressor analysis
    }

}