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
menu* menu::activeMenu=NULL;

//menu navigation engine
//iteract with input until a selection is done, return the selection
int menu::menuKeys(menuOut &p,Stream& c,bool canExit) {
  int op=-2;
  //do {
    //while(!c.available());
    if (!c.available()) return op;
    if (c.peek()!=13) {
      if(c.available()) {
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
      }
    } else
      op=sel==sz?-1:sel;
    if (!((op>=0&&op<sz)||(canExit&&op==-1))) op=-2;//validate the option
    //add some delays to be sure we do not have more characters NL or CR on the way
    //delay might be adjusted to vope with stream speed
    delay(50);while (c.peek()==13||c.peek()==10) {c.read();delay(50);}//discard ENTER and CR
  //} while(op==-2);//repeat until we have a valid option (can be Exit/Esc)
  return op;
}
    
//execute the menu
//cycle:
//	...->draw -> input scan -> iterations -> [activate submenu or user function] -> ...
// draw: call target device object
//input scan: call the navigation function (self)
void menu::activate(menuOut& p,Stream& c,bool canExit) {
	if (activeMenu!=this) {
		previousMenu=activeMenu;
		activeMenu=this;
		sel=0;
		p.top=0;
   	//c.flush();//reset the encoder
   	p.drawn=0;//redraw menu
	}
  int op=-1;
  //do {
  	//Serial<<"activation printing menu "<<text<<endl;
    printMenu(p,canExit);
    op=menuKeys(p,c,canExit);
    if (op>=0&&op<sz) {
    	sel=op;
      if (data[op]->enabled) {
      	/*Serial<<"activating option "<<op<<endl;
      	Serial.flush();
      	delay(10);*/
      	data[op]->activate(p,c,true);
      	//c.flush();//reset the encoder
      	//p.drawn=0;//redraw menu
      }
    } else if (op==-1) {//then exit
    	activeMenu=previousMenu;
		 	c.flush();//reset the encoder
		 	//p.drawn=0;//redraw menu
    }
  //} while(op!=-1);
}

void menu::poll(menuOut& p,Stream& c,bool canExit) {
	if (!activeMenu) activeMenu=this;
	activeMenu->activate(p,c,activeMenu==this?canExit:true);
}

//todo: preparing functions for touch screen support
// this functions will probably move to the touch device class
// however they are now here for test purposes
/*void menu::clampY(menuOut& o) {//keep menu inside screen Y
	if (o.top+o.maxY>sz) o.top=sz-o.maxY;
	else if (o.top<0) o.top=0;
}

//we scroll by lines, return remaining pixels
int menu::scrollY(menuOut& o,int pixels) {
	if (o.top==0&&sz<=o.maxY) return pixels;//menu is inside screen, no need to scroll
	int lines=pixels/o.resY;//convert pixels to lines
	o.top+=lines;
	clampY(o);
	if (sel<o.top) sel=o.top;
	else if (sel>=(o.top+o.maxY)) sel=o.top+o.maxY-1;
	//menu needs to be redrawn after this
	return pixels-lines*o.resY;
}

void menu::click(menuOut &p, Stream &c,int x,int y) {
	int row=y/p.resY;
	int col=x/p.resX;
	if (row<p.maxY&&row>=0&&col>=0&&col<p.maxX&&(p.top+row)<sz)
		data[p.top+row]->activate(p,c,true);
}


*/	
