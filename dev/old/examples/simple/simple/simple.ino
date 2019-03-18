#include <streamFlow.h>
#include <menu.h>
using serialOut=decltype(Serial);

using Sys=SysDef<serialOut,Nil,uint8_t>;
// #include <defs/simple.h>

//simple text menu example
//Rui Azevedo - Mar2019
//ruihfazevedo@gmail.com

/*output:
AM5 test ------------------
Op1
Op2
-------
*/

// MENU<2> mainMenu("Main menu",{
//   new OP("Op1"),
//   new OP("Op2"),
// });

//using components, usually we have a restricter set, but this is a demo...
template<typename O> using Prompt=Sys::Prompt<O>;
using Static=Sys::Static<>;

//a multilanguage title
template<size_t id>
using Title=Sys::asTitle<Sys::Text<>>;

const char* op1_text="op1";
Prompt<Sys::asTitle<Static::Text<&op1_text>>> op1;
Prompt<Sys::asTitle<Sys::Text<>>> op2("op2");

Sys::Prompt<Sys::Static<Sys::Text<>>::Menu<2>>
subMenu("title",&op1,&op2);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial<<"AM5 test ------------------"<<endl;
  //and we print the menu ourselves
  // for(idx_t n=0;n<mainMenu.sz();n++)
  //   Serial<<mainMenu[n]<<endl;
  Serial<<"-------"<<endl;
}

void loop() {
}
