#include <streamFlow.h>

#include <menu.h>
#include <items.h>
#include <menuIO/serialOut.h>

using SerialOut=MenuOutCap<WrapTitle<SerialOutDef>>;

SerialOut serialOut;

Prompt<asTitle<Text<Empty>>> op("Ok");

void setup() {
  Serial.begin(115200);
  while(!Serial);
  serialOut<<op;
}

void loop() {}
