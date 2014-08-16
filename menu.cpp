#include "menu.h"

int menu::menuKeys(menuOut &p,Stream& c) {
  int op=-2;
  do {
    while(!c.available()); delay(20);
    if (c.peek()!=13) {
      if(c.available()) {
        int ch=c.read();
        if (ch=='-') {
          if (sel>0) {
            sel--;
            if (sel+1>=p.maxY) p.top=sel-p.maxY;
            printMenu(p);
          }
        }
        else if (ch=='+') {
          if (sel<sz) {
            sel++;
            if ((sz-sel+1)>=p.maxY) p.top=sel;
            printMenu(p);
          }
        }
        else op=ch-49;
        if (c.peek()==13) c.read();//discard serial enter at eol
      }
    } else {
      c.read();
      op=sel==sz?-1:sel;
    }
  } while(op<-1||op>=sz);
  return op;
}
    
void menu::printMenu(menuOut& p) {
  p.clear();
  if (p.style==menuOut::enumerated) {p.print(text); p.print("==========================");}
  if (sel-p.top>=p.maxY) p.top=sel-p.maxY+1;//selected option outside device (bottom)
  else if (sel<p.top) p.top=sel;//selected option outside device (top)
  int i=0;for(;i<sz;i++) {
    if ((i>=p.top)&&((i-p.top)<p.maxY)) {
      p.setCursor(0,i-p.top);
      if(i-p.top>=p.maxY) break;
      if (p.style==menuOut::enumerated) {
        p.print(i<10?" ":"");
        p.print(i+1);
      }
      p.print((i==sel)?">":" ");
      p.print(data[i]->text);
    }
  }
  if (i-p.top<p.maxY) {
    p.setCursor(0,i);
    if (p.style==menuOut::enumerated) 
      p.print(" 0");
    p.print(sel==sz?">":" ");
    p.print("Exit");
  }
}
  
void menu::activate(menuOut& p,Stream& c) {
  sel=0;
  p.top=0;
  int op=-1;
  do {
    printMenu(p);
    op=menuKeys(p,c);
    if (op>=0&&op<sz)
       data[op]->activate(p,c);
  } while(op!=-1);
}

