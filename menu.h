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
*/
#ifndef RSITE_ARDUINO_MENU_SYSTEM
  #define RSITE_ARDUINO_MENU_SYSTEM

	#include <Stream.h>
	#include <HardwareSerial.h>
	//#include <Flash.h>

	//#include <streamFlow.h>

  class prompt;
  class menu;
  class menuOut;
  template <typename T> class menuField;

  #define CONCATENATE(arg1, arg2)   CONCATENATE1(arg1, arg2)
  #define CONCATENATE1(arg1, arg2)  CONCATENATE2(arg1, arg2)
  #define CONCATENATE2(arg1, arg2)  arg1##arg2

  #define FOR_EACH_1(what, x, ...) what(x)
  #define FOR_EACH_2(what, x, ...)\
    what(x)\
    FOR_EACH_1(what,  __VA_ARGS__)
  #define FOR_EACH_3(what, x, ...)\
    what(x)\
    FOR_EACH_2(what, __VA_ARGS__)
  #define FOR_EACH_4(what, x, ...)\
    what(x)\
    FOR_EACH_3(what,  __VA_ARGS__)
  #define FOR_EACH_5(what, x, ...)\
    what(x)\
   FOR_EACH_4(what,  __VA_ARGS__)
  #define FOR_EACH_6(what, x, ...)\
    what(x)\
    FOR_EACH_5(what,  __VA_ARGS__)
  #define FOR_EACH_7(what, x, ...)\
    what(x)\
    FOR_EACH_6(what,  __VA_ARGS__)
  #define FOR_EACH_8(what, x, ...)\
    what(x)\
    FOR_EACH_7(what,  __VA_ARGS__)
  #define FOR_EACH_9(what, x, ...)\
    what(x)\
    FOR_EACH_8(what,  __VA_ARGS__)
  #define FOR_EACH_10(what, x, ...)\
    what(x)\
    FOR_EACH_9(what,  __VA_ARGS__)
  #define FOR_EACH_11(what, x, ...)\
    what(x)\
    FOR_EACH_10(what,  __VA_ARGS__)
  #define FOR_EACH_12(what, x, ...)\
    what(x)\
    FOR_EACH_11(what,  __VA_ARGS__)
  #define FOR_EACH_13(what, x, ...)\
    what(x)\
    FOR_EACH_12(what,  __VA_ARGS__)
  #define FOR_EACH_14(what, x, ...)\
    what(x)\
    FOR_EACH_13(what,  __VA_ARGS__)
  #define FOR_EACH_15(what, x, ...)\
    what(x)\
    FOR_EACH_14(what,  __VA_ARGS__)
  #define FOR_EACH_16(what, x, ...)\
    what(x)\
    FOR_EACH_15(what,  __VA_ARGS__)

  #define FOR_EACH_NARG(...) FOR_EACH_NARG_(__VA_ARGS__, FOR_EACH_RSEQ_N())
  #define FOR_EACH_NARG_(...) FOR_EACH_ARG_N(__VA_ARGS__)
  #define FOR_EACH_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, N, ...) N
  #define FOR_EACH_RSEQ_N() 16,15,14,13,12,11,10,9,8, 7, 6, 5, 4, 3, 2, 1, 0

  #define FOR_EACH_(N, what, x, ...) CONCATENATE(FOR_EACH_, N)(what, x, __VA_ARGS__)
  #define FOR_EACH(what, x, ...) FOR_EACH_(FOR_EACH_NARG(x, __VA_ARGS__), what, x, __VA_ARGS__)

  #define DECL(x) DECL_##x
  #define DEF(x) DEF_##x,

  #define MENU(id,text,...)\
    FOR_EACH(DECL,__VA_ARGS__)\
    prompt* const id##_data[]={\
      FOR_EACH(DEF,__VA_ARGS__)\
    };\
    menu id (text,sizeof(id##_data)/sizeof(prompt*),id##_data);

  #define CHOOSE(target,id,text,...)\
    FOR_EACH(menuValue<typeof(target)> DECL_VALUE,__VA_ARGS__)\
    menuValue<typeof(target)>* const id##_data[]={\
      FOR_EACH(DEF,__VA_ARGS__)\
    };\
    menuChoice<typeof(target)> id (text,sizeof(id##_data)/sizeof(prompt*),id##_data,target);

  #define TOGGLE(target,id,text,...)\
    FOR_EACH(menuValue<typeof(target)> DECL_VALUE,__VA_ARGS__)\
    menuValue<typeof(target)>* const id##_data[]={\
      FOR_EACH(DEF,__VA_ARGS__)\
    };\
    menuToggle<typeof(target)> id (text,sizeof(id##_data)/sizeof(prompt*),id##_data,target);

	/*#define GET_MACRO(_1,_2,NAME,...) NAME
	#define VALUE(...) GET_MACRO(__VA_ARGS__, EXPLICIT_VALUE, IMPLICIT_VALUE)(__VA_ARGS__)*/

  #define OP(...) OP_(__COUNTER__,__VA_ARGS__)
  #define FIELD(...) FIELD_(__COUNTER__,__VA_ARGS__)
  #define VALUE(...) VALUE_(__COUNTER__,__VA_ARGS__)
  #define TEXTFIELD(...) TEXTFIELD_(__COUNTER__,__VA_ARGS__)

  #define DECL_OP_(cnt,...) prompt op##cnt(__VA_ARGS__);
  #define DECL_FIELD_(cnt,target,...) menuField<typeof(target)> _menuField##cnt(target,__VA_ARGS__);
  #define DECL_TEXTFIELD_(cnt,target,...) menuTextField _menuTextField##cnt(target,__VA_ARGS__);
  #define DECL_SUBMENU(id)
	#define DECL_VALUE(...) _##__VA_ARGS__
	#define _VALUE_(cnt,...) choice##cnt(__VA_ARGS__);

  #define DEF_OP_(cnt,...) &op##cnt
  #define DEF_FIELD_(cnt,...) &_menuField##cnt
  #define DEF_TEXTFIELD_(cnt,...) &_menuTextField##cnt
  #define DEF_SUBMENU(id) &id
  #define DEF_VALUE(id) &id
  #define DEF_VALUE_(cnt,...) &choice##cnt

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
    virtual void printPrompt(prompt &o,bool selected,int idx,int posY,int width);
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
	class promptAction {
	public:
		typedef void (*callback)(prompt &p, menuOut &o, Stream &i);//callback fynction type
		callback hFn;//the hooked callback function
		//cast no arguments or partial arguments to be accepted as promptActions
		inline promptAction() {}
		inline promptAction(void (*f)()):hFn((callback)f) {}
		inline promptAction(void (*f)(prompt&)):hFn((callback)f) {}
		inline promptAction(void (*f)(prompt&,menuOut&)):hFn((callback)f) {}
		inline promptAction(callback f):hFn(f) {}
		//use this objects as a function (replacing functions)
		inline void operator()(prompt &p, menuOut &o, Stream &i) {hFn(p,o,i	);}
	};

	//holds a menu option
	//a menu is also a prompt so we can have sub-menus
  class prompt {
    public:
    const char* text;
    static void nothing() {}
    promptAction action;
    bool enabled;
    inline prompt(const char * text):text(text),enabled(true),action(nothing) {}
    inline prompt(const char * text,promptAction action)
    	:text(text),action(action),enabled(true) {}
    virtual void printTo(menuOut& p) {
			p.print(text);
		}
    virtual void activate(menuOut& p,Stream&c,bool) {
    	action(*this,p,c);
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
    static const char *exit;//text used for exit option
    static char enabledCursor;//character to be used as navigation cursor
    static char disabledCursor;//to be used when navigating over disabled options
    static prompt exitOption;//option to append to menu allowing exit when no escape button/key is available
    const int sz;
    int sel;//selection
    prompt* const* data;// PROGMEM;
    bool canExit;//store last canExit value for inner reference
    menu(const char * text,int sz,prompt* const data[]):menuNode(text),sz(sz),data(data),canExit(false) {}

    inline void setPosition(int x,int y) {ox=x;oy=y;}
    int menuKeys(menuOut &p,Stream& c,bool drawExit);
    inline void printMenu(menuOut& p,bool drawExit) {
    	p.printMenu(*this,drawExit);
    }
    //force menu redraw on selected device
    inline void redraw(menuOut& p,Stream& c,bool canExit=false) {p.drawn=NULL;poll(p,c,canExit);}
    //set the focus to menu->Option idx, the focused menu will exit to the current
    inline void focus(int idx,menu *prev=NULL) {
      previousMenu=prev;
      activeNode=this;//redirect drawing
      sel=(idx>=sz||idx<0)?sz-1:idx;//focus idx option if out of range then the last will be selected
    }

    void activate(menuOut& p,Stream& c,bool canExit=false);

    void poll(menuOut& p,Stream& c,bool canExit=false);

    virtual bool isMenu() const {return true;}
  };

#endif
