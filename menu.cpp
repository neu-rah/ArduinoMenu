/********************
Sept. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no 
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extendable: Yes

Arduino generic menu system
*/
#include <Arduino.h>
#include "menu.h"

const char* menu::exit="Exit";
char menu::enabledCursor='>';
char menu::disabledCursor='-';
prompt menu::exitOption(menu::exit);
menuNode* menuNode::activeNode=NULL;

//menu navigation engine
//iteract with input until a selection is done, return the selection
int menu::menuKeys(menuOut &p,Stream& c,bool canExit) {
  int op=-2;
  if (!c.available()) return op;
  if (c.peek()!=13) {
    //if(c.available()) {
      int ch=c.read();
      if (ch=='-') {
        if (sel>0) {
          sel--;
          if (sel+1>=p.maxY) p.top=sel-p.maxY;
          p.drawn=0;
          //printMenu(p,canExit);
        }
      } else if (ch=='+') {
        if (sel<(sz-(canExit?0:1))) {
          sel++;
          if ((sz-sel+(canExit?1:0))>=p.maxY) p.top=sel-(canExit?1:0);
          p.drawn=0;
          //printMenu(p,canExit);
        }
      } else if (ch==27) {
      	op=-1;
      } else op=ch-49;
    //}
  } else
    op=sel==sz?-1:sel;
  if (!((op>=0&&op<sz)||(canExit&&op==-1))) op=-2;//validate the option
  //add some delays to be sure we do not have more characters NL or CR on the way
  //delay might be adjusted to cope with stream speed
  delay(50);while (c.peek()==13||c.peek()==10) {c.read();delay(50);}//discard ENTER and CR
  return op;
}
    
//execute the menu
//cycle:
//	...->draw -> input scan -> iterations -> [activate submenu or user function] -> ...
// draw: call target device object
//input scan: call the navigation function (self)
void menu::activate(menuOut& p,Stream& c,bool canExit) {
	if (activeNode!=this) {
		previousMenu=(menu*)activeNode;
		activeNode=this;
		sel=0;
		p.top=0;
   	p.drawn=0;//redraw menu
   	this->canExit=canExit;
	}
  int op=-1;
  printMenu(p,canExit);
  op=menuKeys(p,c,canExit);
  if (op>=0&&op<sz) {
  	sel=op;
    if (data[op]->enabled) {
    	data[op]->activate(p,c,true);
    }
  } else if (op==-1) {//then exit
  	activeNode=previousMenu;
	 	c.flush();//reset the encoder
  }
}

void menu::poll(menuOut& p,Stream& c,bool canExit) {
	if (!activeNode) activeNode=this;
	activeNode->activate(p,c,activeNode==this?canExit:true);
}

