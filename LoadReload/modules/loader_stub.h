#pragma once
#include <vector>
#include <cstdint>
#include <windows.h>
#include <stdio.h>
#include <string>


namespace LoaderStub {

	// Load binary file from disk
    std::vector<uint8_t> LoadBinaryFile(const std::string& path);

    // Execute payload in current process
    void Execute(std::vector<uint8_t>& payload);
}
