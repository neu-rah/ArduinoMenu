#include "ansiSerialOut.h"
#include <AnsiStream.h>

using namespace Menu;

void ansiSerialOut::printMenu(navNode &nav) {
  if (nav.target->changed(nav,*this)) {

    *this<<"["<<*(prompt*)nav.target<<"]"<<endl;
    for(idx_t i=0;i<maxY;i++) {
      if (i>=nav.sz()) break;
      prompt *p=(prompt*)memPtr(nav.data()[i]);
      *this<<"["<<i+1<<"]";
      write(i==nav.sel?options.selectedCursor:' ');
      p->printTo(i,nav,*this);
      *this<<endl;
      p->dirty=false;
    }
    nav.target->dirty=false;
    for(int n=memStrLen(nav.target->shadow->text)+2;n;n--) *this<<"-";
    *this<<endl;
  }
}
