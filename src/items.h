/* -*- C++ -*- */
#ifndef RSITE_ARDUINO_MENU_SYSTEM_ITEMS
  #define RSITE_ARDUINO_MENU_SYSTEM_ITEMS

  #include "menuBase.h"
  #include "shadows.h"

  namespace Menu {

    #ifdef MENU_FMT_WRAPS
      enum classes {noClass=0,promptClass,textFieldClass,fieldClass,toggleClass,selectClass,chooseClass,valueClass,menuClass};
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
        virtual bool changed(const navNode &nav,const menuOut& out,bool sub=true,bool test=false) {return dirty;}
        //this is the system version of enter handler, its used by elements like toggle
        virtual result sysHandler(SYS_FUNC_PARAMS) {return proceed;}
        virtual result eventHandler(eventMask e,navNode& nav,idx_t i) {
          return operator()(e,nav,*this);
        }
        #ifdef MENU_FMT_WRAPS
          virtual classes type() const {return promptClass;}
        #endif
        #ifdef MENU_ASYNC
          virtual prompt* seek(idx_t* uri,idx_t len) {return len?NULL:this;}
          virtual bool async(constText*uri,navRoot& root,idx_t lvl) {
            return ((!*uri)||(uri[0]=='/'&&!uri[1]));
          }
          //some functions to use on htmlFmt
          // for enumerations:
          virtual idx_t selected() const {return 0;}
        #endif
        #ifdef DEBUG
          virtual void printValue(menuOut&) const {}
          virtual void printHigh(menuOut&) const {}
          virtual void printLow(menuOut&) const {}
        #endif
    };

    class Exit:public prompt {
      public:
        Exit(constText* t):prompt(t,(callback)doExit,enterEvent) {}
    };

    //--------------------------------------------------------------------------
    // can receive navigation focus and process keys
    // this is the minimal candidate for navRoot::navFocus
    class navTarget:public prompt {
      public:
        navTarget(constMEM promptShadow& shadow):prompt(shadow) {}
        navTarget(constText* t,action a=doNothing,eventMask e=noEvent,styles s=noStyle,systemStyles ss=_noStyle)
          :prompt(t,a,e,s,ss) {}
        virtual void parseInput(navNode& nav,menuIn& in);
        virtual void doNav(navNode& nav,navCmd cmd);
    };

    //--------------------------------------------------------------------------
    class textField:public navTarget {
    public:
      // int hash=0;//not implemented yet
      bool charEdit=false;
      bool edited=false;
      idx_t cursor=0;
      textField(constMEM textFieldShadow& shadow):navTarget(shadow) {}
      textField(
        constText*label,
        char* b,
        idx_t sz,
        constText* constMEM* v,
        action a=doNothing,
        eventMask e=noEvent,
        styles style=noStyle,
        systemStyles ss=(Menu::systemStyles)(_noStyle|_canNav|_parentDraw)
      ):navTarget(*new textFieldShadow(label,b,sz,v,a,e,style,ss)) {}
      inline char* buffer() const {return ((textFieldShadow*)shadow)->_buffer();}
      inline idx_t sz() const {return ((textFieldShadow*)shadow)->_sz();}
      constText* validator(int i) {return ((textFieldShadow*)shadow)->operator[](i%sz());}
      void doNav(navNode& nav,navCmd cmd) override;
      Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr=0) override;
    };

    //--------------------------------------------------------------------------
    //using this fieldBase saves 418 flash bytes per field template instantiation
    class fieldBase:public navTarget {
      public:
        bool tunning=false;
        fieldBase(constMEM promptShadow& shadow):navTarget(shadow) {}
        #ifdef MENU_FMT_WRAPS
          virtual classes type() const {return fieldClass;}
        #endif
        #ifdef MENU_ASYNC
          bool async(constText*uri,navRoot& root,idx_t lvl) override;
        #endif
        inline constText* units() {return ((fieldBaseShadow*)shadow)->_units();}
        void doNav(navNode& nav,navCmd cmd) override;
        virtual bool canTune()=0;
        virtual void constrainField()=0;
        virtual void stepit(int increment)=0;
        virtual idx_t printReflex(menuOut& o) const =0;
        Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr=0) override;
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
        bool canTune() override {return !!tune();}
        void constrainField() override {target() = constrain(target(), low(), high());}
        idx_t printReflex(menuOut& o) const override;
        void parseInput(navNode& nav,menuIn& in) override;
        Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr=0) override;
        inline T& target() const {return ((menuFieldShadow<T>*)shadow)->target();}
        inline T getTypeValue(constMEM T* from) const {return ((menuFieldShadow<T>*)shadow)->getTypeValue(from);}
        inline T low() const {return  ((menuFieldShadow<T>*)shadow)->_low();}
        inline T high() const {return ((menuFieldShadow<T>*)shadow)->_high();}
        inline T step() const {return ((menuFieldShadow<T>*)shadow)->_step();}
        inline T tune() const {return ((menuFieldShadow<T>*)shadow)->_tune();}
        virtual void clearChanged(const navNode &nav,const menuOut& out,bool sub) {
          fieldBase::clearChanged(nav,out,sub);
          reflex=target();
        }
        bool changed(const navNode &nav,const menuOut& out,bool sub=true,bool test=false) override {
          trace(if (test&&dirty) Serial<<"field dirty"<<endl);
          trace(if (test&&(reflex!=target())) Serial<<"reflex!=target reflex:"<<reflex<<" target:"<<target()<<endl);
          return dirty||(reflex!=target());
        }
        #ifdef DEBUG
          void printValue(menuOut& o) const override;
          void printHigh(menuOut& o) const override;
          void printLow(menuOut& o) const override;
        #endif
        void stepit(int dir) override {
          dir*=options->invertFieldKeys?-1:1;
          T thisstep = tunning?tune():step();
          dirty=true;
          //by default they are inverted.. now buttons and joystick have to flip them
          if (dir > 0) {
            if ((high()-target()) < thisstep)
              target() = canWrap()?low():high();
            else
              target() += thisstep;
          } else {
            if ((target()-low()) < thisstep)
              target() = canWrap()?high():low();
            else
              target() -= thisstep;
          }
        }
      };

    //--------------------------------------------------------------------------
    template<typename T>
    class menuValue:public prompt {
      public:
        menuValue(constMEM menuValueShadow<T>& shadow):prompt(shadow) {}
        menuValue(constText* text,T value,action a=doNothing,eventMask e=noEvent)
          :menuValue(*new menuValueShadow<T>(text,value,a,e)) {}
        // #ifdef DEBUG
        // bool changed(const navNode &nav,const menuOut& out,bool sub=true) override {return false;}
        // #endif
        //inline T getTypeValue(const T* from) const {return &((menuValueShadow<T>*)shadow)->getTypeValue(from);}
        inline T target() const {return ((menuValueShadow<T>*)shadow)->target();}
        #ifdef MENU_FMT_WRAPS
          virtual classes type() const {return valueClass;}
        #endif
    };

    //--------------------------------------------------------------------------
    class menuNode:public navTarget {
      public:
        menuNode(constMEM menuNodeShadow& s):navTarget(s) {}
        menuNode(constText* text,idx_t sz,prompt* constMEM data[],action a=noAction,eventMask e=noEvent,styles style=wrapStyle,systemStyles ss=(systemStyles)(_menuData|_canNav))
          :navTarget(*new menuNodeShadow(text,sz,data,a,e,style,ss)) {}
        #ifdef MENU_FMT_WRAPS
          virtual classes type() const {return menuClass;}
        #endif
        inline prompt& operator[](idx_t i) const {return ((menuNodeShadow*)shadow)->operator[](i);}
        bool changed(const navNode &nav,const menuOut& out,bool sub=true,bool test=false) override;
        void clearChanged(const navNode &nav,const menuOut& out,bool sub) override;
        inline idx_t sz() const {return ((menuNodeShadow*)shadow)->_sz();}
        inline prompt* constMEM* data() const {return ((menuNodeShadow*)shadow)->_data();}
        #ifdef MENU_ASYNC
          prompt* seek(idx_t* uri,idx_t len) override;
        #endif
        #ifdef MENU_ASYNC
          bool async(constText*uri,navRoot& root,idx_t lvl=0) override;
        #endif
    };

    //--------------------------------------------------------------------------
    class menu:public menuNode {
      public:
        menu(constMEM menuNodeShadow& shadow):menuNode(shadow) {}
        // Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr=0) override {
        // }
    };

    //--------------------------------------------------------------------------
    class menuVariantBase:public menuNode {
      public:
        menuVariantBase(constMEM menuNodeShadow& s):menuNode(s) {}
        virtual idx_t sync()=0;
        virtual idx_t sync(idx_t i)=0;
        Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr=0) override;
        idx_t togglePrintTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr);
        void doNav(navNode& nav,navCmd cmd) override;
    };
    template<typename T>
    class menuVariant:public menuVariantBase {
      public:
        idx_t reflex;
        menuVariant(constMEM menuNodeShadow& s):menuVariantBase(s) {}
        menuVariant(constText* text,T &target,idx_t sz,prompt* constMEM* data,action a,eventMask e,styles style)
          :menuVariantBase(*new menuVariantShadow<T>(text,target,sz,data,a,e,style)) {}
        idx_t sync() override {
          for(idx_t i=0;i<sz();i++)
            if (((menuValue<T>*)&operator[](i))->target()==target()) return i;
          #ifdef DEBUG
            Serial.print(F("value out of range "));
            Serial.println(target());Serial.flush();
            assert(false);
          #endif
          return -1;
        }
        idx_t sync(idx_t i) override {
          #ifdef DEBUG
            if (!(i>=0&&i<sz())){
              print_P(Serial,getText());
              Serial.print(F(" : value out of range "));
              Serial.println(i);
            }
            assert(i>=0&&i<sz());
          #endif
          if (i!=reflex) dirty=true;
          reflex=i;
          target()=((menuValue<T>*)&operator[](i))->target();
          return i;
        }
        inline T& target() const {return ((menuVariantShadow<T>*)shadow)->target();}
        bool changed(const navNode &nav,const menuOut& out,bool sub=true,bool test=false) override;
        #ifdef MENU_ASYNC
        virtual idx_t selected() const {return reflex;}
        #endif
    };

    template<typename T>//-------------------------------------------
    class select:public menuVariant<T> {
      public:
        select(constMEM menuNodeShadow& s):menuVariant<T>(s) {}
        select(
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
    };

    template<typename T>//-------------------------------------------
    class toggle:public menuVariant<T> {
      public:
        toggle(constMEM menuNodeShadow& s):menuVariant<T>(s) {}
        toggle(
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
          virtual classes type() const {return toggleClass;}
        #endif
        Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr=0) override;
        result sysHandler(SYS_FUNC_PARAMS) override {
          switch(event) {
              case activateEvent: {
              idx_t at=menuVariant<T>::sync();
              assert(at!=-1);
              at++;
              if (at>=menuNode::sz()) at=0;
              menuVariant<T>::sync(at);
              prompt::dirty=true;
              (menuNode::operator[](at))(FUNC_VALUES);
            }
            default:
              return proceed;
          }
        }
    };

    template<typename T>//-------------------------------------------
    class choose:public menuVariant<T> {
      public:
        choose(constMEM menuNodeShadow& s):menuVariant<T>(s) {}
        choose(
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
          virtual classes type() const {return chooseClass;}
        #endif
        Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr=0) override;
        result sysHandler(SYS_FUNC_PARAMS) override;
        bool changed(const navNode &nav,const menuOut& out,bool sub=true,bool test=false) override {
          return menuVariant<T>::changed(nav,out)||menuNode::changed(nav,out);
        }
    };

  }

  ////////////////////////////////////////////////////////////////////////
  // template implementation
  #include "io.h"
  #include "nav.h"
  namespace Menu {

    template<typename T>
    idx_t menuField<T>::printReflex(menuOut& o) const {return o.print(reflex);}

    template<typename T>
    Used menuField<T>::printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr) {
      trace(print_P(out,getText());Serial<<" menuField<T>::printTo "<<reflex<<endl);
      reflex=target();
      return fieldBase::printTo(root,sel,out,idx,len,panelNr);
    }

    template<typename T>
    void menuField<T>::parseInput(navNode& nav,menuIn& in) {
      if (strchr(numericChars,in.peek())) {//a numeric value was entered
        if (in.numValueInput) {
          target()=(T)in.parseFloat();
          tunning=true;
          doNav(nav,enterCmd);
        } else doNav(nav,idxCmd);
      } else doNav(nav,nav.navKeys(in.read()));
    }

    #ifdef DEBUG
      template<typename T>
      void menuField<T>::printValue(menuOut& o) const {o.print(reflex);}
      template<typename T>
      void menuField<T>::printHigh(menuOut& o) const {o.print(high());}
      template<typename T>
      void menuField<T>::printLow(menuOut& o) const {o.print(low());}
    #endif

    template<typename T>
    Used choose<T>::printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr) {
      bool ed=this==root.navFocus;
      return ed?
        prompt::printTo(root,sel,out,idx,len,panelNr)
        :menuVariantBase::printTo(root,sel,out,idx,len,panelNr);
    }

    template<typename T>
    Used toggle<T>::printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr) {
      return menuVariantBase::togglePrintTo(root,sel,out,idx,len,panelNr);
    }

    template<typename T>
    bool menuVariant<T>::changed(const navNode &nav,const menuOut& out,bool sub,bool test) {
      return dirty||((menuValue<T>*)&operator[](reflex))->target()!=target();
    }

    template<typename T>
    result choose<T>::sysHandler(SYS_FUNC_PARAMS) {
      switch(event) {
        case updateEvent:
        case enterEvent:
          nav.sel=menuVariant<T>::sync();
        default:
          return proceed;
      }
    }
  }//namespace Menu
#endif
