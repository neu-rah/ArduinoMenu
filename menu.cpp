/********************
Sept. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

Arduino generic menu system
www.r-site.net
*/
#include <Arduino.h>
#include "menu.h"

const char* menu::exit="Exit";
char menu::escCode='/';
char menu::enterCode='*';
char menu::upCode='+';
char menu::downCode='-';
char menu::enabledCursor='>';
char menu::disabledCursor='-';
prompt menu::exitOption(menu::exit);
menuNode* menuNode::activeNode=NULL;

bool menuOut::needRedraw(menu& m,int i) {return (drawn!=&m)||(top!=lastTop)||(m.sel!=lastSel&&((i==m.sel)||(i==lastSel)));}

//menu navigation engine
//iteract with input until a selection is done, return the selection
int menu::menuKeys(menuOut &p,Stream& c,bool canExit) {
  int op=-2;
  if (!c.available()) return op;//only work when stream data is available
  int ch=c.read();
  if (ch!=menu::enterCode) {
    if (ch==menu::downCode) {
      if (sel>0) {
        sel--;
        if (sel+1>=p.maxY) p.top=sel-p.maxY;
      }
    } else if (ch==menu::upCode) {
      if (sel<(sz-(canExit?0:1))) {
        sel++;
        if ((sz-sel+(canExit?1:0))>=p.maxY) p.top=sel-(canExit?1:0);
      }
    } else if (ch==menu::escCode) {
    	op=-1;
    } else
      op=ch-'1';
  } else {
    op=sel==sz?-1:sel;
  }
  if (!((op>=0&&op<sz)||(canExit&&op==-1))) op=-2;//validate the option
  //c.read();
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
   	this->canExit=canExit;
	}
  int op=-1;
  printMenu(p,canExit);
  op=menuKeys(p,c,canExit);
  if (op>=0&&op<sz) {
  	sel=op;
    if (data[op]->enabled) {
      printMenu(p,canExit);//clearing old selection
    	data[op]->activate(p,c,true);
    }
  } else if (op==-1) {//then exit
    p.clear();
  	activeNode=previousMenu;
	 	c.flush();//reset the encoder
  }
}

void menu::poll(menuOut& p,Stream& c,bool canExit) {
  (activeNode?activeNode:this)->activate(p,c,activeNode==this?canExit:true);
}
