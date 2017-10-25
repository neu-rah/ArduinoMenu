/* -*- C++ -*- */
#ifndef RSITE_ARDUINO_MENU_SYSTEM_NAV
  #define RSITE_ARDUINO_MENU_SYSTEM_NAV
  #include "menuBase.h"
  #include "io.h"
  namespace Menu {

    //navigation panels (min 1) describe output dimensions (in characters)
    struct panel {
      idx_t x,y,w,h;
      inline idx_t maxX() const {return x+w;}
      inline idx_t maxY() const {return y+h;}
    };

    //multiple panels
    class panelsList {
      public:
        constMEM panel* panels;
        navNode** nodes;
        constMEM idx_t sz;
        idx_t cur=0;
        panelsList(constMEM panel p[],navNode* nodes[],idx_t sz):panels(p),nodes(nodes),sz(sz) {
          reset();
        }
        void reset(idx_t from=0) {
          for(int n=from;n<sz;n++) nodes[n]=NULL;
        }
        inline constMEM panel operator[](idx_t i) const {
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
          for(int n=0;n<sz;n++) r=_MAX(operator[](n).maxX(),r);
          return r;
        }
        idx_t maxY() const {
          idx_t r=0;
          for(int n=0;n<sz;n++) r=_MAX(operator[](n).maxY(),r);
          return r;
        }
    };

    // Navigation
    //////////////////////////////////////////////////////////////////////////
    class navNode {
      friend class navRoot;
      protected:
        inline menuNodeShadow& shadow() const {return *(menuNodeShadow*)target->shadow;}
        inline void useMenu(menuNode &menu) {target=&menu;reset();}
      public:
        idx_t sel=0;
        menuNode* target;
        /*static*/ navRoot* root;//v 4.0 removed static to allow multiple menus
        inline void reset() {sel=0;}
        inline idx_t sz() const {return target->sz();}
        inline prompt* constMEM * data() const {return target->data();}
        inline prompt& selected() const {return *(prompt*)memPtr(data()[sel]);}
        inline bool wrap() const {return target->style()&wrapStyle;}
        result event(eventMask e,idx_t i);//send event to item index i
        result event(eventMask e) {return event(e,sel);}//send event to current item
        result sysEvent(eventMask e,idx_t i);//send system event to item index i
        inline result sysEvent(eventMask e) {return sysEvent(e,sel);}//send event to current item
        navCmd navKeys(char ch);
        navCmd doNavigation(navCmd cmd);//aux function
        inline bool changed(const menuOut& out) const {return out.drawn==NULL||target->changed(*this,out);}
        inline prompt& operator[](idx_t i) const {return target->operator[](i);}

        inline bool is(systemStyles chk)  const {return target->is(chk);}
        inline bool has(systemStyles chk) const {return target->has(chk);}
        inline bool is(styles chk)        const {return target->is(chk);}
        inline bool has(styles chk)       const {return target->has(chk);}
        inline bool is(eventMask chk)     const {return target->is(chk);}
        inline bool has(eventMask chk)    const {return target->has(chk);}
    };

    class navRoot {
      public:
        outputsList &out;
        menuIn& in;
        navNode* path;
        constMEM idx_t maxDepth=0;
        idx_t level=0;
        bool showTitle=true;
        bool idleChanged=false;//does idle screen need refresh?
        idleFunc idleTask=inaction;//to do when menu exits, menu system will set idleFunc to this on exit
        idleFunc sleepTask=NULL;//menu suspended, call this function
        navTarget* navFocus=NULL;
        bool nav2D=false;
        bool canExit=true;//v4.0 moved from global options
        bool useUpdateEvent=false;//if false, when field value is changed use enterEvent instead.
        idx_t inputBurst=1;//limit of inputs that can be processed before output
        navRoot(menuNode& root,navNode* path,idx_t d,menuIn& in,outputsList &o)
          :out(o),in(in),path(path),maxDepth(d-1) {
            useMenu(root);
            initPath(d-1);
          }
        void initPath(idx_t d) {
          for(idx_t n=0;n<=d;n++)//initialize path chain for this root (v4.0)
            path[n].root=this;
        }
        void useMenu(menuNode &menu) {
          navFocus=&menu;
          path[0].target=&menu;
          reset();
          refresh();
        }
        inline void reset() {
          level=0;
          path[0].sel=0;
        }
        inline void refresh() {out.refresh();}
        inline navNode& node() const {return path[level];}
        inline menuNode& active() const {return *node().target;}
        inline prompt& selected() const {return active()[node().sel];}
        inline bool changed(const menuOut& out) const {
          return sleepTask?idleChanged:node().changed(out);
        }
        inline bool changed(idx_t n) const {return changed(out[n]);}
        #ifdef MENU_ASYNC
          inline bool async(const char* at) {
            navFocus=path[level].target;
            return active().async(at, *this, 0);
          }
          menuOut& printPath(menuOut& o) const {
            for(idx_t n=0;n<level;n++) {
              o.print('/');
              o.print(path[n].sel);
            }
            return o;
          }
          //async printMenu on arbitrary menuOut device
          Used printMenu(menuOut& o) const {
            trace(Serial<<"navRoot::printMenu(menuOut& o)"<<endl);
            if ((active().sysStyles()&_parentDraw)&&level)
              return o.printMenu(path[level-1]);
            else return o.printMenu(node());
          }
        #endif
        Used printMenu() const {
          trace(Serial<<"navRoot::printMenu"<<endl);
          if ((active().sysStyles()&_parentDraw)&&level)
            return out.printMenu(path[level-1]);
          else return out.printMenu(node());
        }

        //menu IO - external iteration functions
        void doInput(menuIn& in);
        #ifdef MENU_ASYNC
          inline void doInput(const char*in) {
            StringStream inStr(in);
            while(inStr.available()) doInput(inStr);
          }
        #endif
        inline void doInput() {doInput(in);}
        inline void doOutput() {
          if (!sleepTask) printMenu();
          else {
            idleChanged=false;//turn it off here so that sleepTask can force it on again
            out.idle(sleepTask,idling);
          }
        }
        inline void poll() {doInput();doOutput();};//fire and forget mode
        void doNav(navCmd cmd);//fly by wire mode
        navCmd enter();//aux function
        navCmd exit();//aux function

        //enter idle mode ---------------------------------
        inline void idleOn(idleFunc task=inaction) {
          out.clear();
          sleepTask=task;
          idleChanged=true;
          active().dirty=true;
          out.idle(sleepTask,idleStart);
        }
        inline void idleOff() {
          out.idle(sleepTask,idleEnd);
          sleepTask=NULL;
          active().dirty=true;
          out.clear();
        }
    };

    #ifdef DEBUG
      inline Stream& operator<<(Stream&o,const navNode& p) {
        o.print((__FlashStringHelper*)p.target->getText());
        return o;
      }
      inline Stream& operator<<(Stream&o,const navRoot& p) {return o<<p.node();}
    #endif

  }//namespace Menu

#endif
