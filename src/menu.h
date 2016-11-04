/* -*- C++ -*- */
/********************
Sept 2014 ~ Sept. 2016 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

menu library objects
definitions and enumerations

www.r-site.net

***/

#ifndef RSITE_ARDUINO_MENU_SYSTEM
  #define RSITE_ARDUINO_MENU_SYSTEM
  #include <Arduino.h>
  //#include <Stream.h>
  #include "menuBase.h"
  #include "shadows.h"

  #ifdef DEBUG
  #define DEBUG_LED A4
  #endif

  namespace Menu {


    // Menu objects and data
    //////////////////////////////////////////////////////////////////////////
    class prompt {
      friend class navNode;
      friend class menuOut;
      protected:
        const promptShadow* shadow;//constant read-only data (PROGMEM)
      public:
        status enabled=enabledStatus;//ignore enter if false
        bool dirty=true;//needs to be  redrawn
        inline prompt(const promptShadow& shadow):shadow(&shadow) {}
        inline const char* getText() const {return shadow->getText();}
        inline const systemStyles sysStyles() const {return shadow->_sysStyles();}
        inline const eventMask events() const {return shadow->_events();}
        inline styles style() const {return shadow->_style();}
        inline bool canNav() const {return sysStyles()&_canNav;}//can receive navigation focus and process keys
        inline bool isMenu() const {return sysStyles()&_menuData;}//has menu data list and can be a navNode target
        inline bool isVariant() const {return sysStyles()&_isVariant;}//a menu as an enumerated field, connected to a variable value
        inline bool parentDraw() const {return sysStyles()&_parentDraw;}//a menu as an enumerated field, connected to a variable value
        virtual idx_t printTo(navRoot &root,bool sel,menuOut& out,idx_t len);//raw print to output device
        virtual bool changed(const navNode &nav,const menuOut& out,bool sub=true) {return dirty;}
        //this is the system version of enter handler, its used by elements like toggle
        virtual result sysHandler(FUNC_PARAMS) {return proceed;}
        inline result operator()(FUNC_PARAMS) const {return (*shadow)(FUNC_VALUES);}
        idx_t printRaw(menuOut& out,idx_t len) const;
    };

    //--------------------------------------------------------------------------
    // can receive navigation focus and process keys
    class navTarget:public prompt {
      public:
        navTarget(const promptShadow& shadow):prompt(shadow) {}
        //bool canNav() const override {return true;}
        virtual void parseInput(navNode& nav,Stream& in);
        virtual void doNav(navNode& nav,navCmd cmd);
    };

    //--------------------------------------------------------------------------
    class menuNode:public navTarget {
      public:
        menuNode(const menuNodeShadow& s):navTarget(s) {}
        inline prompt& operator[](idx_t i) const {return ((menuNodeShadow*)shadow)->operator[](i);}
        bool changed(const navNode &nav,const menuOut& out,bool sub=true) override;
        inline idx_t sz() const {return ((menuNodeShadow*)shadow)->_sz();}
        inline prompt* const* data() const {return ((menuNodeShadow*)shadow)->_data();}
    };

    //--------------------------------------------------------------------------
    class menu:public menuNode {
      public:
        menu(const menuNodeShadow& shadow):menuNode(shadow) {}
    };

    //--------------------------------------------------------------------------
    template<typename T>
    class menuField:public navTarget {
      public:
        bool tunning=false;
        T reflex;
        menuField(const menuFieldShadow<T> & shadow):navTarget(shadow) {}
        void parseInput(navNode& nav,Stream& in) override;
        void doNav(navNode& nav,navCmd cmd) override;
        idx_t printTo(navRoot &root,bool sel,menuOut& out,idx_t len) override;
        inline T& target() const {return ((menuFieldShadow<T>*)shadow)->target();}
        inline const char* units() {return ((menuFieldShadow<T>*)shadow)->_units();}
        inline T getTypeValue(const T* from) const {return ((menuFieldShadow<T>*)shadow)->getTypeValue(from);}
        inline T low() const {return  ((menuFieldShadow<T>*)shadow)->_low();}
        inline T high() const {return ((menuFieldShadow<T>*)shadow)->_high();}
        inline T step() const {return ((menuFieldShadow<T>*)shadow)->_step();}
        inline T tune() const {return ((menuFieldShadow<T>*)shadow)->_tune();}
        bool changed(const navNode &nav,const menuOut& out,bool sub=true) override {
          return dirty||(reflex!=target());
        }
        void clamp() {
          if (target()<low()) {
            if (style()&wrapStyle) target()=high();
            else target()=low();
          } else if (target()>high()) {
            if (style()&wrapStyle) target()=low();
            else target()=high();
          }
        }
    };

    //--------------------------------------------------------------------------
    template<typename T>
    class menuValue:public prompt {
      public:
        menuValue(const menuValueShadow<T>& shadow):prompt(shadow) {}
        #ifdef DEBUG
        bool changed(const navNode &nav,const menuOut& out,bool sub=true) override {return false;}
        #endif
        //inline T getTypeValue(const T* from) const {return &((menuValueShadow<T>*)shadow)->getTypeValue(from);}
        inline T target() const {return ((menuValueShadow<T>*)shadow)->target();}
    };

    //--------------------------------------------------------------------------
    template<typename T>//-------------------------------------------
    class menuVariant:public menuNode {
      public:
        T reflex;
        menuVariant(const menuNodeShadow& s):menuNode(s) {}
        idx_t sync() {
          //menuVariantShadow<T>& s=*(menuVariantShadow<T>*)shadow;
          for(idx_t i=0;i<sz();i++) {
            if (((menuValue<T>*)&operator[](i))->target()==target()) {
              dirty=true;
              return i;
            }
          }
          #ifdef DEBUG
          Serial<<F("value out of range ")<<target()<<endl;Serial.flush();
          assert(false);
          #endif
          return -1;
        }
        idx_t sync(idx_t i) {
          //menuVariantShadow<T>& s=*(menuVariantShadow<T>*)shadow;
          #ifdef DEBUG
          if (!(i>=0&&i<sz())){
            Serial<<*(prompt*)this<<F(" : value out of range ")<<i<<endl;
          }
          assert(i>=0&&i<sz());
          #endif
          target()=reflex=((menuValue<T>*)&operator[](i))->target();
          return i;
        }
        inline T& target() const {return ((menuVariantShadow<T>*)shadow)->target();}
        bool changed(const navNode &nav,const menuOut& out,bool sub=true) override;
        //void parseInput(navNode& nav,Stream& in) override;
        void doNav(navNode& nav,navCmd cmd) override;
        idx_t printTo(navRoot &root,bool sel,menuOut& out,idx_t len) override;
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
        idx_t printTo(navRoot &root,bool sel,menuOut& out,idx_t len) override;
        //bool canNav() const override {return false;}//can receive navigation focus and process keys
        result sysHandler(FUNC_PARAMS) override {
          switch(event) {
            case activateEvent: {
              //menuNodeShadow& s=*(menuNodeShadow*)prompt::shadow;
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
          //return proceed;
        }
    };

    template<typename T>//-------------------------------------------
    class choose:public menuVariant<T> {
      public:
        choose(const menuNodeShadow& s):menuVariant<T>(s) {}
        result sysHandler(FUNC_PARAMS) override;
        bool changed(const navNode &nav,const menuOut& out,bool sub=true) override {
          return menuVariant<T>::changed(nav,out)||menuNode::changed(nav,out);
        }
    };

    // Output
    ////////////////////////////////////////////////////////////////////////////

    //navigation panels (min 1) describe output dimensions (in characters)
    struct panel {
      idx_t x,y,w,h;
      inline idx_t maxX() const {return x+w;}
      inline idx_t maxY() const {return y+h;}
    };

    class panelsList {
      public:
        const panel* panels;
        navNode** nodes;//TODO use navNode here!
        const idx_t sz;
        idx_t cur=0;
        panelsList(const panel p[],navNode* nodes[],idx_t sz):panels(p),nodes(nodes),sz(sz) {
          reset();
        }
        void reset(idx_t from=0) {
          for(int n=from;n<sz;n++) nodes[n]=NULL;
        }
        inline const panel operator[](idx_t i) const {
          assert(i<sz);
          #ifdef USING_PGM
            panel tmp;
            memcpy_P(&tmp, &panels[i], sizeof(panel));
            return tmp;
          #else
            return panels[i];
          #endif
        }
        idx_t maxX() const {
          idx_t r=0;
          for(int n=0;n<sz;n++) r=max(operator[](n).maxX(),r);
          return r;
        }
        idx_t maxY() const {
          idx_t r=0;
          for(int n=0;n<sz;n++) r=max(operator[](n).maxY(),r);
          return r;
        }
    };

    class menuOut:public Print {
      public:
        idx_t* tops;
        panelsList& panels;
        idx_t lastSel=-1;
        //TODO: turn this bool's into bitfield flags
        enum styles {none=0<<0,redraw=1<<0,minimalRedraw=1<<1, drawNumIndex=1<<2, usePreview=1<<3} style;

        /*bool redraw=false;//redraw all menu every cycle, some display drivers require it
        bool minimalRedraw=true;//redraw only changed options (avoids flicking on LCDS), not good for Serial
        bool drawNumIndex=false;
        bool usePreview=false;*/
        menuNode* drawn=NULL;
        menuOut(idx_t *topsList,panelsList &p,styles os=minimalRedraw)
          :tops(topsList),panels(p),style(os) {}
        inline idx_t maxX(idx_t i=0) const {return panels[i].w;}
        inline idx_t maxY(idx_t i=0) const {return panels[i].h;}
        inline idx_t& top(navNode& nav) const;
        idx_t printRaw(const char* at,idx_t len);
        virtual menuOut& operator<<(prompt const &p);
        virtual menuOut& fill(
          int x1, int y1, int x2, int y2,char ch=' ',
          colorDefs color=bgColor,
          bool selected=false,
          status stat=enabledStatus,
          bool edit=false
        ) {return *this;}
        void previewMenu(navRoot& root,menuNode& menu,idx_t panelNr);//draw a preview on a panel
        void printMenu(navNode &nav);//print menus and previews on panels
        void clearChanged(navNode &nav);//clean up changed flags after everyone printed!
        virtual void clearLine(idx_t ln,idx_t panelNr=0,colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false)=0;
        virtual void clear()=0;
        virtual void clear(idx_t panelNr)=0;
        virtual void setCursor(idx_t x,idx_t y,idx_t panelNr=0)=0;
        virtual void setColor(colorDefs c,bool selected=false,status s=enabledStatus,bool edit=false) {}
        virtual void drawCursor(idx_t ln,bool selected,status stat,bool edit=false,idx_t panelNr=0) {
          setColor(cursorColor, selected, stat,edit);
          write(selected?(stat==disabledStatus?options->disabledCursor:options->selectedCursor):' ');
        }
        void doNav(navCmd cmd,navNode &nav);
        enum fmtParts {fmtPanel,fmtTitle,fmtBody,fmtOp,fmtIdx,fmtCursor,fmtOpBody,fmtPreview};
        virtual result fmtStart(fmtParts part,navNode &nav,idx_t panelNr,idx_t idx=-1) {}
        virtual result fmtEnd(fmtParts part,navNode &nav,idx_t panelNr,idx_t idx=-1) {}
      protected:
        void printMenu(navNode &nav,idx_t panelNr);
    };

    class gfxOut:public menuOut {
      public:
        idx_t resX=1;
        idx_t resY=1;
        idx_t fontMarginY=1;//in pixels, compensate vertical font alignment
        gfxOut(idx_t rx,idx_t ry,idx_t* t,panelsList &p,menuOut::styles st=menuOut::redraw)
          :menuOut(t,p,st),resX(rx),resY(ry) {}
        //void printMenu(navNode &nav) override;
    };

    //list of output devices
    //this allows parallel navigation on multiple devices
    class outputsList {
      public:
        int cnt=1;
        menuOut* const* outs;
        outputsList(menuOut* const o[],int n):cnt(n),outs(o) {}
        menuOut& operator[](idx_t i) {
          assert(i<cnt);
          return *(menuOut*)memPtr(outs[i]);
        }
        void printMenu(navNode& nav) const {
          for(int n=0;n<cnt;n++)
            ((menuOut*)memPtr(outs[n]))->printMenu(nav);
          clearChanged(nav);
        }
        //void alert(char *msg) const {for(int n=0;n<cnt;n++) ((menuOut*)memPtr(outs[n]))->alert(msg);}
        void clearLine(idx_t ln,idx_t panelNr=0,colorDefs color=bgColor,bool selected=false,status stat=enabledStatus) const {
          for(int n=0;n<cnt;n++) ((menuOut*)memPtr(outs[n]))->clearLine(ln,panelNr,color,selected,stat);
        }
        void clearChanged(navNode& nav) const {
          for(int n=0;n<cnt;n++) ((menuOut*)memPtr(outs[n]))->clearChanged(nav);
        }
        void clear() {for(int n=0;n<cnt;n++) ((menuOut*)memPtr(outs[n]))->clear();}
        void setCursor(idx_t x,idx_t y) {
          for(int n=0;n<cnt;n++) ((menuOut*)memPtr(outs[n]))->setCursor(x,y);
        }
        void setColor(colorDefs c,bool selected=false,status s=enabledStatus) {
          for(int n=0;n<cnt;n++) ((menuOut*)memPtr(outs[n]))->setColor(c,selected,s);
        }
        void drawCursor(idx_t ln,bool selected,status stat,idx_t panelNr=0) {
          for(int n=0;n<cnt;n++) ((menuOut*)memPtr(outs[n]))->drawCursor(ln,selected,stat,panelNr);
        }
        void doNav(navCmd cmd,class navNode &nav) {for(int n=0;n<cnt;n++) ((menuOut*)memPtr(outs[n]))->doNav(cmd,nav);}
        result idle(idleFunc f,idleEvent e) {
          for(int n=0;n<cnt;n++) {
            menuOut& o=*((menuOut*)memPtr(outs[n]));
            switch(e) {
              case idleStart:
                if ((*f)(o,e)==proceed) {
                  if (!o.redraw) {
                    o.clear();//reset the coordinates and colors
                    result r=(*f)(o,idling);
                    if (r==quit) return r;
                  }
                } else return quit;
                break;
              case idling:
                if (o.redraw) {
                  o.clear();//reset the coordinates and colors
                  result r=(*f)(o,e);
                  if (r==quit) return r;
                }
                break;
              case idleEnd:
                result r=(*f)(o,e);
                if (r==quit) return r;
                break;
            }
          }
          return proceed;
        }
    };

    // input
    ////////////////////////////////////////////////////////////////////////////

    // Navigation
    //////////////////////////////////////////////////////////////////////////
    class navNode {
      protected:
        inline menuNodeShadow& shadow() const {return *(menuNodeShadow*)target->shadow;}
      public:
        idx_t sel=0;
        menuNode* target;
        static navRoot* root;
        inline idx_t sz() const {return target->sz();}
        inline prompt* const * data() const {return target->data();}
        inline prompt& selected() const {return *(prompt*)memPtr(data()[sel]);}
        inline bool wrap() const {return target->style()&wrapStyle;}
        /*inline result sysHandler(eventMask event, prompt &item, Stream &in, menuOut &out) const {
          return target->sysHandler(event,*this,item,in,out);
        }*/
        result event(eventMask e,idx_t i);//send event to item index i
        result event(eventMask e) {return event(e,sel);}//send event to current item
        result sysEvent(eventMask e,idx_t i);//send event to item index i
        inline result sysEvent(eventMask e) {return sysEvent(e,sel);}//send event to current item
        navCmd navKeys(char ch);
        //inline void doNav(navCmd cmd) {target->doNav(*this,cmd);}
        navCmd doNavigation(navCmd cmd);//aux function
        inline bool changed(const menuOut& out) const {return target->changed(*this,out);}
        inline prompt& operator[](idx_t i) const {return target->operator[](i);}
    };

    class navRoot {
      public:
        outputsList &out;
        Stream& in;
        navNode* path;
        idx_t maxDepth=0;
        idx_t level=0;
        //bool suspended=false;
        bool showTitle=true;
        //bool sleeping=false;//when sleeping poll will simply return
        idleFunc idleTask=inaction;//to do when menu exits
        idleFunc sleepTask=NULL;//user task suspending menu
        navTarget* navFocus=NULL;
        navRoot(menuNode& root,navNode* path,idx_t d,Stream& in,outputsList &o)
          :out(o),in(in),path(path),maxDepth(d) {
            navFocus=&root;
            path[0].target=&root;
            navNode::root=this;
            //for(int i=0;i<d;i++) path[i].root=this;
          }
        inline navNode& node() const {return path[level];}
        inline menuNode& active() const {return *node().target;}
        inline prompt& selected() const {return active()[node().sel];}
        inline bool changed(const menuOut& out) const {return node().changed(out);}
        inline bool changed(idx_t n) const {return node().changed(out[n]);}
        void printMenu() const {
          if ((active().sysStyles()&_parentDraw)&&level)
            out.printMenu(path[level-1]);
          else out.printMenu(node());
        }

        //menu IO - external iteration functions
        void doInput();
        inline void doOutput() {if (!sleepTask) printMenu();}
        inline void poll() {doInput();doOutput();};//fire and forget mode
        void doNav(navCmd cmd);//fly by wire mode
        navCmd enter();//aux function
        navCmd exit();//aux function

        //enter leva idle mode ---------------------------------
        inline void idleOn(idleFunc task=inaction) {
          out.clear();
          sleepTask=task;
          active().dirty=true;
          out.idle(sleepTask,idleStart);
        }
        inline void idleOff() {
          out.idle(sleepTask,idleEnd);
          sleepTask=NULL;
          active().dirty=true;
          out.clear();
        }
        //inline void alert(char *msg,bool modal=true) {out.alert(msg);}
    };

    ////////////////////////////////////////////////////////////////////////
    template<typename T>
    idx_t menuField<T>::printTo(navRoot &root,bool sel,menuOut& out,idx_t len) {
      //menuFieldShadow<T>& s=*(menuFieldShadow<T>*)shadow;
      reflex=target();
      idx_t l=prompt::printTo(root,sel,out,len);
      bool ed=this==root.navFocus;
      //bool sel=nav.sel==i;
      if (l<len) {
        out<<((root.navFocus==this&&sel)?(tunning?'>':':'):' ');
        l++;
        if (l<len) {
          out.setColor(valColor,sel,enabled,ed);
          //out<<reflex;
          l+=out.print(reflex);//NOTE: this can exceed the limits!
          if (l<len) {
            out.setColor(unitColor,sel,enabled,ed);
            l+=print_P(out,units(),len);
          }
        }
      }
      return l;
    }

    static const char* numericChars="0123456789.";

    template<typename T>
    void menuField<T>::parseInput(navNode& nav,Stream& in) {
      //menuFieldShadow<T>& s=*(menuFieldShadow<T>*)shadow;
      if (strchr(numericChars,in.peek())) {//a numeric value was entered
        target()=(T)in.parseFloat();
        tunning=false;
        nav.root->exit();
        doNav(nav,noCmd);//just clamping
      } else doNav(nav,nav.navKeys(in.read()));
    }

    template<typename T>
    void menuField<T>::doNav(navNode& nav,navCmd cmd) {
      switch(cmd.cmd) {
        case enterCmd:
          if (tunning||options->nav2D||!tune()) {//then exit edition
            tunning=false;
            dirty=true;
            nav.root->exit();
            nav.event(enterEvent);
          } else tunning=true;
          dirty=true;
          break;
        case upCmd:
          target()+=(tunning?tune():step())*(options->invertFieldKeys?-1:1);
          dirty=true;
          break;
        case downCmd:
          target()-=(tunning?tune():step())*(options->invertFieldKeys?-1:1);;
          dirty=true;
          break;
        default:break;
      }
      /*if (ch==options->getCmdChar(enterCmd)&&!tunning) {
        nav.event(enterEvent);
      }*/
      clamp();
    }

    template<typename T>
    idx_t toggle<T>::printTo(navRoot &root,bool sel,menuOut& out,idx_t len) {
      idx_t l=prompt::printTo(root,sel,out,len);
      idx_t at=menuVariant<T>::sync(menuVariant<T>::sync());
      bool ed=this==root.navFocus;
      //bool sel=nav.sel==i;
      out.setColor(valColor,sel,prompt::enabled,ed);
      //out<<menuNode::operator[](at);
      if (len-l>0) l+=toggle<T>::operator[](at).printRaw(out,len-l);
      return l;
    }

    template<typename T>
    idx_t menuVariant<T>::printTo(navRoot &root,bool sel,menuOut& out,idx_t len) {
      idx_t l=prompt::printTo(root,sel,out,len);
      idx_t at=menuVariant<T>::sync(menuVariant<T>::sync());
      bool ed=this==root.navFocus;
      //bool sel=nav.sel==i;
      out<<(this==&root.active()?':':' ');
      l++;
      out.setColor(valColor,sel,prompt::enabled,ed);
      //out<<menuNode::operator[](at);
      if (len-l>0) l+=operator[](at).printRaw(out,len-l);
      return l;
    }

    /*template<typename T>
    void menuVariant<T>::parseInput(navNode& nav,Stream& in) {
      char ch=in.read();*/

    template<typename T>
    void menuVariant<T>::doNav(navNode& nav,navCmd cmd) {
      //Serial<<"variant::doNav "<<cmd.cmd<<endl;
      nav.sel=sync();
      navCmd c=nav.doNavigation(cmd);
      sync(nav.sel);
      if (c.cmd==enterCmd) {
        //Serial<<"variant:enter!"<<endl;
        //Serial<<"exit variant"<<endl;
        nav.root->exit();
      }
    }

    template<typename T>
    bool menuVariant<T>::changed(const navNode &nav,const menuOut& out,bool sub) {
      return dirty||reflex!=target();
    }

    template<typename T>
    result choose<T>::sysHandler(FUNC_PARAMS) {
      switch(event) {
        case enterEvent:
          nav.sel=menuVariant<T>::sync();
        default:
          return proceed;
      }
    }

    idx_t& menuOut::top(navNode& nav) const {return tops[nav.root->level];}

  }//namespace Menu

#endif
