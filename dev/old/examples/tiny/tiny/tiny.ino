#include <defs/tiny.h>

//tiny text menu example
//294 bytes, just text prompts, we print the menu
//and would deal to whatever input or output is needed

/*output:
AM5 test ------------------
Op1
Op2
-------
*/

const char* op1_text="Op1";
OP<&op1_text> op1;
const char* op2_text="Op2";
OP<&op2_text> op2;
MENU<2> mainMenu({&op1,&op2});

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial<<"AM5 test ------------------"<<endl;
  //and we print the menu ourselves
  for(idx_t n=0;n<mainMenu.sz();n++)
    Serial<<mainMenu[n]<<endl;
  Serial<<"-------"<<endl;
}

void loop() {
}
