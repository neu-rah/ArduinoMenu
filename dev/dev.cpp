#include "dev.h"

const char* op1_text="op1";
Prompt<Sys::asTitle<Static<Sys>::Text<&op1_text>>> op1;
Prompt<Sys::asTitle<Text<Sys>>> op2("op2");

// Prompt<
// >
Prompt<VectorMenu<Sys,Text<Sys>>>
subMenu("non-static title",&op1,&op2);

const char* sm_title="static menu";
Prompt<Static<Sys,Static<Sys>::Text<&sm_title>>::Menu<2>> sm({&op1,&op2});

//a dynamic (vector) menu with static text title
const char* mainMenu_title="Main menu";
VectorMenu<Sys,Static<Sys>::Text<&mainMenu_title>> mainMenu(
  &op1,//a static text option
  &op2,//a non-static text option
  &subMenu,//dynamic menu with a non-static text title
  &sm,//static menu with static title text
  new Prompt<Lang<Sys,Code>::Text<yesText>>,
  new Prompt<Title<noText>>,
  new Prompt<Title<okText>>,
  new Prompt<Title<cancelText>>
);

template<typename Sys>
struct Core {
  using Raw=typename Sys::RawOutput;
  using Prompt=typename Sys::IfPrompt;
  Raw& printMenu(Raw& out,Prompt& menu) {
    out<<menu<<endl;//this `endl`s will be compose on out device style instead!
    for(int n=0;n<menu.sz();n++)
      out<<menu[n]<<endl;
    return out;
  }
};

Core<Sys> rawCore;

int main(int argc, char** argv) {
  cout<<"AM5 test ------------------"<<endl;
  rawCore.printMenu(cout,mainMenu);
  Lang<Sys,Code>::sel=pt;
  rawCore.printMenu(cout,mainMenu);
  cout<<"-------"<<endl;
  return 0;
}
