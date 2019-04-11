/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// ouput internal process
// output drivers are a composition between MenuOut<->MenuOutCap
// delivered as O parameter of MenuOutCap template

#include "menu.h"

namespace AM5 {
  ///////////////////////////////////////////////////////////////////
  // output interface
  class MenuOut {
    friend class Void;
    public:
      virtual MenuOut& operator<<(Item&) {return *this;}
      virtual MenuOut& operator<<(const char*) {return *this;}
      virtual MenuOut& operator<<(char) {return *this;}
      virtual MenuOut& operator<<(unsigned char) {return *this;}
      virtual MenuOut& operator<<(int) {return *this;}
      virtual MenuOut& operator<<(unsigned int) {return *this;}
      #if defined(ARDUINO) && !defined(__AVR_ATtiny13__)
        virtual MenuOut& operator<<(endlObj) {return *this;}
        virtual MenuOut& operator<<(const __FlashStringHelper *i) {return *this;}
      #endif
      virtual void fmtMenu(bool io)=0;
      virtual void fmtPanel(bool io)=0;
      virtual void fmtTitle(bool io)=0;
      virtual void fmtItem(bool io)=0;
      virtual void fmtAccel(bool io)=0;
      virtual void fmtCursor(bool io)=0;
      virtual void fmtLabel(bool io)=0;
      virtual void fmtMode(bool io)=0;
      virtual void fmtValue(bool io)=0;
      virtual void fmtUnit(bool io)=0;
      virtual void printMenu()=0;
      virtual void setTarget(Item& i)=0;
      // virtual Item& getTarget() {return *(Item*)NULL;}
    protected:
      static bool onMenu;
    };

  template<typename O> using asMenu=Role<Roles::AM5,O,&MenuOut::fmtMenu>;
  template<typename O> using asPanel=Role<Roles::Panel,O,&MenuOut::fmtPanel>;
  template<typename O> using asTitle=Role<Roles::Title,O,&MenuOut::fmtTitle>;
  template<typename O> using asItem=Role<Roles::Item,O,&MenuOut::fmtItem>;
  template<typename O> using asAccel=Role<Roles::Accel,O,&MenuOut::fmtAccel>;
  template<typename O> using asCursor=Role<Roles::Cursor,O,&MenuOut::fmtCursor>;
  template<typename O> using asLabel=Role<Roles::Label,O,&MenuOut::fmtLabel>;
  // template<typename O> using asMode=Role<Roles::Mode,O,&MenuOut::fmtMode>;
  template<typename O> using asValue=Role<Roles::Value,O,&MenuOut::fmtValue>;
  template<typename O> using asUnit=Role<Roles::Unit,O,&MenuOut::fmtUnit>;

  template<typename O>
  struct PrintHead {
    O& printer;
    size_t pos;
    size_t line;
  };

  //interestingly we know the exact type of output
  //no need to put them on a list (we use a type level chain)
  //inner calls can have the type because we pass it down the chain
  template<typename O>
  struct MenuOutCap:public MenuOut,public O {
    // using This=MenuOutCap<O>;
    using O::O;
    PrintHead<O> head{*this,0};
    MenuOut& operator<<(Item& i) override;
    MenuOut& operator<<(const char* i) override {O::raw(i);return *this;}
    MenuOut& operator<<(char i) override {O::raw(i);return *this;}
    MenuOut& operator<<(unsigned char i) override {O::raw(i);return *this;}
    MenuOut& operator<<(int i) override {O::raw(i);return *this;}
    MenuOut& operator<<(unsigned int i) override {O::raw(i);return *this;}
    #if defined(ARDUINO) && !defined(__AVR_ATtiny13__)
      MenuOut& operator<<(endlObj) override {O::raw("\n");return *this;}
      MenuOut& operator<<(const __FlashStringHelper * i) override {O::raw(i);return *this;}
    #endif
    void fmtMenu(bool io) override {O::fmtMenu(head,io);}
    void fmtPanel(bool io) override {O::fmtPanel(head,io);}
    void fmtTitle(bool io) override {O::fmtTitle(head,io);}
    void fmtItem(bool io) override {O::fmtItem(head,io);}
    void fmtAccel(bool io) override {O::fmtAccel(head,io);}
    void fmtCursor(bool io) override {O::fmtCursor(head,io);}
    void fmtLabel(bool io) override {O::fmtLabel(head,io);}
    void fmtMode(bool io) override {O::fmtMode(head,io);}
    void fmtValue(bool io) override {O::fmtValue(head,io);}
    void fmtUnit(bool io) override {O::fmtUnit(head,io);}
    void printMenu() override {
      //TODO: install panel browser here
      O::enterMenuRender();
      O::newView();
      O::printMenuRaw(*this,PrintHead<O>{*this,0},O::getTarget());
      O::exitMenuRender();
    }
    void setTarget(Item& i) override {O::setTarget(i);}
  };

  //base for output combinators --------------------------
  struct Void {
    template<typename T> static inline void raw(T) {}//just ignore stuff
    static inline void newView() {}//restart the viewport from the panel definition
    template<typename P> static inline void fmtMenu  (PrintHead<P>,bool io) {}
    template<typename P> static inline void fmtPanel (PrintHead<P>,bool io) {}
    template<typename P> static inline void fmtTitle (PrintHead<P>,bool io) {}
    template<typename P> static inline void fmtItem  (PrintHead<P>,bool io) {}
    template<typename P> static inline void fmtAccel (PrintHead<P>,bool io) {}
    template<typename P> static inline void fmtCursor(PrintHead<P>,bool io) {}
    template<typename P> static inline void fmtLabel (PrintHead<P>,bool io) {}
    template<typename P> static inline void fmtMode  (PrintHead<P>,bool io) {}
    template<typename P> static inline void fmtValue (PrintHead<P>,bool io) {}
    template<typename P> static inline void fmtUnit  (PrintHead<P>,bool io) {}
    constexpr static inline bool canNav() {return false;}
    constexpr static inline bool isRangePanel() {return false;}
    template<typename P> inline void printMenuRaw(MenuOut& menuOut,P,Item&) {}
    template<typename T> using itemFmt=ID<T>;
    template<typename T> using titleFmt=ID<T>;
    template<typename T> using menuFmt=ID<T>;
    template<typename T> using panelFmt=ID<T>;
    static inline void endl() {}
    template<typename H>
    static inline void clearLine(PrintHead<H>) {}
    static inline bool onMenuRender() {return MenuOut::onMenu;}
    static inline void enterMenuRender() {MenuOut::onMenu=true;}
    static inline void exitMenuRender() {MenuOut::onMenu=false;}
  };

  template<typename O,typename... OO>
  class OutList:public O {
    public:
      using O::O;
      //this works because printer head is never taken at this level
      //so dont do it!
      inline void newView() {
        O::newView();
        if (!O::onMenuRender()) next.newView();
      }
      template<typename T> inline void raw(T o) {
        O::raw(o);
        //without this glojbal print hits only the first device
        //with it menus will chain printing to next devices creating chaos
        if (!O::onMenuRender()) next.raw(o);//chain printing to all devices!
      }
      template<typename P>
      inline void printMenuRaw(MenuOut& menuOut,P p,Item&i) {
        assert(O::onMenuRender());
        O::printMenuRaw(menuOut,p,i);
        next.newView();
        next.printMenuRaw(next,PrintHead<OutList<OO...>>{/*next,*/next,0},i);
      }
      inline void setTarget(Item& i) {
        O::setTarget(i);
        assert(O::sharedNav());
        //next.setTarget(i);
      }
    protected:
      AM5::MenuOutCap<OutList<OO...>> next;
  };

  template<typename O>
  struct OutList<O>:public O {using O::O;};


  //holds scroll position. step should be font size in device coordinates
  template<typename O,int step=1>
  class ScrollPos:public O {
  public:
    //this should be a device only thing
    //not related to menu structure
    //but eventually controlled by it
    inline size_t top() const {return oi;}
    inline void scrlUp() {oi+=step;}
    inline void scrlDown() {oi-=step;}
    inline void scrlTo(size_t i) {oi=i;}
  protected:
    size_t oi;//option index
  };

  //bind output to existing device ---------------------------
  //use any stream as menu output
  template<typename Dev, Dev& dev, typename O>
  struct OutDev:public O {
    enum RAW_DEVICE {};
    constexpr bool hasRawDevice() {return true;}
    template<typename T> inline void raw(T o) {dev<<o;}
    // static inline void raw(const char*i) {dev<<i;}
    // static inline void raw(char i) {dev<<i;}
    // static inline void raw(unsigned char i) {dev<<i;}
    // #ifdef ARDUINO
    //   static inline void raw(endlObj i) {dev<<i;}
    //   static inline void raw(const __FlashStringHelper * i) {dev<<i;}
    // #endif
    //.. add more types here
  };

};//AM5
