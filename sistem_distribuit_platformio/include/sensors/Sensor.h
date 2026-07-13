#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

class Sensor {
    public:
    virtual bool begin() = 0;
    virtual void update() = 0;
    virtual void print(Stream& out) = 0;
    virtual void toJson(JsonObject obj) = 0;
    virtual String toCsv() const { return "";}
    virtual String csvHeader() const {return "";}
    struct BinaryPacket {
        uint8_t type;       //ex: 1 = gaz, 2 = bmp, 3 = aht
        uint8_t length;     // bytes valizi
        uint8_t data[14];
    };
    virtual BinaryPacket toBinary() const {return BinaryPacket();}
    virtual const char* name() const = 0;
    virtual ~Sensor() = default;
};

