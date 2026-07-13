#pragma once

#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include "Sensor.h"

class Aht21Sensor: public Sensor{
public:
    Aht21Sensor(TwoWire* wire): wire(wire){}

    float temperature = 0;
    float humidity = 0;
    
    bool begin() override{
        return aht.begin(wire);
    }

    

    void update() override{
        sensors_event_t hum, temp;
        aht.getEvent(&hum, &temp);
        temperature = temp.temperature;
        humidity = hum.relative_humidity;
    }
    

    void print(Stream& out) override{
            out.println(F("[AHT21]"));
            out.print(F(" Temperatura: "));
            out.print(temperature);
            out.println(F(" °C"));

            out.print(F(" RH: "));
            out.print(humidity);
            out.println(F(" %"));
            out.print(F("\n"));
        }

    const char* name() const override{
            return "aht21";
        }

    void toJson(JsonObject obj) override{
            obj["temperatura"]["value"] = temperature;
            obj["temperatura"]["unit"] = "°C";
            obj["umiditate"]["value"] = humidity;
            obj["umiditate"]["unit"] = "%";
        }

        String toCsv() const override{
            String s;
            s.reserve(32);
            s += String(temperature) + ",";
            s += String(humidity);
            return s;
        }
        String csvHeader() const override{
            return F("temp_aht,humidity");
        }

        BinaryPacket toBinary() const override{
            BinaryPacket p{};
            p.type = SENSOR_AHT21;
            p.length = sizeof(ahtPacket);

            ahtPacket pkt{temperature, humidity};
            memcpy(p.data, &pkt, sizeof(pkt));
            
            return p;
        }
    
private:
    TwoWire* wire;
    Adafruit_AHTX0 aht;

};