#include <menu/def/tinyArduino.h>

using FlashText=FlashTextDef<Empty>;

//normal option
SerialOut serialOut;
Prompt<Op> op1("Op 1");

//option using flash text
const char op2_text[] PROGMEM="Op 2";
Prompt<FlashOp> op2(op2_text);

//they can fit on same array
//and will preserve the composed behavior
Item* ops[]{&op1,&op2};

void setup() {
  Serial.begin(115200);
  while(!Serial);
  serialOut<<"AM5 tiny example ----"<<endl;
  //and we print them, just.
  for(auto o: ops) serialOut<<*o<<endl;
  serialOut<<"----"<<endl;
}

void loop() {}
