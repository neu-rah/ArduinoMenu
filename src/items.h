/* -*- C++ -*- */
#ifndef RSITE_ARDUINO_MENU_SYSTEM_ITEMS
  #define RSITE_ARDUINO_MENU_SYSTEM_ITEMS

  #include "menuBase.h"
  #include "shadows.h"

  namespace Menu {

    #ifdef MENU_FMT_WRAPS
      enum classes {noClass=0,promptClass,textFieldClass,fieldClass,toggleClass,selectClass,chooseClass,valueClass,menuClass};
    #endif

    #ifdef MENU_ASYNC
      template<typename T> inline const char* typeStr() {return "o";}
      template<> inline const char* typeStr<uint8_t>() {return "u8";}
      template<> inline const char* typeStr<uint16_t>() {return "u16";}
      template<> inline const char* typeStr<uint32_t>() {return "u32";}
      template<> inline const char* typeStr<int8_t>() {return "i8";}
      template<> inline const char* typeStr<int16_t>() {return "i16";}
      template<> inline const char* typeStr<int32_t>() {return "i32";}
      // template<> inline const char* typeStr<long>() {return "long";}
      // template<> inline const char* typeStr<unsigned long>() {return "unsigned long";}
      // template<> inline const char* typeStr<long long>() {return "long long";}
      // template<> inline const char* typeStr<unsigned long long>() {return "unsigned long long";}
      template<> inline const char* typeStr<float>() {return "f";}
      template<> inline const char* typeStr<double>() {return "d";}
    #endif
    // Menu objects and data
    //////////////////////////////////////////////////////////////////////////
    class prompt {
      friend class navNode;
      friend class menuOut;
      protected:
      public:
        constMEM promptShadow* shadow;//constant read-only data (PROGMEM)
      public:
        status enabled=enabledStatus;//ignore enter if false
        bool dirty=true;//needs to be  redrawn
        inline uint16_t hash() const {
          int tmp=(int)this;
          return ((uint16_t*)&tmp)[sizeof(this)/sizeof(uint16_t)-1];
        }
        inline prompt(constMEM promptShadow& shadow):shadow(&shadow) {}
        inline prompt(constText* t,action a=doNothing,eventMask e=noEvent,styles s=noStyle,systemStyles ss=_noStyle)
          :shadow(new promptShadow(t,a,e,s,ss)) {}
        inline void enable() {enabled=enabledStatus;}
        inline void disable() {enabled=disabledStatus;}
        inline constText* getText() const {return shadow->getText();}

        inline systemStyles sysStyles() const {return shadow->_sysStyles();}
        inline styles style() const {return shadow->_style();}
        inline eventMask events() const {return shadow->_events();}

        virtual idx_t sz() const {return 0;}
        inline bool is(systemStyles chk)  const {return (sysStyles()&chk)==chk;}
        inline bool has(systemStyles chk) const {return sysStyles()&chk;}
        inline bool is(styles chk)        const {return (style()&chk)==chk;}
        inline bool has(styles chk)       const {return style()&chk;}
        inline bool is(eventMask chk)     const {return (events()&chk)==chk;}
        inline bool has(eventMask chk)    const {return events()&chk;}

        inline bool canWrap()    const {return style()&wrapStyle;}
        inline bool canNav()     const {return sysStyles()&_canNav;}//can receive navigation focus and process keys
        inline bool isMenu()     const {return sysStyles()&_menuData;}//has menu data list and can be a navNode target
        inline bool isVariant()  const {return sysStyles()&_isVariant;}//a menu as an enumerated field, connected to a variable value
        inline bool parentDraw() const {return sysStyles()&_parentDraw;}//a menu as an enumerated field, connected to a variable value
        inline bool asPad()      const {return sysStyles()&_asPad;}//a menu as an enumerated field, connected to a variable value

        inline bool hasTitle(navNode& nav) const;

        inline result operator()(FUNC_PARAMS) const {return (*shadow)(FUNC_VALUES);}
        idx_t printRaw(menuOut& out,idx_t len) const;

        virtual void clearChanged(const navNode &nav,const menuOut& out,bool sub) {dirty=false;}
        virtual Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr=0);//raw print to output device
        virtual bool changed(const navNode &nav,const menuOut& out,bool sub=true,bool test=false);
        //this is the system version of enter handler, its used by elements like toggle
        virtual result sysHandler(SYS_FUNC_PARAMS) {return proceed;}
        virtual result eventHandler(eventMask e,navNode& nav,idx_t i);
        #ifdef MENU_FMT_WRAPS
          virtual classes type() const;
        #endif
        #ifdef MENU_ASYNC
          // virtual prompt* seek(idx_t* uri,idx_t len) {return len?NULL:this;}
          virtual bool async(const char*uri,navRoot& root,idx_t lvl);
          //some functions to use on htmlFmt
          // for enumerations:
          virtual idx_t selected() const;
          virtual const char* typeName() const;
        #endif
        #ifdef MENU_ASYNC
          virtual void printValue(menuOut&) const {}
          virtual void printHigh(menuOut&) const {}
          virtual void printLow(menuOut&) const {}
          virtual void printStep(menuOut& o) const {};
          virtual void printTune(menuOut& o) const {};
        #endif
    };

    class Exit:public prompt {
      public:
        inline Exit(constText* t):prompt(t,(callback)doExit,enterEvent) {}
    };

    //--------------------------------------------------------------------------
    // can receive navigation focus and process keys
    // this is the minimal candidate for navRoot::navFocus
    class navTarget:public prompt {
      public:
        inline navTarget(constMEM promptShadow& shadow):prompt(shadow) {}
        inline navTarget(constText* t,action a=doNothing,eventMask e=noEvent,styles s=noStyle,systemStyles ss=_noStyle)
          :prompt(t,a,e,s,ss) {}
        virtual void parseInput(navNode& nav,menuIn& in);
        virtual void doNav(navNode& nav,navCmd cmd);
        #ifdef MENU_ASYNC
          const char* typeName() const override;
        #endif
    };

    //--------------------------------------------------------------------------
    class textField:public navTarget {
    public:
      // int hash=0;//not implemented yet
      bool charEdit=false;
      bool edited=false;
      idx_t cursor=0;
      inline textField(constMEM textFieldShadow& shadow):navTarget(shadow) {}
      inline textField(
        constText*label,
        char* b,
        idx_t sz,
        char* const* v,
        action a=doNothing,
        eventMask e=noEvent,
        styles style=noStyle,
        systemStyles ss=(Menu::systemStyles)(_noStyle|_canNav|_parentDraw)
      ):navTarget(*new textFieldShadow(label,b,sz,v,a,e,style,ss)) {}
      inline char* buffer() const {return ((textFieldShadow*)shadow)->_buffer();}
      inline idx_t sz() const {return ((textFieldShadow*)shadow)->_sz();}
      constText* validator(int i);
      void parseInput(navNode& nav,menuIn& in) override;
      void doNav(navNode& nav,navCmd cmd) override;
      Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr=0) override;
      #ifdef MENU_FMT_WRAPS
        virtual classes type() const;
      #endif
      #ifdef MENU_ASYNC
        bool async(const char*uri,navRoot& root,idx_t lvl) override;
        const char* typeName() const override;
      #endif
    };

    //--------------------------------------------------------------------------
    //using this fieldBase saves 418 flash bytes per field template instantiation
    class fieldBase:public navTarget {
      public:
        bool tunning=false;
        inline fieldBase(constMEM promptShadow& shadow):navTarget(shadow) {}
        #ifdef MENU_FMT_WRAPS
          virtual classes type() const;
        #endif
        #ifdef MENU_ASYNC
          bool async(const char*uri,navRoot& root,idx_t lvl) override;
        #endif
        inline constText* units() {return ((fieldBaseShadow*)shadow)->_units();}
        void doNav(navNode& nav,navCmd cmd) override;
        virtual bool canTune()=0;
        virtual void constrainField()=0;
        virtual void stepit(int increment)=0;
        virtual idx_t printReflex(menuOut& o) const =0;
        Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr=0) override;
        #ifdef MENU_ASYNC
          const char* typeName() const override;
        #endif
    };
    //--------------------------------------------------------------------------
    template<typename T>
    class menuField:public fieldBase {
      public:
        T reflex;
        menuField(constMEM menuFieldShadow<T> & shadow):fieldBase(shadow) {}
        menuField(
          T &value,
          constText* text,
          constText*units,
          T low,
          T high,
          T step,
          T tune,
          action a=doNothing,
          eventMask e=noEvent,
          styles s=noStyle
        ):menuField(*new menuFieldShadow<T>(value,text,units,low,high,step,tune,a,e,s)) {}
        bool canTune() override;
        void constrainField() override;
        idx_t printReflex(menuOut& o) const override;
        void parseInput(navNode& nav,menuIn& in) override;
        Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr=0) override;
        inline T& target() const {return ((menuFieldShadow<T>*)shadow)->target();}
        inline T getTypeValue(constMEM T* from) const {return ((menuFieldShadow<T>*)shadow)->getTypeValue(from);}
        inline T low() const {return  ((menuFieldShadow<T>*)shadow)->_low();}
        inline T high() const {return ((menuFieldShadow<T>*)shadow)->_high();}
        inline T step() const {return ((menuFieldShadow<T>*)shadow)->_step();}
        inline T tune() const {return ((menuFieldShadow<T>*)shadow)->_tune();}
        void clearChanged(const navNode &nav,const menuOut& out,bool sub) override;
        bool changed(const navNode &nav,const menuOut& out,bool sub=true,bool test=false) override;
        void stepit(int dir) override;
        #ifdef MENU_ASYNC
          void printValue(menuOut& o) const override;
          void printHigh(menuOut& o) const override;
          void printLow(menuOut& o) const override;
          void printStep(menuOut& o) const override;
          void printTune(menuOut& o) const override;
          const char* typeName() const override;
        #endif
      };

      template<uint8_t dps>
      struct decPlaces {
        template<typename T>
        struct menuField:Menu::menuField<T> {
          using Menu::menuField<T>::menuField;
          idx_t printReflex(menuOut& o) const override;
        };
      };

    //--------------------------------------------------------------------------
    template<typename T>
    class menuValue:public prompt {
      public:
        inline menuValue(constMEM menuValueShadow<T>& shadow):prompt(shadow) {}
        inline menuValue(constText* text,T value,action a=doNothing,eventMask e=noEvent)
          :menuValue(*new menuValueShadow<T>(text,value,a,e)) {}
        inline T target() const {return ((menuValueShadow<T>*)shadow)->target();}
        #ifdef MENU_FMT_WRAPS
          virtual classes type() const {return valueClass;}
        #endif
        #ifdef MENU_ASYNC
          bool async(const char*uri,navRoot& root,idx_t lvl=0) override;
          const char* typeName() const override;
        #endif
    };

    //--------------------------------------------------------------------------
    class menuNode:public navTarget {
      public:
        inline menuNode(constMEM menuNodeShadow& s):navTarget(s) {}
        inline menuNode(const char* text,idx_t sz,prompt* constMEM data[],action a=noAction,eventMask e=noEvent,styles style=wrapStyle,systemStyles ss=(systemStyles)(_menuData|_canNav))
          :navTarget(*new menuNodeShadow(text,sz,data,a,e,style,ss)) {}
        #ifdef MENU_FMT_WRAPS
          virtual classes type() const;
        #endif
        virtual prompt& operator[](idx_t i) const {return ((menuNodeShadow*)shadow)->operator[](i);}
        bool changed(const navNode &nav,const menuOut& out,bool sub=true,bool test=false) override;
        void clearChanged(const navNode &nav,const menuOut& out,bool sub) override;
        inline idx_t sz() const {return ((menuNodeShadow*)shadow)->_sz();}
        inline prompt* constMEM* data() const {return ((menuNodeShadow*)shadow)->_data();}
        #if defined(MENU_USERAM) && ! defined(swap)
          //Adafruit_GFX_AS defines swap macro (what a bad idea)
          void swap(idx_t a,idx_t b) {
            if (has(_menuData)) {//ignore on virtual data menus
              auto ops=((menuNodeShadow*)shadow)->_data();
              auto tmp=ops[a];
              ops[a]=ops[b];
              ops[b]=tmp;
            }
          }
        #endif
        // #ifdef MENU_ASYNC
        //   prompt* seek(idx_t* uri,idx_t len) override;
        // #endif
        #ifdef MENU_ASYNC
          bool async(const char*uri,navRoot& root,idx_t lvl=0) override;
          const char* typeName() const override;
          //aux function, parse uri node (text to idx_t)
          idx_t parseUriNode(const char*&uri);
        #endif
        template<bool clear>
        bool _changes(const navNode &nav,const menuOut& out,bool sub,bool test);
    };

    //--------------------------------------------------------------------------
    class menu:public menuNode {
      public:
        inline menu(constMEM menuNodeShadow& shadow):menuNode(shadow) {}
        // Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr=0) override {
        // }
        #ifdef MENU_ASYNC
          const char* typeName() const override;
        #endif
    };

    //--------------------------------------------------------------------------
    class menuVariantBase:public menuNode {
      public:
        inline menuVariantBase(constMEM menuNodeShadow& s):menuNode(s) {}
        virtual idx_t sync()=0;
        virtual idx_t sync(idx_t i)=0;
        Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr=0) override;
        idx_t togglePrintTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr);
        void doNav(navNode& nav,navCmd cmd) override;
        #ifdef MENU_ASYNC
          const char* typeName() const override;
        #endif
    };
    template<typename T>
    class menuVariant:public menuVariantBase {
      public:
        idx_t reflex;
        inline menuVariant(constMEM menuNodeShadow& s):menuVariantBase(s) {}
        inline menuVariant(constText* text,T &target,idx_t sz,prompt* constMEM* data,action a,eventMask e,styles style)
          :menuVariantBase(*new menuVariantShadow<T>(text,target,sz,data,a,e,style)) {}
        idx_t sync() override;
        idx_t sync(idx_t i) override;
        inline T& target() const {return ((menuVariantShadow<T>*)shadow)->target();}
        bool changed(const navNode &nav,const menuOut& out,bool sub=true,bool test=false) override;
        #ifdef MENU_ASYNC
          idx_t selected() const override;
          const char* typeName() const override;
        #endif
    };

    template<typename T>//-------------------------------------------
    class select:public menuVariant<T> {
      public:
        inline select(constMEM menuNodeShadow& s):menuVariant<T>(s) {}
        inline select(
          constText* text,
          T &target,
          idx_t sz,
          prompt* constMEM* data,
          action a=doNothing,
          eventMask e=noEvent,
          styles style=noStyle,
          systemStyles ss=((systemStyles)(Menu::_menuData|Menu::_canNav|Menu::_isVariant|Menu::_parentDraw))
        ):menuVariant<T>(*new menuVariantShadow<T>(text,target,sz,data,a,e,style,ss)) {}
        #ifdef MENU_FMT_WRAPS
          virtual classes type() const {return selectClass;}
        #endif
        #ifdef MENU_ASYNC
          const char* typeName() const override {return "select";}
        #endif
    };

    template<typename T>//-------------------------------------------
    class toggle:public menuVariant<T> {
      public:
        inline toggle(constMEM menuNodeShadow& s):menuVariant<T>(s) {}
        inline toggle(
          constText* text,
          T &target,
          idx_t sz,
          prompt* constMEM* data,
          action a=doNothing,
          eventMask e=noEvent,
          styles style=noStyle,
          systemStyles ss=((systemStyles)(Menu::_menuData|Menu::_isVariant))
        ):menuVariant<T>(*new menuVariantShadow<T>(text,target,sz,data,a,e,style,ss)) {}
        #ifdef MENU_FMT_WRAPS
          classes type() const override;
        #endif
        Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr=0) override;
        result sysHandler(SYS_FUNC_PARAMS) override;
        #ifdef MENU_ASYNC
          const char* typeName() const override;
          bool async(const char*uri,navRoot& root,idx_t lvl) override;
        #endif
    };

    template<typename T>//-------------------------------------------
    class choose:public menuVariant<T> {
      public:
        inline choose(constMEM menuNodeShadow& s):menuVariant<T>(s) {}
        inline choose(
          constText* text,
          T &target,
          idx_t sz,
          prompt* constMEM* data,
          action a=doNothing,
          eventMask e=noEvent,
          styles style=noStyle,
          systemStyles ss=((systemStyles)(Menu::_menuData|Menu::_canNav|Menu::_isVariant))
        ):menuVariant<T>(*new menuVariantShadow<T>(text,target,sz,data,a,e,style,ss)) {}
        #ifdef MENU_FMT_WRAPS
          classes type() const override;
        #endif
        Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr=0) override;
        result sysHandler(SYS_FUNC_PARAMS) override;
        bool changed(const navNode &nav,const menuOut& out,bool sub=true,bool test=false) override;
        #ifdef MENU_ASYNC
          const char* typeName() const override;
        #endif
    };

  }

#endif
