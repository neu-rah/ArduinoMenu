#ifndef RSITE_ARDUINO_MENU_SYSTEM
  #define RSITE_ARDUINO_MENU_SYSTEM
  #include <Stream.h>
  #include "menuBase.h"

  namespace Menu {

    // Menu objects and data
    //////////////////////////////////////////////////////////////////////////
    struct promptShadowRaw {
      actionRaw a;//the hooked callback function
      systemStyles sysStyles;
      const char*text;
      const eventMask events;//registered events
    };
    class promptShadow:public action {
      public:
        systemStyles sysStyles=_noStyle;
        const char*text;
        const eventMask events=noEvent;//registered events
        promptShadow(const char* t,action a=doNothing,eventMask e=noEvent):action(a),text(t),events(e) {}
    };
    class prompt {
      public:
        bool enabled=true;//ignore enter if false
        bool dirty=true;//needs to be  redrawn
        const promptShadow* shadow;//constant read-only data (PROGMEM)
        inline prompt(const promptShadow& shadow):shadow(&shadow) {}
        inline systemStyles sysStyles() const {return (systemStyles)memWord(&shadow->sysStyles);}
        inline bool canNav() const {return sysStyles()&_canNav;}//can receive navigation focus and process keys
        inline bool isMenu() const {return sysStyles()&_menuData;}//has menu data list and can be a navNode target
        inline bool isVariant() const {return sysStyles()&_isVariant;}//has menu data list and can be a navNode target
        virtual void printTo(idx_t i,navNode &nav,menuOut& out);//raw print to output device
        virtual bool changed(const navNode &nav,const menuOut& out) {return dirty;}
        //this is the system version of enter handler, its used by elements like toggle
        virtual result sysHandler(FUNC_PARAMS) {return proceed;}
        inline result operator()(FUNC_PARAMS) const {return (*shadow)(FUNC_VALUES);}
    };

    //initialization order fiasco invalidates this:
    //extern const promptShadow exitShadow;
    //extern menu& systemOptions;
    //extern prompt exitPrompt;
    //extern prompt calcelPrompt;

    //--------------------------------------------------------------------------
    // can receive navigation focus and prrocess keys
    class navTarget:public prompt {
      public:
        navTarget(const promptShadow& shadow):prompt(shadow) {}
        //bool canNav() const override {return true;}
        virtual void navigate(navNode& nav,char ch,Stream& in,menuOut& out);
    };

    //--------------------------------------------------------------------------
    struct menuNodeShadowRaw {
      actionRaw a;
      systemStyles sysStyles;
      const char*text;
      const eventMask events;//registered events
      idx_t sz;
      prompt* const* data;
      styles style;
      //int width;//field or menu width
      //int ox,oy;//coordinate origin displacement
    };
    class menuNodeShadow:public promptShadow {
      public:
        idx_t sz;
        prompt* const* data;
        styles style;
        //int width;//field or menu width
        //int ox,oy;//coordinate origin displacement
        menuNodeShadow(const char* text,idx_t sz,prompt* const* data,action a,eventMask e,styles style)
        :promptShadow(text,a,e),sz(sz),data(data),style(style) {}
    };
    class menuNode:public navTarget {
      public:
        menuNode(const menuNodeShadow& s):navTarget(s) {}
        inline prompt& operator[](idx_t i) const {
          return *(prompt*)memPtr(((prompt**)memPtr(((menuNodeShadow*)shadow)->data))[i]);
        }
        bool changed(const navNode &nav,const menuOut& out) override;
        inline idx_t sz() const {return memIdx(((menuNodeShadow*)shadow)->sz);}
        inline prompt** data() const {return (prompt**)memPtr(((menuNodeShadow*)shadow)->data);}
    };

    //--------------------------------------------------------------------------
    class menu:public menuNode {
      public:
        menu(const menuNodeShadow& shadow):menuNode(shadow) {}
    };

    //--------------------------------------------------------------------------
    template<typename T>
    struct menuFieldShadowRaw {
      actionRaw a;
      systemStyles sysStyles;
      const char*text;
      const eventMask events;//registered events
      T* value;
      const char* units;
      const T low,high,step,tune;
    };
    template<typename T>
    class menuFieldShadow:public promptShadow {
      public:
        T* value;
        const char* units;
        const T low,high,step,tune;
        menuFieldShadow(T &value,const char * text,const char *units,T low,T high,T step,T tune,action a=doNothing,eventMask e=noEvent)
          :value(&value),units(units),low(low),high(high),step(step),tune(tune),promptShadow(text,a,e) {}
    };
    template<typename T>
    class menuField:public navTarget {
      public:
        bool tunning=false;
        T reflex;
        menuField(const menuFieldShadow<T> & shadow):navTarget(shadow) {}
        void navigate(navNode& nav,char ch,Stream& in,menuOut& out) override;
        void printTo(idx_t i,navNode &nav,menuOut& out) override;
        inline T& target() const {return *(T*)memPtr(((menuFieldShadow<T>*)shadow)->value);}
        inline T getTypeValue(const T* from) const {
          #ifdef USING_PGM
            T tmp;
            memcpy_P(&tmp, from, sizeof(T));
            return tmp;
          #else
            return *from;
          #endif
        }
        inline T low() const {return getTypeValue(&((menuFieldShadow<T>*)shadow)->low);}
        inline T high() const {return getTypeValue(&((menuFieldShadow<T>*)shadow)->high);}
        inline T step() const {return getTypeValue(&((menuFieldShadow<T>*)shadow)->step);}
        inline T tune() const {return  getTypeValue(&((menuFieldShadow<T>*)shadow)->tune);}
        bool changed(const navNode &nav,const menuOut& out) override {
          return dirty||(dirty=reflex!=target());
        }
        void clamp() {
          if (target()<low()) target()=low();
          else if (target()>high()) target()=high();
    		}
    };

    //--------------------------------------------------------------------------
    template<typename T>
    struct menuValueShadowRaw {
      actionRaw a;
      systemStyles sysStyles;
      const char*text;
      const eventMask events;//registered events
      T value;
    };
    template<typename T>
    class menuValueShadow:public promptShadow {
      public:
        T value;
        inline menuValueShadow(const char * text,T value,action a=doNothing,eventMask e=noEvent)
          :promptShadow(text,a,e),value(value) {}
    };
    template<typename T>
    class menuValue:public prompt {
      public:
        menuValue(const menuValueShadow<T>& shadow):prompt(shadow) {}
        #ifdef DEBUG
        bool changed(const navNode &nav,const menuOut& out) override {
          /*Serial<<"Value changed? TODO: remove call to this and this functions!"<<endl;
          assert(false);*/
          return false;
        }
        #endif
        inline T getTypeValue(const T* from) const {
          #ifdef USING_PGM
            T tmp;
            memcpy_P(&tmp, from, sizeof(T));
            return tmp;
          #else
            return *from;
          #endif
        }
        inline T target() const {return getTypeValue(&((menuValueShadow<T>*)shadow)->value);}
    };

    //--------------------------------------------------------------------------
    template<typename T>
    struct menuVariantShadowRaw {
      actionRaw a;
      systemStyles sysStyles;
      const char*text;
      const eventMask events;//registered events
      idx_t sz;
      prompt* const* data;
      styles style;
      //int width;//field or menu width
      //int ox,oy;//coordinate origin displacement
      T* value;
    };
    template<typename T>
    class menuVariantShadow:public menuNodeShadow {
      public:
        T* value;
        menuVariantShadow(const char* text,T &target,idx_t sz,prompt* const* data,action a,eventMask e,styles style)
        :menuNodeShadow(text,sz,data,a,e,style),value(&target) {}
    };
    template<typename T>//-------------------------------------------
    class menuVariant:public menuNode {
      public:
        T reflex;
        menuVariant(const menuNodeShadow& s):menuNode(s) {}
        idx_t sync() {
          menuVariantShadow<T>& s=*(menuVariantShadow<T>*)shadow;
          for(idx_t i=0;i<sz();i++) {
            //Serial<<"checking "<<i<<" "<<target()<<"=="<<((menuValue<T>*)&operator[](i))->target()<<endl;
            if (((menuValue<T>*)&operator[](i))->target()==target()) return i;
          }
          #ifdef DEBUG
          Serial<<"value out of range "<<target()<<endl;Serial.flush();
          assert(false);
          #endif
          return -1;
        }
        //TODO: implement reflexivity!
        idx_t sync(idx_t i) {
          menuVariantShadow<T>& s=*(menuVariantShadow<T>*)shadow;
          #ifdef DEBUG
          if (!(i>=0&&i<sz())){
            Serial<<*(prompt*)this<<" : value out of range "<<i<<endl;
          }
          assert(i>=0&&i<sz());
          #endif
          target()=reflex=((menuValue<T>*)&operator[](i))->target();
          //Serial<<"Set toggle value to :"<<s.value<<endl;
          return i;
        }
        inline T& target() const {return *(T*)memPtr(((menuVariantShadow<T>*)shadow)->value);}
        bool changed(const navNode &nav,const menuOut& out) override {
          //Serial<<"menuVariant::shadow "<<target()<<"/"<<reflex<<endl;
          return dirty||reflex!=target();
        }
        void navigate(navNode& nav,char ch,Stream& in,menuOut& out) override;
        void printTo(idx_t i,navNode &nav,menuOut& out) override;
    };

    template<typename T>//-------------------------------------------
    class select:public menuVariant<T> {
      public:
        select(const menuNodeShadow& s):menuVariant<T>(s) {}
    };

    template<typename T>//-------------------------------------------
    class toggle:public menuVariant<T> {
      public:
        toggle(const menuNodeShadow& s):menuVariant<T>(s) {}
        void printTo(idx_t i,navNode &nav,menuOut& out) override;
        //bool canNav() const override {return false;}//can receive navigation focus and process keys
        result sysHandler(FUNC_PARAMS) override {
          switch(event) {
            case activateEvent: {
              menuNodeShadow& s=*(menuNodeShadow*)prompt::shadow;
              idx_t at=menuVariant<T>::sync();
              assert(at!=-1);
              at++;
              if (at>=menuNode::sz()) at=0;
              menuVariant<T>::sync(at);
              prompt::dirty=true;
              (*menuNode::operator[](at).shadow)(FUNC_VALUES);
            }
            default:
              return proceed;
          }
          //return proceed;
        }
    };

    template<typename T>//-------------------------------------------
    class choose:public menuVariant<T> {
      public:
        choose(const menuNodeShadow& s):menuVariant<T>(s) {}
        result sysHandler(FUNC_PARAMS) override;
        bool changed(const navNode &nav,const menuOut& out) override {
          //Serial<<"choose::changed "<<menuVariant<T>::reflex<<"=="<<menuVariant<T>::target()<<endl;Serial.flush();
          return menuVariant<T>::changed(nav,out)||menuNode::changed(nav,out);
          //return false;
        }
    };

    // Output
    ////////////////////////////////////////////////////////////////////////////

    //navigation panels (min 1)
    //this is a thing of output devices, some might have it
    struct panel {unsigned char x,y,w,h;};

    class menuOut:public Print {
      public:
        //device size (in characters)
        idx_t maxX=80;
        idx_t maxY=24;
        //device resolution (pixels per character)
        idx_t resX=1;
        idx_t resY=1;
        menuNode* drawn;
        menuOut() {}
        menuOut(idx_t x,idx_t y):maxX(x),maxY(y) {}
        virtual menuOut& operator<<(prompt const &p);
        virtual void clearLine(int ln)=0;
        virtual void clear()=0;
        virtual void setCursor(int x,int y);
        virtual void printMenu(navNode &nav)=0;
    };

    // Navigation
    //////////////////////////////////////////////////////////////////////////
    class navNode {
      public:
        idx_t sel=0;
        idx_t top=0;
        menuNode* target;
        static navRoot* root;
        inline menuNodeShadow& shadow() const {return *(menuNodeShadow*)target->shadow;}
        inline idx_t sz() const {return memIdx(shadow().sz);}
        inline prompt* const * data() const {return shadow().data;}
        inline prompt& selected() const {return *(prompt*)memPtr(data()[sel]);}
        inline bool wrap() const {return shadow().style&wrapStyle;}
        /*inline result sysHandler(eventMask event, prompt &item, Stream &in, menuOut &out) const {
          return target->sysHandler(event,*this,item,in,out);
        }*/
        inline result event(eventMask e,idx_t i);//send event to item index i
        inline result event(eventMask e) {return event(e,sel);}//send event to current item
        inline result sysEvent(eventMask e,idx_t i);//send event to item index i
        inline result sysEvent(eventMask e) {return sysEvent(e,sel);}//send event to current item
        navCmds navKeys(char ch);
        void doNavigation(char ch,Stream& in,menuOut& out);
        inline bool changed(const menuOut& out) const {return target->changed(*this,out);}
        inline prompt& operator[](idx_t i) const {return target->operator[](i);}
    };

    class navRoot {
      public:
        menuOut& out;
        Stream& in;
        navNode* path;
        idx_t maxDepth=0;
        idx_t level=0;
        bool suspended=false;
        navTarget* navFocus=NULL;
        navRoot(menuNode& root,navNode* path,idx_t d,Stream& in,menuOut& out)
          :out(out),in(in),path(path),maxDepth(d) {
            navFocus=&root;
            path[0].target=&root;
            navNode::root=this;
            //for(int i=0;i<d;i++) path[i].root=this;
          }
        inline navNode& node() const {return path[level];}
        inline menuNode& active() const {return *node().target;}
        inline prompt& selected() const {return active()[node().sel];}
        void printMenu() const {
          if ((active().sysStyles()&_parentDraw)&&level)
            out.printMenu(path[level-1]);
          else out.printMenu(node());
        }
        inline bool changed() const {return node().changed(out);}
        void poll();
        void enter();
        void exit();
    };

    ////////////////////////////////////////////////////////////////////////
    template<typename T>
    void menuField<T>::printTo(idx_t i,navNode &nav,menuOut& out) {
      menuFieldShadow<T>& s=*(menuFieldShadow<T>*)shadow;
      reflex=target();
      dirty=false;
      prompt::printTo(i,nav,out);
      out<<(this==nav.root->navFocus?(tunning?">":":"):" ");
      out<<reflex;
      print_P(out,(const char*)memPtr(s.units));
    }

    static const char* numericChars="0123456789.";

    template<typename T>
    void menuField<T>::navigate(navNode& nav,char ch,Stream& in,menuOut& out) {
      //Serial<<"menuField<T>::navigate"<<endl;
      menuFieldShadow<T>& s=*(menuFieldShadow<T>*)shadow;
			if (strchr(numericChars,in.peek())) {//a numeric value was entered
      	target()=(T)in.parseFloat();
        //Serial<<"new parsed value:"<<target()<<endl;
    		tunning=false;
				nav.root->exit();
      } else {
			  //char ch=in.read();
        navCmds cmd=nav.navKeys(ch);
        switch(cmd) {
          case enterCmd:
            //Serial<<"enterCmd"<<endl;
            if (tunning||options.nav2D||!tune()) {//then exit edition
  		    		tunning=false;
              dirty=true;
  		    		nav.root->exit();
  		    	} else {
              dirty=true;
              tunning=true;
            }
            break;
          case upCmd:
            //Serial<<"upCmd"<<endl;
            target()+=(tunning?tune():step())*(options.invertFieldKeys?-1:1);
            break;
          case downCmd:
            //Serial<<"downCmd"<<endl;
            target()-=(tunning?tune():step())*(options.invertFieldKeys?-1:1);;
            break;
          default:break;
        }
				if (ch==options.getCmdChar(enterCmd)&&!tunning) {
          nav.event(enterEvent);
	      }
		  }
      //Serial<<"value:"<<s.value<<endl;
			clamp();
    }

    template<typename T>
    void toggle<T>::printTo(idx_t i,navNode &nav,menuOut& out) {
      out<<*(prompt*)this;
      idx_t at=menuVariant<T>::sync(menuVariant<T>::sync());
      out<<menuNode::operator[](at);
      prompt::dirty=false;
    }

    template<typename T>
    void menuVariant<T>::printTo(idx_t i,navNode &nav,menuOut& out) {
      out<<*(prompt*)this;
      idx_t at=menuVariant<T>::sync(menuVariant<T>::sync());
      out<<(this==&nav.root->active()?":":" ");
      out<<menuNode::operator[](at);
      prompt::dirty=false;
    }

    template<typename T>
    void menuVariant<T>::navigate(navNode& nav,char ch,Stream& in,menuOut& out) {
      //Serial<<"menuVariant::navigate"<<endl;Serial.flush();
      nav.sel=sync();
      nav.doNavigation(ch,in,out);
      //Serial<<"sel:"<<nav.sel<<endl;
      sync(nav.sel);
      if (ch==options.navCodes[enterCmd].ch) nav.root->exit();
    }

    template<typename T>
    result choose<T>::sysHandler(FUNC_PARAMS) {
      //Serial<<*(prompt*)this<<*(prompt*)nav.target<<" choose<T>::sysHandler "<<event<<endl;
      switch(event) {
        case enterEvent:
          //Serial<<"just entered "<<*(prompt*)nav.target<<endl;
          nav.sel=menuVariant<T>::sync();
        default:
          return proceed;
      }
    }

  }//namespace Menu

#endif
