#include <menu/def/tinyArduino.h>

SerialOut serialOut;
Op op("Example option");

void setup() {
  Serial.begin(115200);
  while(!Serial);
  serialOut<<op;
}

void loop() {}
