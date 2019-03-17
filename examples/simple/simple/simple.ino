#include <defs/simple.h>

//simple text menu example
//Rui Azevedo - Mar2019
//ruihfazevedo@gmail.com

/*output:
AM5 test ------------------
Op1
Op2
-------
*/

MENU<2> mainMenu("Main menu",{
  new OP("Op1"),
  new OP("Op2"),
});

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
