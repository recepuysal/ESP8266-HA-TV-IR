#pragma once

#include <stdint.h>

// TV On/Off sinyali — Serial Monitor'dan okunan degerler
// Protokol : NEC
// Hex      : 0x4FB4AB5
// Bits     : 32
constexpr uint64_t TV_POWER_CODE = 0x4FB4AB5ULL;
constexpr uint16_t TV_POWER_BITS = 32;
