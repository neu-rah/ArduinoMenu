#include <menu/def/tinyArduino.h>
using namespace Menu;

//normal option
SerialOut serialOut;
Op op1("Op 1");

//option using flash text
const char op2_text[] PROGMEM="Op 2";
FlashOp op2(op2_text);

//they can fit on same array
//and will preserve the composed behavior
Item* ops[]{&op1,&op2};

void setup() {
  Serial.begin(115200);
  while(!Serial);
  for(int n=0;n<sizeof(ops)/sizeof(Item*);n++) {
    serialOut<<*ops[n];
    Serial<<endl;
  }
}

void loop() {}
