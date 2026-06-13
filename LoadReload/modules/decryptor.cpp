#include "decryptor.h"
#include <vector>
#include "../config.h"


namespace Decryptor {

    // XOR Routines for decrypting from a input to a function or in place to a certain length of buffer

    std::vector<uint8_t> Decrypt(const std::vector<uint8_t>& input) {
        std::vector<uint8_t> output = input;
        std::vector<uint8_t> xorKey(XOR_KEY);
        if (!xorKey.empty()) {
            for (size_t i = 0; i < output.size(); ++i) {
                output[i] ^= xorKey[i % xorKey.size()];
            }
        }
        return output;
    }

    bool DecryptInPlace(uint8_t* buffer, size_t size) {
        std::vector<uint8_t> xorKey(XOR_KEY);
        if (xorKey.empty()) {
            return false;
        }

        for (size_t i = 0; i < size; ++i) {
            buffer[i] ^= xorKey[i % xorKey.size()];
        }

        return true;
    }


}
