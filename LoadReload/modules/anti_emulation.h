#pragma once

// Namespace declaration only, implementation lives in anti_emulation.cpp
// sleepy sleep
void BusySleepMs(DWORD sleepMs);

namespace AntiEmulation_Timing {
    void Run();   // entry point for timing-based anti-emulation checks
}

namespace AntiEmulation_CPU {
    void Run();   // entry point for CPU-based anti-emulation checks
}   