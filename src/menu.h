/********************
Sept. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

Arduino generic menu system

    prompt: class representing a text and an associated function pointer
      menu: prompt derived holding a list of prompts (options and submenus)
   menuOut: print derived with special virtual functions needed for menu output, derive this to have the menu in other devices
 menuPrint: menuOut implementation for generic print (as Serial)
   menuLCD: menuOut implementation for standard LiquidCrystal LCD

the menu system will read provided stream for input, it works for Serial,
encoders, joysticks, keyboards (or touch?) a stream must be made out of them
www.r-site.net

v2.1 - Add full support of SetPosition(x,y) to move the menu inside the screen (char positioning) - can be extended for pixel positioning

*/
#ifndef RSITE_ARDUINO_MENU_SYSTEM
  #define RSITE_ARDUINO_MENU_SYSTEM

	#include <Stream.h>
	#include <HardwareSerial.h>

  //#define DEBUG
  #ifdef DEBUG
	#include <streamFlow.h>
  #endif

  #include <macros.h>

  /////////////////////////////////////////////////////////
  // menu pure virtual output device, use derived
  // this base class represents the output device either derived to serial, LCD or other
  class menuOut:public Print {
    public:

    menu* drawn;//last drawn menu, avoiding clear/redraw
    int top;//top for this device
    //device size (in characters)
    int maxX;
    int maxY;
    //device resolution (pixels per character)
    int resX;
    int resY;
    //preventing uneeded redraws
    int lastTop;
    int lastSel;

    inline menuOut(int x=0x7F,int y=0x7F,int resX=1,int resY=1)
    	:maxX(x),maxY(y),top(0),resX(resX),resY(resY),drawn(0),lastTop(-1),lastSel(-1) {}

    enum drawStyle {NORMAL=0,SELECTED,EDITING,TUNNING,DISABLED};

  //member functions
    inline void redraw() {drawn=NULL;}// this device will draw the menu on the next poll
    virtual size_t write(uint8_t) = 0;
    bool needRedraw(menu& m,int i);
    virtual void clearLine(int ln)=0;
    virtual void clear()=0;
    virtual void showCursor() {}
		virtual void hideCursor() {}
    void setCursor(int x) {
      //Serial<<"menuOut::setCursor "<<x<<","<<lastSel-lastTop<<endl;
      setCursor(x,lastSel-lastTop);
    }
    virtual void setCursor(int x,int y)=0;
    virtual void printPrompt(prompt &o,bool selected,int idx,int posX,int posY,int width);
		virtual void printMenu(menu&,bool drawExit)=0;
    //virtual void drawEditCursor(int at) {}//text edit cursor
  };

  ////////////////////////////////////////////////////////////////////
  // menu structure

  //an associated function to be called on menu selection
  //associated functions can accept no parameters
  // or accept some of the standard parameters preserving the order
  // standard parameters (for this menu lib) are:
  // prompt -> the associated prompt object that trigged the call
  // menuOut -> the device we were using to display the menu.. you migh want to draw on it
  // Stream -> the input stream we are using to play the menu, can be a serial or an encoder or keyboard stream


  /*template<void (*T)(prompt &p, menuOut &o, Stream &i)>
  int devoid(prompt &p, menuOut &o, Stream &i) {T(p,o,i);return 0;}

  template<>
  int (*)(prompt &p, menuOut &o, Stream &i) operator(void (*f)(prompt &p, menuOut &o, Stream &i)) {return devoid<f>;}*/

  #define promptFeedback bool

	class promptAction {
	public:
    typedef promptFeedback (*callback)(prompt &p, menuOut &o, Stream &i);//callback fynction type
		callback hFn;//the hooked callback function

		//cast no arguments or partial arguments to be accepted as promptActions
		inline promptAction() {}
    inline promptAction(promptFeedback (*f)()):hFn((callback)f) {}
		inline promptAction(promptFeedback (*f)(prompt&)):hFn((callback)f) {}
		inline promptAction(promptFeedback (*f)(prompt&,menuOut&)):hFn((callback)f) {}
		inline promptAction(callback f):hFn(f) {}
		//use this objects as a function (replacing functions)
		inline promptFeedback operator()(prompt &p, menuOut &o, Stream &i) const {return hFn(p,o,i);}
	};

  void print_P(menuOut& s,const char* at);

	//holds a menu option
	//a menu is also a prompt so we can have sub-menus
  class prompt {
    public:
    const char* text;// MEMMODE; memmode is defined by the source, here its only a pointer
    promptAction action;
    static promptFeedback nothing() {return false;}
    bool enabled;
    inline prompt(const char * text):text(text),enabled(true),action(nothing) {}
    inline prompt(const char * text,promptAction action)
    	:text(text),action(action),enabled(true) {}
    inline void enable() {enabled=true;}
    inline void disable() {enabled=false;}
    virtual void printName(menuOut& p) {print_P(p,text);}
    virtual void printValue(menuOut& p) {}
    virtual void printUnit(menuOut& p) {}
    virtual void printTo(menuOut& p) {
        printName(p);
        printValue(p);
        printUnit(p);
    }
    virtual bool needRedraw(menuOut&,bool) {return false;}
    virtual promptFeedback activate(menuOut& p,Stream&c,bool) {
      return action(*this,p,c);
    }
    virtual bool isMenu() const {return false;}
  };

  class menuNode:public prompt {//some basic information for menus and fields
  	public:
    int width;//field or menu width
    int ox,oy;//coordinate origin displacement
    //navigation and focus control
    static menuNode* activeNode;
    class menu* previousMenu;
    inline menuNode(const char * text):prompt(text),ox(0),oy(0),width(32),previousMenu(NULL) {}
    inline menuNode(const char * text,promptAction action):prompt(text,action),ox(0),oy(0),width(32) {}
  };

  //a menu or sub-menu
  class menu:public menuNode {
    public:
    static char escCode;
    static char enterCode;
    static char upCode;
    static char downCode;
    static const char exit[] MEMMODE;//text used for exit option
    static char enabledCursor;//character to be used as navigation cursor
    static char disabledCursor;//to be used when navigating over disabled options
    static prompt exitOption;//option to append to menu allowing exit when no escape button/key is available
    static bool wrapMenus;//loop menu ends
    enum feedback {//
      cont=0,
      quit
    };
    const int sz;
    signed int sel;//selection
    prompt* const* data;
    bool canExit;//store last canExit value for inner reference
    menu(const char * text,int sz,prompt* const data[]):menuNode(text),sz(sz),data(data),canExit(false) {}

    inline prompt& operator[](int i) {return *(prompt*)pgmPtrNear(data[i]);}
    inline void setPosition(int x,int y) {ox=x;oy=y;}
    int menuKeys(menuOut &p,Stream& c,bool drawExit);
    inline void printMenu(menuOut& p,bool drawExit=false) {
    	p.printMenu(*this,drawExit);
    }
    //virtual bool needRedraw(menuOut& o,bool) {return o.drawn!=true;}
    //force menu redraw on selected device
    inline void redraw(menuOut& p,Stream& c,bool canExit=false) {p.drawn=NULL;poll(p,c,canExit);}
    //set the focus to menu->Option idx, the focused menu will exit to the current
    /*inline void focus(int idx,menu *prev=NULL) {
      previousMenu=prev;
      activeNode=this;//redirect drawing
      sel=(idx>=sz||idx<0)?sz-1:idx;//focus idx option if out of range then the last will be selected
    }*/

    promptFeedback activate(menuOut& p,Stream& c,bool canExit=false);

    void poll(menuOut& p,Stream& c,bool canExit=false);

    virtual bool isMenu() const {return true;}
  };

  //PROGMEM AUX PRINT
  void print_P(menuOut& s,const char* at);
#endif
