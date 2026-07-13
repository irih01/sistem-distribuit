#pragma once
#include <APDS9930.h>
#include "Sensor.h"
#include <Wire.h>

class ApdsSensor: public Sensor{
public:
    ApdsSensor(TwoWire* w) : _wire(w){}

    bool begin() override{
        _wire->begin();

        if (!apds.init()) {
            Serial.println(F("APDS9930 init FAIL"));
            return false;
        }

        apds.enableLightSensor(false);
        apds.enableProximitySensor(false);

        Serial.println(F("APDS9930 OK"));
        return true;
    }

    void update() override{
        apds.readAmbientLightLux(ambientLux);
        apds.readProximity(proximity);
       
    }
    

    void print(Stream& out) override{
            out.println(F("[APDS9960]"));
            out.print(F(" Prox: "));
            out.println(proximity);
            out.print(F(" Ambient: "));
            out.println(ambientLux);
        }

    void toJson(JsonObject obj) override {
        obj["proximity"] = proximity;
        obj["ambient"]  = ambientLux;
    }

    String csvHeader() const override {
        return "apds_prox,apds_ambient";
    }

    String toCsv() const override {
        return String(proximity) + "," + String(ambientLux);
    }

    BinaryPacket toBinary() const override {
        BinaryPacket pkt{};
        pkt.type = 0x03;   // APDS9930
        pkt.length = 6;

        // proximity
        pkt.data[0] = proximity >> 8;
        pkt.data[1] = proximity & 0xFF;

        // lux (float → uint16 scaled)
        uint16_t luxScaled = (uint16_t)(ambientLux * 10.0f);
        pkt.data[2] = luxScaled >> 8;
        pkt.data[3] = luxScaled & 0xFF;

        pkt.data[4] = 0;
        pkt.data[5] = 0;
        return pkt;
    }

    const char* name() const override {
        return "apds9930";
    }

    uint16_t getProximity() const { return proximity; }
    uint16_t getAmbientLux() const { return ambientLux; }

private:
    TwoWire* _wire;
    APDS9930 apds;
    uint16_t proximity  = 0;
    float ambientLux = 0.0f;
};
