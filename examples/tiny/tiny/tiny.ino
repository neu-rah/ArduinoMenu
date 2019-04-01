#include <menu/def/tinyArduino.h>

//serial output
MenuOut<SerialOut> serialOut;

//normal option
Prompt<Op> op1("Op 1");

//option using flash text
const char op2_text[] PROGMEM="Op 2";
Prompt<FlashOp> op2(op2_text);

//they can fit on same array
//and will preserve the composed behavior
Item* ops[]{&op1,&op2};

Prompt<StaticMenu<2>> mainMenu("Main menu",&op1,&op2);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  serialOut<<"AM5 tiny example ----"<<endl;
  serialOut<<mainMenu<<endl;//prints as defined text (asTitle<Text>)
  //and we just print the options.
  for(auto o: ops) serialOut<<*o<<endl;
  serialOut<<"----"<<endl;
}

void loop() {}
