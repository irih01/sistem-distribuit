#pragma once

#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "Sensor.h"
#include <BinaryProtocol.h>

class Bmp280Sensor : public Sensor {
    public:
        Bmp280Sensor(TwoWire* wire, uint8_t addr = 0x77) : bmp(wire), address(addr){}

        float temperature = 0.0f;
        float pressure = 0.0f;
        // float altitude = 0.0f;
        
        bool begin() override {
            if (!bmp.begin(address)){
                return false;
            }
            bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);
            
            return true;
        }

        void update() override {
            temperature = bmp.readTemperature();
            pressure = bmp.readPressure() / 100.0f;
            // altitude = bmp.readAltitude(1013.25);
        }


        void print(Stream& out) override{
            out.println(F("[BMP280]"));
            out.print(F(" Temperatura: "));
            out.print(temperature);
            out.println(F(" °C"));

            out.print(F(" Presiune: "));
            out.print(pressure);
            out.println(F(" hPa"));
            out.print(F("\n"));

            // out.print(F(" Altitudine: "));
            // out.print(altitude);
            // out.println(F(" m"));
            // out.print(F("\n"));
        }
    
        const char* name() const override{
            return "bmp280";
        }

        void toJson(JsonObject obj) override{
            obj["temperatura"]["value"] = temperature;
            obj["temperatura"]["unit"] = "°C";
            obj["presiune"]["value"] = pressure;
            obj["presiune"]["unit"] = "hPa";
            // obj["altitudine"]["value"] = altitude;
            // obj["altitudine"]["unit"] = "m";
        }

        String toCsv() const override{
            String s;
            s.reserve(32);
            s += String(temperature) + ",";
            s += String(pressure);
            //s += String(altitude);
            return s;
        }
        String csvHeader() const override{
            return F("temp_bmp,pressure");
        }

       

        BinaryPacket toBinary() const override{
            BinaryPacket p{};
            p.type = SENSOR_BMP280;
            p.length = sizeof(BmpPacket);

            BmpPacket pkt{temperature, pressure};
            memcpy(p.data, &pkt, sizeof(pkt));
            
            return p;
        }
    private:
        Adafruit_BMP280 bmp;
        uint8_t address;
        
};