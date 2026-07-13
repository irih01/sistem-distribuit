#pragma once

#include "Sensor.h"
#include <DHT.h>

class Dht22Sensor : public Sensor {
public:
    Dht22Sensor(uint8_t pin) : dht(pin, DHT22) {}

    bool begin() override {
        dht.begin();
        lastRead = millis();
        return true;
    }

    void update() override {
        if (millis() - lastRead < 2000) return; // DHT22 limit
        lastRead = millis();

        float t = dht.readTemperature();
        float h = dht.readHumidity();

        if (!isnan(t)) temperature = t;
        if (!isnan(h)) humidity = h;
    }

    void print(Stream& out) override {
        out.println(F("[DHT22]"));
        out.print(F(" Temp: "));
        out.print(temperature);
        out.println(F(" C"));
        out.print(F(" Hum: "));
        out.print(humidity);
        out.println(F(" %"));
    }

    void toJson(JsonObject obj) override {
        obj["temperature"]["value"] = temperature;
        obj["temperature"]["unit"]  = "°C";

        obj["humidity"]["value"] = humidity;
        obj["humidity"]["unit"]  = "%";
    }

    String toCsv() const override {
        return String(temperature, 2) + "," + String(humidity, 2);
    }

    String csvHeader() const override {
        return "dht_temp,dht_humidity";
    }

    BinaryPacket toBinary() const override {
        BinaryPacket pkt{};
        pkt.type = 2; // DHT
        pkt.length = 4;

        int16_t t = temperature * 10;
        int16_t h = humidity * 10;

        pkt.data[0] = t >> 8;
        pkt.data[1] = t & 0xFF;
        pkt.data[2] = h >> 8;
        pkt.data[3] = h & 0xFF;

        return pkt;
    }

    const char* name() const override {
        return "dht22";
    }

    float getTemperature() const { return temperature; }
    float getHumidity() const { return humidity; }

private:
    DHT dht;
    uint32_t lastRead = 0;

    float temperature = NAN;
    float humidity = NAN;
};