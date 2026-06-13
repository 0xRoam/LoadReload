#pragma once
#include <windows.h>
#include <vector>
#include <cstdint>
#include <iostream>

namespace Module_Stomping {
    bool PatchSectionAndExecute(LPCWSTR dllPath, const std::vector<uint8_t>& shellcode);
    
} // namespace Module_Stomping