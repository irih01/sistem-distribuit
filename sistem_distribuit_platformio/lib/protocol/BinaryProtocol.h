#pragma once
#include <stdint.h>

#pragma pack(push,1)

// ID-urile senzorilor
enum SensorId : uint8_t {
    SENSOR_BMP280 = 1,
    SENSOR_AHT21 = 2,
    SENSOR_GAS = 3,
};

// Definiția nivelurilor de calitate (Aceasta lipsea!)
enum GasQuality : uint8_t {
    GAS_EXCELENT = 0,
    GAS_BUN = 1,
    GAS_MODERAT = 2,
    GAS_SARAC = 3,
    GAS_FOARTE_SARAC = 4
};

// Header generic (doar ID si lungime)
struct PacketHeader {
    uint8_t sensor_id;
    uint8_t payload_len;
};

// Structura date BMP280
struct BmpPacket {
  float temperature;
  float pressure;
};

// Structura date AHT21
struct ahtPacket {
  float temperature;
  float humidity;
};

// Structura date Gas (trebuie sa fie identica cu cea de pe Node!)
struct GasPayload {
  int16_t raw;
  int16_t delta;
  uint8_t score;
  uint8_t quality; // ID-ul numeric (0-4) din enum-ul de mai sus
};

// Functie helper
const char* GasQualityToStr(uint8_t q);

#pragma pack(pop)