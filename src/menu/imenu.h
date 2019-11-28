/* -*- C++ -*- */
#pragma once

//menu interfaces
//for dynamic menus

#include "item.h"

//navigation interface
class INav {
  public:
    // template<Cmds c,typename Nav>
    virtual void cmd(Cmds c,INav& nav)=0;
};

template<typename N>
class NavRoot:public INav,public N {
  public:
    using This=NavRoot<N>;
    void cmd(Cmds c,INav& nav) override {N::template cmd<c,INav>(*this);}
    inline void up() {N::template cmd<Cmds::Up,This>(*this);}
    inline void down() {N::template cmd<Cmds::Down,This>(*this);}
    inline void enter() {N::template cmd<Cmds::Enter,This>(*this);}
    inline void esc() {N::template cmd<Cmds::Esc,This>(*this);}
};

//output interface -----------------------------------------------------
class IMenuOut {
  public:
    virtual void print(const char* o,Roles role=Roles::Raw,bool toPrint=true)=0;
    template<typename T,typename Out,bool toPrint>
    inline void print(T o,Out& out,Roles role=Roles::Raw) {print(o,role,toPrint);}
};

template<typename O>
class MenuOut:public IMenuOut,public O {
  public:
    using IMenuOut::print;
    void print(const char* o,Roles role=Roles::Raw,bool toPrint=true) override {
      if (toPrint) O::template print<const char*,MenuOut,true>(o,*this,role);
      else O::template print<const char*,MenuOut,false>(o,*this,role);
    }
};

//input interface -------------------------------------------------
class IMenuIn {};

//item interface -------------------------------------------------------
class IItem {
  public:
    inline virtual Idx size() const=0;
    inline Idx size(Ref) const {return size();}
    virtual void changed(bool)=0;
    virtual bool changed() const=0;
    virtual bool enabled() const=0;
    virtual bool enabled(Idx n) const=0;
    virtual void printItem(IMenuOut&,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal,Roles role=Roles::Item,bool toPrint=true)=0;
    virtual void cmd(Cmds,INav&,Ref)=0;
    template<typename Out,Roles role=Roles::Item,bool toPrint=true>
    inline void printItem(Out& out,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {printItem(out,n,s,e,m,role,toPrint);}
    template<Cmds c,typename Nav> inline void cmd(Nav& nav,Ref ref) {cmd(c,nav,ref);}
};

//item virtual cap
template<typename I>
class Prompt:public IItem,public Item<I> {
  public:
    using Base=Item<I>;
    using Base::Base;
    inline Idx size() const override {return Base::size();}
    void changed(bool o) override {Base::changed(o);}
    bool changed() const override {return Base::changed();}
    bool enabled() const override {return Base::enabled();}
    bool enabled(Idx n) const override {return Base::enabled(n);}
    using Base::printItem;
    void printItem(IMenuOut& out,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal,Roles role=Roles::Item,bool toPrint=true) override {
      switch (role) {
        case Roles::Raw:
          if (toPrint) Base::template printItem<Item<I>,IMenuOut,Roles::Raw,true>(*this,out,n,s,e,m);
          else Base::template printItem<Item<I>,IMenuOut,Roles::Raw,false>(*this,out,n,s,e,m);
          break;
        case Roles::Panel:
          if (toPrint) Base::template printItem<Item<I>,IMenuOut,Roles::Panel,true>(*this,out,n,s,e,m);
          else Base::template printItem<Item<I>,IMenuOut,Roles::Panel,false>(*this,out,n,s,e,m);
          break;
        case Roles::Menu:
          if (toPrint) Base::template printItem<Item<I>,IMenuOut,Roles::Menu,true>(*this,out,n,s,e,m);
          else Base::template printItem<Item<I>,IMenuOut,Roles::Menu,false>(*this,out,n,s,e,m);
          break;
        case Roles::Title:
          if (toPrint) Base::template printItem<Item<I>,IMenuOut,Roles::Title,true>(*this,out,n,s,e,m);
          else Base::template printItem<Item<I>,IMenuOut,Roles::Title,false>(*this,out,n,s,e,m);
          break;
        case Roles::Body:
          if (toPrint) Base::template printItem<Item<I>,IMenuOut,Roles::Body,true>(*this,out,n,s,e,m);
          else Base::template printItem<Item<I>,IMenuOut,Roles::Body,false>(*this,out,n,s,e,m);
          break;
        case Roles::Item:
          if (toPrint) Base::template printItem<Item<I>,IMenuOut,Roles::Item,true>(*this,out,n,s,e,m);
          else Base::template printItem<Item<I>,IMenuOut,Roles::Item,false>(*this,out,n,s,e,m);
          break;
        case Roles::Index:
          if (toPrint) Base::template printItem<Item<I>,IMenuOut,Roles::Index,true>(*this,out,n,s,e,m);
          else Base::template printItem<Item<I>,IMenuOut,Roles::Index,false>(*this,out,n,s,e,m);
          break;
        case Roles::Cursor:
          if (toPrint) Base::template printItem<Item<I>,IMenuOut,Roles::Cursor,true>(*this,out,n,s,e,m);
          else Base::template printItem<Item<I>,IMenuOut,Roles::Cursor,false>(*this,out,n,s,e,m);
          break;
        case Roles::Name:
          if (toPrint) Base::template printItem<Item<I>,IMenuOut,Roles::Name,true>(*this,out,n,s,e,m);
          else Base::template printItem<Item<I>,IMenuOut,Roles::Name,false>(*this,out,n,s,e,m);
          break;
        case Roles::Mode:
          if (toPrint) Base::template printItem<Item<I>,IMenuOut,Roles::Mode,true>(*this,out,n,s,e,m);
          else Base::template printItem<Item<I>,IMenuOut,Roles::Mode,false>(*this,out,n,s,e,m);
          break;
        case Roles::Value:
          if (toPrint) Base::template printItem<Item<I>,IMenuOut,Roles::Value,true>(*this,out,n,s,e,m);
          else Base::template printItem<Item<I>,IMenuOut,Roles::Value,false>(*this,out,n,s,e,m);
          break;
        case Roles::Unit:
          if (toPrint) Base::template printItem<Item<I>,IMenuOut,Roles::Unit,true>(*this,out,n,s,e,m);
          else Base::template printItem<Item<I>,IMenuOut,Roles::Unit,false>(*this,out,n,s,e,m);
          break;
      }
    }
    void cmd(Cmds c,INav& nav,Ref ref) override {
      switch(c) {
        case Cmds::Enter:Base::template cmd<Cmds::Enter,INav>(nav,ref);break;
        case Cmds::Esc:Base::template cmd<Cmds::Esc,INav>(nav,ref);break;
        case Cmds::Up:Base::template cmd<Cmds::Up,INav>(nav,ref);break;
        case Cmds::Down:Base::template cmd<Cmds::Down,INav>(nav,ref);break;
        case Cmds::Left:Base::template cmd<Cmds::Left,INav>(nav,ref);break;
        case Cmds::Right:Base::template cmd<Cmds::Right,INav>(nav,ref);break;
        default:break;
      }
    }
};

template<typename T,T** data,Idx sz,typename I=Empty>
struct ArrayData:I {
  inline constexpr Idx size() const {return sz;}
  T& operator[](Idx n) const {return *data[n];}
};

template<typename T,typename I=Empty>
struct IterableData:I {
  T data;
  using I::I;
  inline Idx size() const {return data.size();}
  inline Idx size(Ref ref) const {return ref.len?data[ref.head()].size(ref.tail()):data.size();}
  template<Cmds c,typename Nav> inline void cmd(Nav& nav,Ref ref) {
    data[ref.head()].template cmd<c,Nav>(nav,ref.tail());
  }
  template<Cmds c,typename Nav> inline void cmd(Nav& nav,Ref ref,Idx) {
    assert(false);
  }
  template<typename Nav,typename Out,Roles role=Roles::Item,OutOp op=OutOp::Printing>
  inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0,bool fullPrint=true) {
    for(int n=top;n<size()&&out.freeY();n++) {
      switch(op) {
        case OutOp::Measure:
          out.template printItem<IItem,Out,false>(data[n],out,n,nav.selected(n),data[n].enabled(),nav.mode());
          break;
        case OutOp::Printing:
          if (fullPrint||data[n].changed()||!out.partialDraw())
            out.template printItem<IItem,Out,true>(data[n],out,n,nav.selected(n),data[n].enabled(),nav.mode());
          else
            out.template printItem<IItem,Out,false>(data[n],out,n,nav.selected(n),data[n].enabled(),nav.mode());
          break;
        case OutOp::ClearChanges:
          data[n].changed(false);
      }
    }
  }
};
