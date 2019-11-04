/* -*- C++ -*- */
#pragma once
#include "base.h"

struct Nil {};

struct IMenuIn;
struct IMenuOut;
struct IItem;
struct INav;

////////////////////////////////////////////////////////////////////////////////
//provide some static data to static functions
template<typename O> struct Data:O {using O::O;static O obj;};

////////////////////////////////////////////////////////////////////////////////
template<typename O=Nil> struct Void:O {
  template<Roles r,bool io,typename Nav,typename Out>
  static inline void fmt(Nav& nav,Out& out,Idx n=0) {}
  static inline void clrLine(Idx) {}
  constexpr static inline bool isViewport() {return false;}
  constexpr static inline Idx height() {return 0;}
  constexpr static inline Idx top() {return 0;}
  constexpr static inline Idx posX() {return 0;}
  constexpr static inline Idx posY() {return 0;}
  constexpr static inline Idx freeX() {return idx_max;}
  constexpr static inline Idx freeY() {return idx_max;}
  constexpr static inline Idx free() {return idx_max;}
  static inline void useX(Idx ux=1) {}
  static inline void useY(Idx uy=1) {}
};

//runtime output interface
struct IMenuOut {
  inline virtual void print(char o) const=0;
  inline virtual void print(int o) const=0;
  inline virtual void print(long o) const=0;
  inline virtual void print(unsigned char o) const=0;
  inline virtual void print(unsigned int o) const=0;
  inline virtual void print(unsigned long o) const=0;
  inline virtual void print(const char* o) const=0;
  inline virtual void print(const unsigned char* o) const=0;
  inline virtual void nl()=0;
  inline virtual void fmt(Roles,bool,INav&,IMenuOut&,Idx)=0;
  inline virtual void clrLine(Idx)=0;
  inline virtual Idx top()=0;
  inline virtual void posTop(INav& nav)=0;
  inline virtual Idx posY()=0;
  inline virtual Idx freeY()=0;
  inline virtual void newView()=0;
  inline virtual void printMenu(IItem& it,INav& nav)=0;
  template<Roles r,bool io,typename Nav,typename Out>
  inline void fmt(Nav& nav,Out& out,Idx n=0) {fmt(r,io,nav,out,n);}
  template<typename T> inline void raw(T o) {print(o);}
  template<typename It,typename Nav,typename Out>
  inline void printMenu(It& it,Nav& nav) {printMenu(it,nav);}
  template<typename Nav> inline void posTop(Nav& nav) {posTop(nav);}
};

////////////////////////////////////////////////////////////////////////////////
template<typename N=Nil> struct Drift:N {
  constexpr static inline Modes mode() {return Modes::Normal;}
  constexpr static inline bool enabled(Idx) {return true;}
  template<Cmds c,typename It,typename Nav>
  inline bool _cmd(It& it,Nav& nav) {return true;}
};

//runtime dynamic interface/////////////////////////////////////////////////////

struct INav {
  virtual inline Modes mode() const=0;
  virtual inline void setMode(Modes m)=0;
  virtual inline bool enabled(Idx) const=0;
  virtual inline bool selected(Idx) const=0;
  virtual inline Idx pos() const=0;
  virtual inline void setPos(Idx)=0;
  virtual inline void printOn(IMenuOut&)=0;
  virtual inline bool cmd(Cmds)=0;
  virtual inline bool cmd(Cmds,IItem&)=0;
  virtual inline void open()=0;
  virtual inline void close()=0;
  virtual inline bool doInput(IMenuIn&)=0;
  virtual operator Ref() const=0;
  operator Idx() const {return pos();}
  template<Cmds c,typename It,typename Nav>
  inline bool _cmd(It& it,Nav&) {return cmd(c,it);}

  inline bool enter() {return cmd(Cmds::Enter);}
  inline bool esc()   {return cmd(Cmds::Esc);}
  inline bool up()    {return cmd(Cmds::Up);}
  inline bool down()  {return cmd(Cmds::Down);}
  inline bool left()  {return cmd(Cmds::Left);}
  inline bool right() {return cmd(Cmds::Right);}
};

////////////////////////////////////////////////////////////////////////////////
struct IMenuIn {
  inline virtual bool cmd(INav& nav,bool inv=false)=0;
};

template<typename I>
struct MenuIn:I,IMenuIn {
  using This=MenuIn<I>;
  inline bool cmd(INav& nav,bool inv=false) override {
    return inv?
      I::template cmd<INav,true>(nav):
      I::template cmd<INav,false>(nav);
  }
};

////////////////////////////////////////////////////////////////////////////////
//menu item dynamic interface //////////////////////////////////////////////////
class IItem {
  public:
    // virtual inline void enable(Idx,bool) const {}
    virtual inline void enable(bool) const {}
    virtual inline bool enabled() const {return true;}
    virtual inline bool enabled(Ref ref,Idx n) const=0;
    virtual inline bool canNav() const=0;
    virtual inline bool canNav(Ref ref,Idx n)=0;
    virtual inline bool parentDraw() const=0;
    virtual inline bool cmd(Cmds,INav&,Ref,Idx)=0;
    virtual inline bool cmd(Cmds c,INav& nav)=0;
    virtual void fmt(Roles r,bool io,INav& nav,IMenuOut& out,Idx n)=0;
    virtual inline Idx size() const {return 0;}
    virtual inline Idx size(Ref) const {return 0;}
    virtual inline void print(IMenuOut&)=0;
    virtual inline void printMenu(INav& nav,IMenuOut&) =0;
    virtual inline void printItems(INav& nav,IMenuOut&) =0;
    template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
    inline void printItems(It& it,Nav& nav,Out& out) {printItems(nav,out);}
    template<Cmds c,typename It,typename Nav>
    inline bool _cmd(It& it,Nav& nav) {return cmd(c,nav);}
    template<Cmds c,typename It,typename Nav>
    inline bool cmd(It& it,Nav& nav) {return cmd(c,nav);}
    template<Cmds c,typename It,typename Nav>
    inline bool cmd(It& it,Nav& nav,Ref ref,Idx n) {return cmd(c,nav,ref,n);}
    template<typename Out> inline void print(Out& out) {print(out);}
    template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
    inline void print(It& it,Nav& nav,Out& out) {print(out);}
    template<Roles r,bool io,typename It,typename Out,typename Nav>
    inline void fmt(Nav& nav,Out& out,Idx n=0) {
      fmt(r,io,nav,out,n);
    }
};

template<typename I=Nil> struct Empty:I {
  constexpr static inline bool enabled() {return true;}
  constexpr static inline bool enabled(Ref,Idx) {return true;}
  inline static void enable(Idx,bool) {}
  inline static void enable(bool) {}
  inline void enable(Ref,Idx,bool) {}
  inline static Idx size() {return 0;}
  inline static constexpr Idx size(Ref ref) {return 0;}
  inline static constexpr Idx size(Ref ref,Idx n) {return 0;}
  template<Roles r,bool io,typename Nav,typename Out>
  static inline void fmt(Nav& nav,Out& out,Idx n=0) {
    out.template fmt<r,io,Nav,Out>(nav,out,n);
  }
  template<typename Out> inline static void print(Out& out) {}
  // template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  // inline static void print(It& it,Nav& nav,IMenuOut& out) {}
  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline static void print(It& it,Nav& nav,Out& out) {it.template print<Out>(out);}
  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline void printItems(It& it,Nav& nav,Out& out) {it.template print<Out>(out);}
  template<typename It,typename Nav,typename Out>
  inline static void printMenu(It& it,Nav& nav,Out& out,Ref ref,Idx n) {
    it.template print<It,Nav,Out>(it,nav,out);
  }
  // template<typename It,typename Nav,typename Out>
  // inline static void printMenu(It& it,Nav& nav,Out& out,Ref ref,Idx n) {
  //   it.print(out);
  // }
  template<Cmds c,typename It,typename Nav>
  inline static bool cmd(It& it,Nav& nav) {return c==Cmds::Activate?false:nav.template _cmd<c,It,Nav>(it,nav);}
  template<Cmds c,typename It,typename Nav>
  inline static bool cmd(It& it,Nav& nav,Ref ref,Idx n) {return it.template _cmd<c,It,Nav>(it,nav);}
  inline static constexpr bool canNav() {return false;}
  inline static constexpr bool canNav(Ref ref,Idx n) {return canNav();}
  inline static constexpr bool parentDraw() {return false;}
  inline static constexpr bool parentDraw(Idx n) {return parentDraw();}
};
