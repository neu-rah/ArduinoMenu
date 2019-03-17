#include "defs/dev.h"

//(imHw->inHwDrv)->inDrv<inHwDrv>->in->nav[menu]->out<OutHwDrv>->(outHwDrv->outHw)
//
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
