#pragma once
#include <stdint.h>

static inline uint16_t toBigEndian16(uint16_t v) {
    return (v >> 8) | (v << 8);
}

static inline uint16_t fromBigEndian16(uint16_t v) {
    return (v >> 8) | (v << 8);
}

static inline int16_t fromBigEndianInt16(const uint8_t* b) {
    return (int16_t)((b[0] << 8) | b[1]);
}
