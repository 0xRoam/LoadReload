#include "guardrails.h"
#include <windows.h>

namespace Guardrails {
	// Check Number of CPU Cores
	bool CheckCpuCores(unsigned int minCores) {
		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);
		if (sysInfo.dwNumberOfProcessors <= minCores) {
			return true; // Insufficient cores
		}
	return false; // Sufficient cores
	}
}