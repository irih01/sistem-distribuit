#include <WiFi.h>

#include <sendToGoogle.h>
#include <MasterTransport.h>
#include <BinaryProtocol.h>
#include <OledDisplay.h>

#include "sensors/DHTSensor.h"
#include "sensors/Apds.h"
#include "sensors/SensorManager.h"
#include "Config.h"



// PINS
#define TOUCH_PIN D7
#define DHT_PIN   D5
#define SDA_PIN   A4  
#define SCL_PIN   A5  




// ~~~~~~~~~~~~~~~~~~~~~ SENSORS ~~~~~~~~~~~~~~~~~~~~~
Dht22Sensor dht(DHT_PIN);
ApdsSensor apds(&Wire);
Sensor* sensors[] = {
  &dht,
  &apds
};
SensorManager sensorManager(sensors, 2);




// ~~~~~~~~~~~~~~~~~~~~ SCREEN ~~~~~~~~~~~~~~~~~~~~~~~~
constexpr uint32_t LONG_PRESS_MS = 700;
OledDisplay oled(SCREEN_WIDTH, SCREEN_HEIGHT);



// ~~~~~~~~~~~~~~~~~~~~ BUFFERS ~~~~~~~~~~~~~~~~~~~~~~~
uint8_t rxBuffer[64];
uint32_t slavePacketsRx = 0;
NodeState node;



/* ================== PUT SLAVE DATA =============== */
void parseSlaveBinary(uint8_t* buf, uint16_t len){
    for(uint16_t i = 0; i + sizeof(Sensor::BinaryPacket) <= len; i += sizeof(Sensor::BinaryPacket)){
        auto* pkt = (Sensor::BinaryPacket*)(buf + i);
        
        node.valid = true;
        node.updated = true;

        switch (pkt->type){
            case 1: { //BMP280
                struct {float t; float p;} data;
                memcpy(&data, pkt->data, sizeof(data));
                node.bmpT = data.t;
                node.bmpP = data.p;
                oled.setSlaveData("BMP T", data.t, " C");
                oled.setSlaveData("BMP P", data.p, "hPa");   
                break;
            }
            case 2: { //AHT21
                struct {float t; float h;} data;
                memcpy(&data, pkt->data, sizeof(data));
                node.ahtT = data.t;
                node.ahtH = data.h;
                oled.setSlaveData("AHT T", data.t, " C");
                oled.setSlaveData("AHT H", data.h, "%");
                break;
            }
            case 3: { //GAS
                struct {int16_t raw; int16_t delta;} data;
                memcpy(&data, pkt->data, sizeof(data));
                node.gasRaw   = data.raw;
                node.gasDelta = data.delta;
                
                oled.setSlaveData("Gas raw", data.raw, "");
                oled.setSlaveData("Gas Δ", data.delta, "");
                break;
            }
        }
    }
}





/* =========================================================(       S   E   T   U   P      )============================================================= */
void setup() {
    Serial.begin(115200);
    while (!Serial);
    Serial.println(F("===== MASTER SETUP ====="));

    Wire.begin();

    /* -------- WiFi -------- */
    Serial.print(F("Connecting WiFi"));
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED) {
        delay(300);
        Serial.print('.');
    }
    Serial.println();
    Serial.println(F("WiFi connected"));
    Serial.print(F("MASTER IP: "));
    Serial.println(WiFi.localIP());


    /* --------- Sensors --------*/
    pinMode(TOUCH_PIN, INPUT);
    sensorManager.beginAll();


    /* -------- Server -------- */
    transport.begin(LISTEN_PORT);
    Serial.print(F("Listening on port "));
    Serial.println(LISTEN_PORT);
    

    /* ---------- Oled ----------*/
    if(!oled.begin()){
    Serial.println("Oled failed");
    //while (1);
    }

    Serial.println(F("===================="));
}




/* =========================================================(          L  O   O   P        )============================================================= */
void loop() {
    static uint32_t lastSend = 0;


    /* -----=== Update sensors ===----- */
    sensorManager.updateAll();


    /* ---------========= Display data =======-----------*/
    oled.setMasterData("Temp", dht.getTemperature(), "°C");
    oled.setMasterData("Hum", dht.getHumidity(), "%");
    oled.setMasterData("Lux",  apds.getAmbientLux(), "lx");
    oled.setMasterData("Prox", apds.getProximity(), "raw");
    
    


   
    /* --------======== Node data =========---------- */
    int len = transport.recieve(rxBuffer, sizeof(rxBuffer));
    if (len > 0){
        OledDisplay::NetInfo net;
        net.state = OledDisplay::NetState::CONNECTED;
        net.rssi = WiFi.RSSI();
        net.packetsRx = ++slavePacketsRx;
        net.lastPacketMs = millis();

        oled.setNetInfo(net);
        
        parseSlaveBinary(rxBuffer, len);
        
    }



    /* ----------------========== POST Sheets ============------------ */
    if (node.updated && millis() - lastSend >= SHEETS_POST_INTERVAL_MS) {
        sendBatch(
            dht.getTemperature(),
            dht.getHumidity(),
            apds.getAmbientLux(),
            apds.getProximity(),
            node
        );

        node.updated = false;
        lastSend = millis();
    }




    /* ---=== Buton + OLED ===--- */
    oled.handleButton(TOUCH_PIN);
    oled.update();
    
}



