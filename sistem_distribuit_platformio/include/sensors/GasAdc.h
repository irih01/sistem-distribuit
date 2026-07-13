#pragma once
#include <Arduino.h>
#include "Sensor.h"
#include <Wire.h>

class GasAdc : public Sensor {
public:
    GasAdc(uint8_t i2cAddress, TwoWire* wire) : address(i2cAddress), wire(wire){}

    bool begin() override {
        wire->beginTransmission(address);
        return wire->endTransmission() == 0;
    }

    void update() override {
        wire->requestFrom(address, (uint8_t)2);
        if (wire->available() == 2){
            raw = (wire->read() << 8) | wire->read();
        }

        // Calcul delta + scor
        delta = raw - baseline;
        // delta>0: aer incarcat; delta=0 aer normal
        score = constrain((delta * 100 ) / 6000, 0, 100);
    }

    int16_t raw = 0;
    int16_t baseline = 12600;
    int16_t delta = 0;
    int16_t score = 0;

    // Helper pentru conversia calității în ID numeric pentru protocol
    uint8_t getQualityId() const {
        if (score < 10) return 0; // EXCELENT
        if (score < 25) return 1; // BUN
        if (score < 50) return 2; // MODERAT
        if (score < 75) return 3; // SARAC
        return 4;                 // FOARTE_SARAC
    }

    const char* quality() const {
        if (score < 10) return "EXCELENT";
        if (score < 25) return "BUN";
        if (score < 50) return "MODERAT";
        if (score < 75) return "SARAC";
        return "FOARTE_SARAC";
    }
    

    
    void print(Stream& out) override{
        out.println(F("[GAS ADC]"));
        out.print(F(" Raw: "));
        out.print(raw);
        out.println(F(" adc"));

        out.print(F(" Delta: "));
        out.print(delta);
        out.println(F(" concentratie VOC"));

        out.print(F(" Score: "));
        out.print(score);
        out.println(F(" intensitate relativa"));

        out.print(F(" Calitate: "));
        out.println(quality());
    }
    
    const char* name() const override{
        return "gas";
    }
    
    void toJson(JsonObject obj) override{
        obj["type"] = "VOC (relaitv)";
        obj["raw"]["value"] = raw;
        obj["raw"]["unit"] = "variatie relativa";
        obj["delta"]["value"] = delta;
        obj["delta"]["unit"] = "concentratie VOC";
        obj["score"]["value"] = score;
        obj["score"]["unit"] = "index_0_100";
        obj["quality"] = quality();
    }
    
    struct GasPacket{
        int16_t raw;
        int16_t delta;
    };
    
    BinaryPacket toBinary() const override{
        BinaryPacket p{};
        p.type = 3;
        p.length = sizeof(GasPacket);

        GasPacket pkt{raw, delta};
        memcpy(p.data, &pkt, sizeof(pkt));
        
        return p;
    }
    
    String toCsv() const override{
        String s;
        s.reserve(32);
        s += String(raw) + ",";
        s += String(delta) + ",";
        s += String(score) + ",";
        s += String(quality()) + ",";
        return s;
    }

    String csvHeader() const override{
        return F("raw, delta, score, quality");
    }
private:
    uint8_t address;
    TwoWire* wire;
};