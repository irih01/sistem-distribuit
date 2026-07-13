#include <WiFiS3.h>
#include <ArduinoJson.h>
#include <NodeTransport.h>


#include "sensors/Bmp280.h"
#include "sensors/Aht21.h"
#include "sensors/SensorManager.h"
#include "sensors/GasAdc.h"
#include "Config.h"





/* ================================== Sensors ======================*/

Bmp280Sensor bmp(&Wire1, 0x77);
Aht21Sensor aht(&Wire1);
GasAdc gas(0x48, &Wire1);


Sensor* sensors[] = {
  &bmp,
  &aht,
  &gas
};
SensorManager manager(sensors, 3);

/* ============================== Buffers ===============================*/

StaticJsonDocument<512> doc;
uint8_t binaryBuffer[3 * sizeof(Sensor::BinaryPacket)]; // 3 senzori x 16 bytes
NodeTransport tp;




void setup() {
  Serial.begin(115200);
  Serial.println(F("=== NODE START ==="));
  Wire1.begin();
  
  /* ------------- WiFi -------------*/
  Serial.print(F("Connecting WiFi"));
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  while (WiFi.status() != WL_CONNECTED){
    delay(300);
    Serial.print('.');
  }
  
  Serial.println();
  Serial.print(F("IP: "));
  Serial.println(WiFi.localIP());
  
  
  /* ----------- Sensors ------------ */
  manager.beginAll();
  Serial.println(F("Sensors initialized"));

  /* ------------- Transport --------------*/
  tp.begin();
  Serial.println(F("Transport ready"));

  Serial.println(F("==================================\n"));
}



void loop() {
  //--------- Update sensors -------------
  manager.updateAll();
  manager.printAll(Serial);


  delay(1000);


  // ------- JSON (debug) ----------------
  doc.clear();
  manager.toJson(doc);

  serializeJsonPretty(doc, Serial);
  Serial.println(F("\n"));

 // --------- Binary ----------------------
  manager.toBinary(binaryBuffer);
 

  Serial.println(F(" BINARY BUFFER DUMP"));
  for(size_t i = 0; i < sizeof(binaryBuffer); i ++){
    if(binaryBuffer[i] < 16) Serial.print('0');
    Serial.print(binaryBuffer[i], HEX);
    Serial.print(' ');
    if ((i + 1) % 16 == 0) Serial.println();
  }
  Serial.println(F("=============================="));
  

   /*
   type  length  data[] payload
    01     08      66 66 C8 41    E3 98 81 44   00 00 00 00 00 00
    BMP    8bytes    25.05 *C      1036.2 hPa

            little endian -> big endian
                   41 C8 66 66      
            IEEE754 0x41C86666 = 25.05
  
    02     08      F0 D7 C3 41    DE 4B 46 42   00 00 00 00 00 00
   AHT21   8bytes    24.48 *C        49.57 %

    03     04        D8 34        A0 03    00 00 00 00   00 00 00 00 00 00
   GAS     4bytes    13528         928
  */

  // ------------- Trimitere date --------------------
 

  bool ok = tp.send(binaryBuffer, sizeof(binaryBuffer));
  Serial.print(F("Trimitere: "));
  Serial.println(ok ? F("OK") : F("FAILED"));


  Serial.println(F("======================================\n"));
  delay(9000);
}
