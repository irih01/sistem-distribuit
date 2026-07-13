#pragma once
#include <stdint.h>

class NodeTransport{
    public: 
        bool begin();
        bool send(const uint8_t* data, uint16_t len);
};