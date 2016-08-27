/********************
Sept. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

Arduino generic menu system
www.r-site.net

v2.0 mods - calling action on every element

*/
#include <Arduino.h>
#include "menu.h"

//#define DEBUG
#ifdef DEBUG
#include <streamFlow.h>
#include <menuPrint.h>
menuPrint debugOut(Serial);
#endif

const char menu::exit[] MEMMODE="Exit";
char menu::escCode='/';
char menu::enterCode='*';
char menu::upCode='+';
char menu::downCode='-';
char menu::enabledCursor='>';
char menu::disabledCursor='-';
prompt menu::exitOption(menu::exit);
bool menu::wrapMenus=false;
menuNode* menuNode::activeNode=NULL;

//PROGMEM AUX PRINT
void print_P(menuOut& s,const char* at) {
  while(uint8_t ch=pgmByteNear(at++)) s.write(ch);
}

bool menuOut::needRedraw(menu& m,int i) {
  return
    (drawn!=&m)//menu changed
    ||(top!=lastTop)//screen scrolled
    ||(m.sel!=lastSel&&((i==m.sel)||(i==lastSel)))//selection changed
    ||(i<m.sz&&((prompt*)pgmPtrNear(m.data[i]))->needRedraw(*this,i==m.sel));//reflexivity, value changed
  }

//menu navigation engine
//iteract with input until a selection is done, return the selection
int menu::menuKeys(menuOut &p,Stream& c,bool canExit) {
  int op=-2;
  if (!c.available()) return op;//only work when stream data is available
  int ch=c.read();
  if (ch!=menu::enterCode) {
    if (ch==menu::downCode) {
      sel--;
      if (sel<0) {
        if (wrapMenus) {
          sel=sz-(canExit?0:1);
          if (sel+1>=p.maxY) {
            p.top=sel-p.maxY+1;
            //Serial<<"menuKeys top1:"<<p.top<<endl;
          }
        } else sel=0;
      }
      if (p.top>sel) {
        p.top=sel;
        //Serial<<"menuKeys top2:"<<p.top<<endl;
      }
    } else if (ch==menu::upCode) {
      sel++;
      if (sel>(sz-(canExit?0:1))) {
        if (wrapMenus) {
          sel=0;
          p.top=0;
        } else sel=sz-(canExit?0:1);
      }
      if (sel+1-p.top>p.maxY) {
        p.top=sel-p.maxY+1;
        //Serial<<"menuKeys top3:"<<p.top<<endl;
      }
    } else if (ch==menu::escCode) {
    	op=-1;
    } else {
      op=ch-'1';
    }
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
promptFeedback menu::activate(menuOut& p,Stream& c,bool canExit) {
	if (activeNode!=this) {
    //Serial<<"first time activation, canExit param:"<<canExit<<endl;
    if (action(*this,p,c)) return true;
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
    prompt* cp=(prompt*)pgmPtrNear(data[op]);
    if (cp->enabled) {
      printMenu(p,canExit);//clearing old selection
      if (cp->activate(p,c,true)) {
        p.clear();
      	activeNode=previousMenu;
        //print_P(debugOut,this->text);
        //Serial<<"->";
        //print_P(debugOut,this->operator[](sel).text);
        //Serial<<" quiting... "<<p.lastSel<<endl;
        return true;
      }
    }
  } else if (op==-1) {//then exit
    p.clear();
  	activeNode=previousMenu;
	 	c.flush();//reset the encoder
  }
  return 0;
}

void menu::poll(menuOut& p,Stream& c,bool canExit) {
  menuNode* n=activeNode?activeNode:this;
  n->activate(p,c,(activeNode==this||!activeNode)?canExit:true);
}
