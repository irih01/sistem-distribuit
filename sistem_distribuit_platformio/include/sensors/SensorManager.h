#pragma once

#include "Sensor.h"
#include <Arduino.h>

class SensorManager {
public:
    SensorManager(Sensor** sensors, uint8_t count) : sensors(sensors), count(count){}

    void beginAll(){
        for (uint8_t i = 0; i < count; i++){
            sensors[i]->begin();
        }
    }

    void updateAll(){
        for(uint8_t i = 0; i < count; i++){
            sensors[i]->update();
        }
    }

    void printAll(Stream& out){
        out.println(F("==== SENSOR DATA ===="));
        for(uint8_t i = 0; i < count; i ++){
            sensors[i]->print(out);
        }
        out.println(F("======================="));
    }

    void toJson(JsonDocument& doc){
        for (uint8_t i = 0; i < count; i++){
            JsonObject sensorObj = doc[sensors[i]->name()].to<JsonObject>();
            sensors[i]->toJson(sensorObj);
        }
    }

    String csvHeader() const{
        String s;
        for (uint8_t i = 0; i < count; i++){
            s += sensors[i]->csvHeader();
            if(i < count-1) s += ",";
        }
        return s;
    }

    String toCsv() const{
        String s;
        for(uint8_t i = 0; i < count; i++){
            s += sensors[i]->toCsv();
            if(i < count-1) s += ",";
        }
        return s;
    }

    void toBinary(uint8_t* buffer) const{
        /*[ BMP ][ AHT ][ GAS ]
            16B    16B     16B*/
        uint8_t* ptr = buffer;
        for(uint8_t i = 0; i < count; i++){
            auto pkt = sensors[i]->toBinary();
            memcpy(ptr, &pkt, sizeof(pkt));
            ptr += sizeof(pkt);
        }
    }
    
private:
    Sensor** sensors;
    uint8_t count;
};