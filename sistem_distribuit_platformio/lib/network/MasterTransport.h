#pragma once
#include <stdint.h>

#ifdef ARDUINO_ARCH_ESP32
    #include <WiFi.h>
#else
    #include <WiFiS3.h>
#endif

class MasterTransport{
    public:
        bool begin(uint16_t port);
        int recieve(uint8_t* buffer, uint16_t maxLen);
    private:
    WiFiServer server{0};
};