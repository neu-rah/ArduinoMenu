/********************

THIS IS EXPERIMENTAL!

www.r-site.net
Nov. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
implementing menu fields as options that show a value
value (variable reference) can be changed by either using:
	menuField - for numeric varibles between range and optinally with a tune speed
	menuChoose - Use menu like navigation to select variable value
	menuToggle - cycle list of possible values

class menuValue is used as a menu prompt with an associated value for menuChoose and menuToggle

this classes are implemented as templates to accomodate virtually any value type

creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

***/

#ifndef RSITE_ARDUINOP_MENU_TEXTFIELDS
#define RSITE_ARDUINOP_MENU_TEXTFIELDS

#include "menuFields.h"

//char * caps=" ABCDEFGHIJKLMNOPQRSTUVWXYZ";
//char *alphaNum=" 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

class menuTextField:public menuNode {
public:
	unsigned int cursor;
	char *target;
	char len;
	const char* allowed;
  menuTextField(
    char target[],
    char *prompt,
		promptAction func,
		const char* allowed=numericChars
  ):menuNode(prompt,func),cursor(0),target(target),allowed(allowed) {len=strlen(target);}
	virtual promptFeedback activate(menuOut& p,Stream&c,bool canExit=false) {
		if (activeNode!=this) {
		  ox=activeNode->ox;
		  oy=activeNode->oy;
			previousMenu=(menu*)activeNode;
			activeNode=this;
    	p.lastSel=-1;
			cursor=0;
    	previousMenu->printMenu(p,previousMenu->canExit);
			p.showCursor();
			p.setCursor(strlen(text)+cursor+2);
		}
		if (!c.available()) return;
		char ch=c.read();
		if (ch==menu::escCode||cursor>=len) {
			p.clear();
			p.hideCursor();
			activeNode=previousMenu;
			c.flush();
		} else if (ch==menu::enterCode) {
			cursor++;
		} else {
			if (ch==menu::upCode||ch==menu::downCode) {
				char *at=strchr(allowed,target[cursor]);
				int idx=at?at-allowed:0;
				if (ch==menu::upCode) idx++;
				else if (ch==menu::downCode) idx--;
				if (idx<0) idx=strlen(allowed);
				if (idx>=strlen(allowed)) idx=0;
				target[cursor]=allowed[idx];
			} else if (strchr(allowed,ch)) target[cursor++]=ch;
		}
		//p.drawEditCursor(strlen(text)+cursor+1);
    //clamp();
    //if (value!=tmp||ch==menu::enterCode) {
    	prompt::activate(p,c,canExit);//call update functions
    	p.lastSel=-1;
    	previousMenu->printMenu(p,previousMenu->canExit);
    //}
		p.setCursor(strlen(text)+cursor+2);
		return false;
	}
	virtual void printTo(menuOut& p) {
		p.print(prompt::text);
		p.print(activeNode==this?":":" ");
		p.print(target);
	}
};


#endif RSITE_ARDUINOP_MENU_TEXTFIELDS
