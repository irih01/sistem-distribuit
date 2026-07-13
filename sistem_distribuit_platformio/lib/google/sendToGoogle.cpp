#include "sendToGoogle.h"
#include <HTTPClient.h>
#include "../../include/Config.h"




bool sendToGoogleSheets(
  const char* node,
  const char* sensor,
  const char* metric,
  float value,
  const char* unit
){
  if (isnan(value) || !isfinite(value)) return false;

  char payload[256];

  // EXACT ca curl
  snprintf(
    payload, sizeof(payload),
    "{\"rows\":[[\"%s\",\"%s\",\"%s\",%.1f,\"%s\"]]}",
    node, sensor, metric, value, unit
  );
 
  

  HTTPClient http;
  http.begin(SHEETS_URL);
  
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(5000);
  http.setReuse(false);
  Serial.println(payload);
  int code = http.POST((uint8_t*)payload, strlen(payload));

  http.end();

  Serial.printf("POST code=%d\n", code);
  return code == 200;
}


bool sendBatch(
  float dhtT, float dhtH,
  float lux, float prox,
  const NodeState& s
){
  if (!s.valid) return false;
  char payload[768];

  int len = snprintf(payload, sizeof(payload),
    "{\"rows\":["
      "[\"master\",\"dht22\",\"temperature\",%.1f,\"°C\"],"
      "[\"master\",\"dht22\",\"humidity\",%.1f,\"%%\"],"
      "[\"master\",\"apds\",\"lux\",%.1f,\"lx\"],"
      "[\"master\",\"apds\",\"proximity\",%.1f,\"raw\"],"

      "[\"node1\",\"bmp280\",\"temperature\",%.1f,\"°C\"],"
      "[\"node1\",\"bmp280\",\"pressure\",%.1f,\"hPa\"],"

      "[\"node1\",\"aht21\",\"temperature\",%.1f,\"°C\"],"
      "[\"node1\",\"aht21\",\"humidity\",%.1f,\"%%\"],"

      "[\"node1\",\"gas\",\"raw\",%d,\"\"],"
      "[\"node1\",\"gas\",\"delta\",%d,\"\" ]"
    "]}",
    dhtT, dhtH, lux, prox,
    s.bmpT, s.bmpP,
    s.ahtT, s.ahtH,
    s.gasRaw, s.gasDelta
  );

  

  HTTPClient http;
  http.begin(SHEETS_URL);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(4000);
  http.setReuse(false);

  int code = http.POST((uint8_t*)payload, len);
  http.end();
  Serial.printf("Sheets POST = %d\n", code);
  return code == 200;
  
}