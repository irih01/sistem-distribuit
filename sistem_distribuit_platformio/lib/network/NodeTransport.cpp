#include "NodeTransport.h"
#include "Endian.h"

#ifdef ARDUINO_ARCH_ESP32
    #include <WiFi.h>
#else
    #include <WiFiS3.h>
#endif


static IPAddress MASTER_IP(YOUR_DEVICE_IP);
static const uint16_t MASTER_PORT = 5000;

bool NodeTransport::begin(){
    return true;
}

bool NodeTransport::send(const uint8_t* data, uint16_t len){
    if (WiFi.status() != WL_CONNECTED) return false;

    WiFiClient client;
    if(!client.connect(MASTER_IP, MASTER_PORT)) return false;

    uint16_t netLen = toBigEndian16(len);
    client.write((uint8_t*)&netLen, sizeof(netLen));
    client.write(data, len);
    
    client.stop();
    return true;
}