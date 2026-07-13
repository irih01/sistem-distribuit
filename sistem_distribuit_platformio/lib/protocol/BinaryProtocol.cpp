#include "BinaryProtocol.h"

const char* GasQualityToStr(uint8_t q){
    switch (q)
    {
    case GAS_EXCELENT:      return "EXCELENT";
    case GAS_BUN:           return "BUN";
    case GAS_MODERAT:       return "MODERAT";
    case GAS_SARAC:         return "SARAC";
    case GAS_FOARTE_SARAC:  return "FOARTE SARAC";
    default:                return "UNKNOWN";
    }
}