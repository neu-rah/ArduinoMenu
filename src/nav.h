/* -*- C++ -*- */
#ifndef RSITE_ARDUINO_MENU_SYSTEM_NAV
  #define RSITE_ARDUINO_MENU_SYSTEM_NAV
  #include "menuBase.h"
  #include "menuIo.h"
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
        const panel* panels;
        navNode** nodes;
        constMEM idx_t sz;
        idx_t cur=0;
        inline panelsList(const panel p[],navNode* nodes[],idx_t sz):panels(p),nodes(nodes),sz(sz) {
          reset();
        }
        void reset(idx_t from=0);
        inline constMEM panel operator[](idx_t i) const {
          // assert(i<sz);
          #ifdef USING_PGM
            panel tmp;
            memcpy_P(&tmp, &panels[i], sizeof(panel));
            return tmp;
          #else
            return panels[i];
          #endif
        }
        idx_t maxX() const;
        idx_t maxY() const;
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
        inline result event(eventMask e) {return event(e,sel);}//send event to current item
        result sysEvent(eventMask e,idx_t i);//send system event to item index i
        inline result sysEvent(eventMask e) {return sysEvent(e,sel);}//send event to current item
        navCmd navKeys(char ch);
        navCmd doNavigation(navCmd cmd);//aux function
        // inline int getPanelIdx(menuOut& out) const {return out.getPanelIdx(*this);}
        bool changed(const menuOut& out) const;
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
        idleFunc idleTask=inaction;//to do when menu exits, menu system will set sleepTask to this on exit
        idleFunc sleepTask=NULL;//menu suspended, call this function
        navTarget* navFocus=NULL;
        bool nav2D=false;
        bool canExit=true;//v4.0 moved from global options
        bool useUpdateEvent=false;//if false, when field value is changed use enterEvent instead.
        idx_t inputBurst=1;//limit of inputs that can be processed before output
        unsigned long lastChanged=0;//last change detected (can be external activity)
        unsigned int timeOut=0;//enter idle mode after `timeOut` seconds of inactivity
        bool useAccel=true;//use numeric accelerators to send idxCmd
        inline navRoot(menuNode& root,navNode* path,idx_t d,menuIn& in,outputsList &o)
          :out(o),in(in),path(path),maxDepth(d-1) {
            useMenu(root);
            initPath(d-1);
          }
        void initPath(idx_t d);
        void useMenu(menuNode &menu);
        inline void reset() {
          level=0;
          navFocus=path[level].target;
          path[level].reset();
          refresh();
        }
        inline void refresh() {out.refresh();}
        inline navNode& node() const {return path[level];}
        inline menuNode& active() const {return *node().target;}
        inline prompt& selected() const {return active()[node().sel];}
        bool changed(const menuOut& out);
        inline bool changed(idx_t n) {return changed(out[n]);}
        #ifdef MENU_ASYNC
          idx_t escTo(idx_t lvl);
          // prompt* seek(idx_t* uri,idx_t len);
          bool async(const char* at);
          menuOut& printPath(menuOut& o,menuNode*) const;
          Used printMenu(menuOut& o) const;
        #endif
        Used printMenu() const;
        //menu IO - external iteration functions
        void doInput(menuIn& in);
        #ifdef MENU_ASYNC
          inline void doInput(const char*in) {
            StringStream inStr(in);
            while(inStr.available()) doInput(inStr);
          }
        #endif
        inline void doInput() {doInput(in);}
        void doOutput();
        inline void poll() {doInput();doOutput();};//fire and forget mode
        void doNav(navCmd cmd);//fly by wire mode
        navCmd enter();//aux function
        navCmd exit();//aux function

        //enter idle mode ---------------------------------
        void idleOn(idleFunc task=inaction);
        void idleOff();
    };

    #ifdef MENU_DEBUG
      inline Stream& operator<<(Stream&o,const navNode& p) {
        o.print((__FlashStringHelper*)p.target->getText());
        return o;
      }
      inline Stream& operator<<(Stream&o,const navRoot& p) {return o<<p.node();}
    #endif

  }//namespace Menu

#endif
