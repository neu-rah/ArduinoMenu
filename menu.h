/********************
Arduino generic menu system
Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

    prompt: class representing a text and an associated function pointer
      menu: prompt derived holding a list of prompts (options and submenus)
   menuOut: print derived with special virtual functions needed for menu output, derive this to have the menu in other devices
 menuPrint: menuOut implementation for generic print (as Serial)
   menuLCD: menuOut implementation for standard LiquidCrystal LCD

the menu system will read provided stream for input, it works for Serial
for encoders, joysticks, keyboards or touch a stream must be made out of them
*/
#ifndef RSITE_ARDUINOP_MENU_SYSTEM
  #define RSITE_ARDUINOP_MENU_SYSTEM
  
  #include <LiquidCrystal.h>
  
  class prompt;
  class menu;
  class menuOut;
  
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
   FOR_EACH_4(what,  __VA_ARGS__);
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
  
  #define OP(...) OP_(__COUNTER__,__VA_ARGS__)
  #define DECL_OP_(cnt,...) prompt op##cnt(__VA_ARGS__);
  #define DECL_SUBMENU(id)
  #define DEF_OP_(cnt,...) &op##cnt
  #define DEF_SUBMENU(id) &id
  
  /////////////////////////////////////////////////////////
  // menu output device
  class menuOut {
    public:
    int top;//top for this device
    //device size
    int maxX;
    int maxY;
    //device resolution
    int resX;
    int resY;
    enum styles {
      enumerated, //print numbers or references for options, keyboard or file
      cursor,//print a cursor at selected option, tracking device
      point,//pointing device
    } style;
    //menuOut(menuOut::styles style=menuOut::enumerated):maxX(0),maxY(0),style(style),top(0) {}
    menuOut(menuOut::styles style=menuOut::enumerated,int x=0x7F,int y=0x7F):maxX(x),maxY(y),style(style),top(0),resX(1),resY(1) {}
    virtual void clear()=0;
    virtual void setCursor(int x,int y)=0;
    virtual void print(const char *text)=0;
    virtual void println(const char *text)=0;
    virtual void print(int)=0;
    virtual void println(int)=0;
  };
  
  class menuPrint:public menuOut {
    public:
    Print& device;
    menuPrint(Print& device):device(device) {}
    virtual void clear() {device.println("");device.println("");}
    virtual void setCursor(int x,int y) {device.println("");}
    virtual void print(const char *text) {device.print(text);}
    virtual void println(const char *text) {device.println(text);}
    virtual void print(int i) {device.print(i);};
    virtual void println(int i) {device.println(i);};
  };
  
  class menuLCD:public menuOut {
    public:
    LiquidCrystal& lcd;
    menuLCD(LiquidCrystal& lcd,int x=16,int y=1):lcd(lcd),menuOut(menuOut::cursor,x,y) {}
    virtual void clear() {lcd.clear();}
    virtual void setCursor(int x,int y) {lcd.setCursor(x*resX,y*resY);}
    virtual void print(const char *text) {lcd.print(text);}
    virtual void println(const char *text) {lcd.print(text);};
    virtual void print(int i) {lcd.print(i);};
    virtual void println(int i) {lcd.println(i);};
  };
  
  ////////////////////////////////////////////////////////////////////
  // menu structure
  
	/*class promptAction {
	public:
		typedef void (*menuHandler)(prompt&,menuOut&,Stream&);
		menuHandler hFn;
		inline promptAction() {}
		inline promptAction(void (*f)()):hFn((menuHandler)f) {}
		inline promptAction(void (*f)(prompt&)):hFn((menuHandler)f) {}
		inline promptAction(void (*f)(prompt &p,menuOut &o)):hFn((menuHandler)f) {}
		inline promptAction(menuHandler f):hFn(f) {}
		inline void operator()(prompt &p,menuOut &o,Stream &i) {hFn(p,o,i);}
	};*/
	
	template<typename ...Ts>
	class handler {
	public:
		typedef void (*menuHandler)(Ts...);
		menuHandler hFn;
		inline handler() {}
		inline handler(void (*f)()):hFn((menuHandler)f) {}
		inline handler(void (*f)(prompt&)):hFn((menuHandler)f) {}
		inline handler(void (*f)(prompt &p,menuOut &o)):hFn((menuHandler)f) {}
		inline handler(menuHandler f):hFn(f) {}
		inline void operator()(Ts... ts) {hFn(ts...);}
	};

	typedef handler<prompt&,menuOut&,Stream&> promptAction;


  class prompt {
    public:
    const char *text;
    promptAction action;
    prompt(const char * text):text(text) {}
    prompt(const char * text,promptAction action):text(text),action(action) {}
    virtual size_t printTo(Print& p) {p.print(text);return strlen(text);}
    virtual void activate(menuOut& p,Stream&c) {action(*this,p,c);}
    void activate(Print& p,Stream&c) {
      menuPrint tmp(p);
      activate(tmp,c);
    }
    inline void activate(LiquidCrystal& p,Stream&c) {
      menuLCD tmp(p);
      activate(tmp,c);
    }
  };
  
  class menu:public prompt {
    public:
    const int sz;
    int sel;//selection
    prompt* const* data;
    menu(const char * text,int sz,prompt* const data[]):prompt(text),sz(sz),data(data),sel(0) {}
    
    int menuKeys(menuOut &p,Stream& c);
    void printMenu(menuOut& p);
    
    void activate(menuOut& p,Stream& c);
    
    inline void activate(Print& p,Stream&c) {
      menuPrint tmp(p);
      activate(tmp,c);
    }
    
    inline void activate(LiquidCrystal& p,Stream&c) {
      menuLCD tmp(p);//defaults to LCD 16x1
      activate(tmp,c);
    }
  };

#endif
