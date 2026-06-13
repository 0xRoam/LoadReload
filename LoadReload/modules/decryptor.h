#pragma once
#include <vector>
#include <cstdint>
namespace Decryptor {
    std::vector<uint8_t> Decrypt(const std::vector<uint8_t>& input);
    
	bool DecryptInPlace(uint8_t* buffer, size_t size);
}