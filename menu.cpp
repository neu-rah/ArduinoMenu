/********************
Arduino generic menu system
Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
*/
#include "menu.h"

const char* menu::exit="Exit";
char menu::enabledCursor='>';
char menu::disabledCursor='-';
prompt menu::exitOption(menu::exit);

int menu::menuKeys(menuOut &p,Stream& c,bool canExit) {
  int op=-2;
  do {
    while(!c.available());// delay(20);
    if (c.peek()!=13) {
      if(c.available()) {
        int ch=c.read();
        if (ch=='-') {
          if (sel>0) {
            sel--;
            if (sel+1>=p.maxY) p.top=sel-p.maxY;
            printMenu(p,canExit);
          }
        } else if (ch=='+') {
          if (sel<(sz-(canExit?0:1))) {
            sel++;
            if ((sz-sel+(canExit?1:0))>=p.maxY) p.top=sel-(canExit?1:0);
            printMenu(p,canExit);
          }
        } else if (ch==27) {
        	op=-1;
        } else op=ch-49;
        if (c.peek()==13||c.peek()==10) c.read();//discard ENTER and CR
      }
    } else {
      c.read();
      op=sel==sz?-1:sel;
    }
  } while(canExit&&(op<-1||op>=sz));
  return op;
}
    
void menu::activate(menuOut& p,Stream& c,bool canExit) {
  sel=0;
  p.top=0;
  int op=-1;
  do {
    printMenu(p,canExit);
		c.flush();//reset the encoder
		while(c.available()) c.read();//clean the stream
    op=menuKeys(p,c,canExit);
    if (op>=0&&op<sz) {
    	sel=op;
      if (data[op]->enabled)
      	data[op]->activate(p,c,true);
      	p.drawn=0;//redraw menu
    }
  } while(op!=-1);
}

