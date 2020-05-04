/* -*- C++ -*- */
#ifndef RSITE_ARDUINO_MENU_SYSTEM_IO
  #define RSITE_ARDUINO_MENU_SYSTEM_IO

  #include "menuBase.h"
  #include "shadows.h"

  namespace Menu {
    //inline menuOut::styles operator | (menuOut::styles a, menuOut::styles b) {return (menuOut::styles)(a|b);}

    ///////////////////////////////////////////////////////////////////////////
    // base for all menu input devices
    class menuIn:public Stream {
      public:
        bool numValueInput=true;
        //when inside a field some devices might invert the movement axis
        virtual void setFieldMode(bool);
        virtual bool fieldMode() const;
        inline void fieldOn() {setFieldMode(true);}
        inline void fieldOff() {setFieldMode(false);}
        void flush() {}
    };

    class noInput:public menuIn {
    public:
      size_t write(uint8_t) override;
      int available() override;
      int read() override;
      int peek() override;
    };

    #ifdef MENU_ASYNC
      class StringStream:public menuIn {
        public:
          const char *src;
          inline StringStream(const char*s):src(s) {}
          int available() override;
          int read() override;
          int peek() override;
          void flush() override;
          size_t write(uint8_t) override;
          inline operator const String() {return String(src);}
      };
    #endif

    ///////////////////////////////////////////////////////////////////////////
    // base for all menu output devices
    class menuOut:public Print {
      friend class prompt;
      public:
        idx_t* tops;
        panelsList& panels;
        // idx_t lastSel=-1;
        enum styles {
          none=0<<0, // default serialOut
          redraw=1<<0,// changing one part implies printing all visible (deprecated)
          minimalRedraw=1<<1, // can draw part of the screen
          drawNumIndex=1<<2, // numerical index should be displayed
          usePreview=1<<3, // reserve one panel for preview
          expandEnums=1<<4, // enumerated typed should be printed along with the option (web interface)
          rasterDraw=1<<5 // can request multiple draws to complete a screen
        } style;
        enum fmtParts {
          fmtPanel,fmtTitle,fmtBody,fmtOp,
          fmtIdx,fmtCursor,fmtOpBody,fmtPreview,
          fmtPrompt,fmtField,fmtToggle,fmtSelect,
          fmtChoose,fmtUnit,fmtTextField,fmtCursorOpen,
          fmtCursorClose,fmtValue,fmtEditCursor
        };
        menuNode* drawn=NULL;
        bool pageScroll=false;
        inline menuOut(idx_t *topsList,panelsList &p,styles os=minimalRedraw)
          :tops(topsList),panels(p),style(os) {}
        idx_t maxX(idx_t i=0) const;
        idx_t maxY(idx_t i=0) const;
        idx_t& top(navNode& nav) const;
        // inline void reset() {panelsList.reset();}
        virtual idx_t printRaw(const char* at,idx_t len);
        inline idx_t printRaw(const __FlashStringHelper* at,idx_t len) {
          return printRaw((const char*)at,len);
        }
        idx_t printText(const __FlashStringHelper* at,idx_t len) {return printRaw(at,len);}
        idx_t printText(const char* at,idx_t len);
        #if defined(MENU_DEBUG) || defined(MENU_ASYNC)
          virtual menuOut& operator<<(prompt const &p);
          #ifdef ESP8266
            template<typename T> menuOut& operator<<(T o);
          #endif
        #endif
        virtual menuOut& fill(
          int x1, int y1, int x2, int y2,char ch=' ',
          colorDefs color=bgColor,
          bool selected=false,
          status stat=enabledStatus,
          bool edit=false
        );
        void clearChanged(navNode &nav);//clean up changed flags after everyone printed!
        void previewMenu(navRoot& root,menuNode& menu,idx_t panelNr);//draw a preview on a panel
        Used printMenu(navNode &nav);//print menus and previews on panels
        void doNav(navCmd cmd,navNode &nav);
        virtual void clearLine(idx_t ln,idx_t panelNr=0,colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false)=0;
        virtual void clear()=0;
        virtual void clear(idx_t panelNr)=0;
        virtual void setCursor(idx_t x,idx_t y,idx_t panelNr=0)=0;
        virtual void setColor(colorDefs c,bool selected=false,status s=enabledStatus,bool edit=false);
        virtual void drawCursor(idx_t ln,bool selected,status stat,bool edit=false,idx_t panelNr=0);
        //text editor cursors
        virtual idx_t startCursor(navRoot& root,idx_t x,idx_t y,bool charEdit,idx_t panelNr=0);
        virtual idx_t endCursor(navRoot& root,idx_t x,idx_t y,bool charEdit,idx_t panelNr=0);
        virtual idx_t editCursor(navRoot& root,idx_t x,idx_t y,bool editing,bool charEdit,idx_t panelNr=0);
        virtual void rect(idx_t panelNr,idx_t x,idx_t y,idx_t w=1,idx_t h=1,colorDefs c=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false);
        virtual void box(idx_t panelNr,idx_t x,idx_t y,idx_t w=1,idx_t h=1,colorDefs c=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false);
        #ifdef MENU_FMT_WRAPS
          virtual result fmtStart(prompt& target,fmtParts part,navNode &nav,idx_t idx=-1);
          virtual result fmtEnd(prompt& target,fmtParts part,navNode &nav,idx_t idx=-1);
        #endif
        #ifdef MENU_ASYNC
          virtual bool isAsync();
        #endif
      protected:
        Used printMenu(navNode &nav,idx_t panelNr);
    };

    //for devices that can position a print cursor (like LCD's)
    class cursorOut:public menuOut {
    public:
      inline cursorOut(idx_t *topsList,panelsList &p,styles os=minimalRedraw)
        :menuOut(topsList,p,os) {}
      void clearLine(idx_t ln,idx_t panelNr=0,colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override;
      void clear(idx_t panelNr) override;
      menuOut& fill(int x1, int y1, int x2, int y2,char ch=' ',colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override;
    };

    //base for graphics displays
    class gfxOut:public menuOut {
      public:
        idx_t resX=1;
        idx_t resY=1;
        idx_t fontMarginX=1;//in pixels, compensate vertical font alignment
        idx_t fontMarginY=1;//in pixels, compensate vertical font alignment
        inline gfxOut(idx_t rx,idx_t ry,idx_t* t,panelsList &p,menuOut::styles st=menuOut::minimalRedraw,idx_t fontMarginY=1)
          :menuOut(t,p,st),resX(rx),resY(ry),fontMarginY(fontMarginY) {}
        idx_t startCursor(navRoot& root,idx_t x,idx_t y,bool charEdit,idx_t panelNr) override;
        idx_t endCursor(navRoot& root,idx_t x,idx_t y,bool charEdit,idx_t panelNr) override;
        idx_t editCursor(navRoot& root,idx_t x,idx_t y,bool editing,bool charEdit,idx_t panelNr) override;
    };

    #ifdef MENU_ASYNC
      template<typename O>
      class asyncOut:public O {
        using O::O;
        bool isAsync() override {return true;}
      };
      using webOut=asyncOut<menuOut>;
    #endif

    //list of output devices
    //this allows parallel navigation on multiple devices
    class outputsList {
      public:
        int cnt=1;
        menuOut* constMEM* outs;
        outputsList(menuOut* constMEM o[],int n):cnt(n),outs(o) {}
        inline menuOut& operator[](idx_t i) {
          assert(i<cnt);
          return *(menuOut*)memPtr(outs[i]);
        }
        Used printMenu(navNode& nav) const;
        void refresh();//force redraw of all outputs on next output call
        void clearChanged(navNode& nav) const;
        void clear();
        // void reset();
        void doNav(navCmd cmd,class navNode &nav);
        result idle(idleFunc f,idleEvent e,bool idleChanged=false);
    };
  }//namespace Menu

  #include "nav.h"
  using namespace Menu;
  inline idx_t menuOut::maxX(idx_t i) const {return panels[i].w;}
  inline idx_t menuOut::maxY(idx_t i) const {return panels[i].h;}

#endif
