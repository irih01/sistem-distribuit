#include "MasterTransport.h"
#include "Endian.h"

bool MasterTransport::begin(uint16_t port){
    server = WiFiServer(port);
    server.begin();
    return true;
}

int MasterTransport::recieve(uint8_t* buffer, uint16_t maxLen){
    WiFiClient client = server.available();
    if(!client) return 0;

    // Asteapta lenght
    while (client.available() < 2) delay(1);
    
    uint16_t netLen;
    client.read((uint8_t*)&netLen, sizeof(netLen));
    uint16_t len = fromBigEndian16(netLen);

    if(len > maxLen){
        client.stop();
        return -1;
    }

    uint16_t recieved = 0;
    while (recieved < len){
        if(client.available()){
            recieved += client.read(buffer + recieved, len - recieved);
        }
    }

    client.stop();
    return len; 
}