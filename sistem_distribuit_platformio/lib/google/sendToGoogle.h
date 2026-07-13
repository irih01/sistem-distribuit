#pragma once
#include <math.h>




struct NodeState {
  bool valid = false;
  bool updated = false;

  float bmpT = NAN;
  float bmpP = NAN;

  float ahtT = NAN;
  float ahtH = NAN;

  int16_t gasRaw = 0;
  int16_t gasDelta = 0;

};


bool sendToGoogleSheets(
  const char* node,
  const char* sensor,
  const char* metric,
  float value,
  const char* unit
);

bool sendBatch(
  float dhtT, float dhtH,
  float lux, float prox,
  const NodeState& node
);