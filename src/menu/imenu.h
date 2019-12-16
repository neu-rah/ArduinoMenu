/* -*- C++ -*- */
#pragma once

//menu interfaces
//for dynamic menus

#include "item.h"

class IItem;
class INav;
class IMenuOut;

//navigation interface
class INav {
  public:
    virtual Idx pos() const=0;
    virtual void cmd(Cmds c,INav& nav)=0;
    virtual void _cmd(Cmds c)=0;
    virtual bool selected(Idx n) const=0;
    virtual Modes mode() const=0;
    //-----------------------
    template<Cmds c> inline void _cmd() {_cmd(c);}
};

template<typename N>
class NavRoot:public INav,public N {
  public:
    using This=NavRoot<N>;
    Idx pos() const override {return N::pos();}
    void cmd(Cmds c,INav& nav) override {
      switch(c) {
        case Cmds::Enter:N::template cmd<Cmds::Enter,INav>(nav);break;
        case Cmds::Esc:N::template cmd<Cmds::Esc,INav>(nav);break;
        case Cmds::Up:N::template cmd<Cmds::Up,INav>(nav);break;
        case Cmds::Down:N::template cmd<Cmds::Down,INav>(nav);break;
        default:break;
      }
    }
    bool selected(Idx n) const override {return N::selected(n);}
    Modes mode() const override {return N::mode();}

    inline void up() {N::template cmd<Cmds::Up,This>(*this);}
    inline void down() {N::template cmd<Cmds::Down,This>(*this);}
    inline void enter() {N::template cmd<Cmds::Enter,This>(*this);}
    inline void esc() {N::template cmd<Cmds::Esc,This>(*this);}
    using INav::_cmd;
    void _cmd(Cmds c) override {
      switch(c) {
        case Cmds::Enter:N::template _cmd<Cmds::Enter>();break;
        case Cmds::Esc:N::template _cmd<Cmds::Esc>();break;
        case Cmds::Up:N::template _cmd<Cmds::Up>();break;
        case Cmds::Down:N::template _cmd<Cmds::Down>();break;
        default:break;
      }
    }
};

//output interface -----------------------------------------------------
class IMenuOut {
  public:
    virtual void print(const char* o,Roles role=Roles::Raw,bool toPrint=true)=0;
    virtual void printMenu(IItem& it,INav& nav,IMenuOut& out,OutOp op=OutOp::Printing)=0;
    virtual void printItem(IItem& it,IMenuOut& out,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal,bool toPrint=true)=0;
    //-----------------------
    template<typename T,typename Out,bool toPrint>
    inline void print(T o,Out& out,Roles role=Roles::Raw) {print(o,role,toPrint);}
    template<typename It,typename Nav,typename Out,OutOp op=OutOp::Printing>
    inline void printMenu(It& it,Nav& nav,Out& out) {
      _trace(MDO<<"IMenuOut::printMenu"<<endl);
      printMenu(it,nav,out,op);}
    template<typename It,typename Out,bool toPrint=true>
    void printItem(It& it,Out& out,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
      printItem(it,out,n,s,e,m,toPrint);
    }
};

template<typename O>
class MenuOut:public IMenuOut,public O {
  public:
    using O::printMenu;
    void printMenu(IItem& it,INav& nav,IMenuOut& out,OutOp op=OutOp::Printing) override {
      _trace(MDO<<"MenuOut::printMenu"<<endl);
      switch(op) {
        case OutOp::Measure:
        O::template printMenu<IItem,INav,IMenuOut,OutOp::Measure>(it,nav,out);
        break;
        case OutOp::Printing:
          O::template printMenu<IItem,INav,IMenuOut,OutOp::Printing>(it,nav,out);
          break;
        case OutOp::ClearChanges:
          O::template printMenu<IItem,INav,IMenuOut,OutOp::ClearChanges>(it,nav,out);
          break;
      }
    }
    using IMenuOut::print;
    void print(const char* o,Roles role=Roles::Raw,bool toPrint=true) override {
      if (toPrint) O::template print<const char*,MenuOut,true>(o,*this,role);
      else O::template print<const char*,MenuOut,false>(o,*this,role);
    }
    using O::printItem;
    void printItem(IItem& it,IMenuOut& out,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal,bool toPrint=true) override {
      if(toPrint) O::template printItem<IItem,IMenuOut,true>(it,out,n,s,e,m);
      else O::template printItem<IItem,IMenuOut,false>(it,out,n,s,e,m);
    }
};

//input interface -------------------------------------------------
class IMenuIn {};

//item interface -------------------------------------------------------
class IItem {
  public:
    inline virtual Idx size() const=0;
    inline Idx size(Ref) const {return size();}
    virtual bool changed() const=0;
    virtual void changed(bool)=0;
    virtual void changed(Idx,bool)=0;
    virtual bool enabled() const=0;
    virtual bool enabled(Idx n) const=0;
    virtual void printItem(IMenuOut&,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal,Roles role=Roles::Item,bool toPrint=true)=0;
    virtual void cmd(Cmds,INav&,Ref,Idx=0)=0;
    virtual bool activate()=0;
    virtual bool activate(Ref,Idx)=0;
    virtual bool canNav() const =0;
    virtual bool canNav(Ref,Idx) const =0;
    virtual bool parentDraw() const=0;
    virtual bool parentDraw(Ref,Idx) const=0;

    virtual void printMenu(bool pd,IItem& it,INav& nav,IMenuOut& out)=0;
    virtual void printMenu(bool pd,IItem& it,INav& nav,IMenuOut& out,Ref ref,Idx n)=0;
    // virtual void printMenu(bool pd,IItem& it,INav& nav,IMenuOut& out,Ref ref)=0;
    virtual void print(IMenuOut&,Roles=Roles::Raw,bool=true)=0;
    virtual void printItems(INav& nav,IMenuOut& out,Idx idx=0,Idx top=0,bool fullPrint=true,Roles role=Roles::Item,OutOp op=OutOp::Printing)=0;

    //--------------------------------------------
    template<typename Out,Roles role=Roles::Item,bool toPrint=true>
    inline void printItem(Out& out,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {printItem(out,n,s,e,m,role,toPrint);}
    template<Cmds c,typename Nav> inline void cmd(Nav& nav,Ref ref) {cmd(c,nav,ref,ref.head());}
    template<Cmds c,typename Nav> inline void cmd(Nav& nav,Ref ref,Idx n) {cmd(c,nav,ref,n);}
    template<typename Out,Roles role=Roles::Raw,bool toPrint=true>
    inline void print(Out& out) {print(out,role,toPrint);}
    template<typename Nav,typename Out,Roles role=Roles::Item,OutOp op=OutOp::Printing>
    inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0,bool fullPrint=true) {
      _trace(MDO<<"IItem::printItems"<<endl);
      printItems(nav,out,idx,top,fullPrint);
    }
};

//item virtual cap
template<typename I>
class Prompt:public IItem,public Item<I> {
  public:
    using Base=Item<I>;
    using Base::Base;
    inline Idx size() const override {return Base::size();}
    bool changed() const override {return Base::changed();}
    void changed(bool o) override {Base::changed(o);}
    void changed(Idx n,bool o) override {Base::changed(n,o);}
    bool enabled() const override {return Base::enabled();}
    bool enabled(Idx n) const override {return Base::enabled(n);}
    bool activate() override {return Base::activate();}
    bool activate(Ref ref,Idx n) override {return Base::activate(ref,n);}
    bool canNav() const override {return Base::canNav();}
    bool canNav(Ref ref,Idx n) const override {return Base::canNav(ref,n);}
    bool parentDraw() const override {return Base::parentDraw();}
    bool parentDraw(Ref ref,Idx n) const override {return Base::parentDraw(ref,n);}

    void printMenu(bool pd,IItem& it,INav& nav,IMenuOut& out) override {
      _trace(MDO<<"Prompt::printMenu"<<endl);
      Base::printMenu(pd,it,nav,out);}
    // void printMenu(bool pd,IItem& it,INav& nav,IMenuOut& out,Ref ref) override {
    //   Base::printMenu(pd,it,nav,out,ref);}
    void printMenu(bool pd,IItem& it,INav& nav,IMenuOut& out,Ref ref,Idx n) override {
      Base::printMenu(pd,it,nav,out,ref,n);}
    void print(IMenuOut& out,Roles role=Roles::Raw,bool toPrint=true) override {
      switch(role) {
        case Roles::Raw:
          if(toPrint) Base::template print<IMenuOut,Roles::Raw,true>(out);
          else Base::template print<IMenuOut,Roles::Raw,false>(out);
          break;
        case Roles::Panel:
          if(toPrint) Base::template print<IMenuOut,Roles::Panel,true>(out);
          else Base::template print<IMenuOut,Roles::Panel,false>(out);
          break;
        case Roles::Menu:
          if(toPrint) Base::template print<IMenuOut,Roles::Menu,true>(out);
          else Base::template print<IMenuOut,Roles::Menu,false>(out);
          break;
        case Roles::Title:
          if(toPrint) Base::template print<IMenuOut,Roles::Title,true>(out);
          else Base::template print<IMenuOut,Roles::Title,false>(out);
          break;
        case Roles::Body:
          if(toPrint) Base::template print<IMenuOut,Roles::Body,true>(out);
          else Base::template print<IMenuOut,Roles::Body,false>(out);
          break;
        case Roles::Item:
          if(toPrint) Base::template print<IMenuOut,Roles::Item,true>(out);
          else Base::template print<IMenuOut,Roles::Item,false>(out);
          break;
        case Roles::Index:
          if(toPrint) Base::template print<IMenuOut,Roles::Index,true>(out);
          else Base::template print<IMenuOut,Roles::Index,false>(out);
          break;
        case Roles::Cursor:
          if(toPrint) Base::template print<IMenuOut,Roles::Cursor,true>(out);
          else Base::template print<IMenuOut,Roles::Cursor,false>(out);
          break;
        case Roles::Name:
          if(toPrint) Base::template print<IMenuOut,Roles::Name,true>(out);
          else Base::template print<IMenuOut,Roles::Name,false>(out);
          break;
        case Roles::Mode:
          if(toPrint) Base::template print<IMenuOut,Roles::Mode,true>(out);
          else Base::template print<IMenuOut,Roles::Mode,false>(out);
          break;
        case Roles::Value:
          if(toPrint) Base::template print<IMenuOut,Roles::Value,true>(out);
          else Base::template print<IMenuOut,Roles::Value,false>(out);
          break;
        case Roles::Unit:
          if(toPrint) Base::template print<IMenuOut,Roles::Unit,true>(out);
          else Base::template print<IMenuOut,Roles::Unit,false>(out);
          break;
      }
    }
    void printItems(INav& nav,IMenuOut& out,Idx idx=0,Idx top=0,bool fullPrint=true,Roles role=Roles::Item,OutOp op=OutOp::Printing) override {
      _trace(MDO<<"Prompt::printItems"<<endl);
     switch(role) {
       case Roles::Raw:
         switch(op) {
           case OutOp::Measure: Base::template printItems<IItem,INav,IMenuOut,Roles::Raw,OutOp::Measure>(*this,nav,out,idx,top);break;
           case OutOp::Printing: Base::template printItems<IItem,INav,IMenuOut,Roles::Raw,OutOp::Printing>(*this,nav,out,idx,top);break;
           case OutOp::ClearChanges: Base::template printItems<IItem,INav,IMenuOut,Roles::Raw,OutOp::ClearChanges>(*this,nav,out,idx,top);break;
         }
         break;
     }
   }
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
    void cmd(Cmds c,INav& nav,Ref ref,Idx n=0) override {
      switch(c) {
        case Cmds::Enter:Base::template cmd<Cmds::Enter,INav>(nav,ref,n);break;
        case Cmds::Esc:Base::template cmd<Cmds::Esc,INav>(nav,ref,n);break;
        case Cmds::Up:Base::template cmd<Cmds::Up,INav>(nav,ref,n);break;
        case Cmds::Down:Base::template cmd<Cmds::Down,INav>(nav,ref,n);break;
        case Cmds::Left:Base::template cmd<Cmds::Left,INav>(nav,ref,n);break;
        case Cmds::Right:Base::template cmd<Cmds::Right,INav>(nav,ref,n);break;
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
  inline bool parentDraw() const {return false;}
  inline bool parentDraw(Idx n) const {return data[n].parentDraw();}
  inline bool parentDraw(Ref ref,Idx n) const {
    return ref?data[n].parentDraw(ref.tail(),ref.tail().head()):parentDraw(n);
  }
  inline Idx size() const {return data.size();}
  inline Idx size(Ref ref) const {return ref.len?data[ref.head()].size(ref.tail()):data.size();}
  template<Cmds c,typename Nav> inline void cmd(Nav& nav,Ref ref) {
    data[ref.head()].template cmd<c,Nav>(nav,ref.tail());
  }
  template<Cmds c,typename Nav> inline void cmd(Nav& nav,Ref ref,Idx n) {
    if (ref) data[n].template cmd<c,Nav>(nav,ref.tail(),ref.tail().head());
    else data[n].template cmd<c,Nav>(nav,ref);
  }
  inline static constexpr bool activate() {return true;}
  inline bool activate(Ref ref,Idx n) {
    return ref?data[n].activate(ref.tail(),ref.tail().head()):activate();
  }
  inline static constexpr bool canNav() {return true;}
  inline bool canNav(Ref ref,Idx n) {
    return ref?data[n].canNav(ref.tail(),ref.tail().head()):data[n].canNav();
  }
  template<typename It,typename Nav,typename Out>
  inline void printMenu(bool pd,It& it,Nav& nav,Out& out,Ref ref,Idx n) {
    _trace(MDO<<"IterableData::printItems"<<endl);
    if (ref) data[n].printMenu(pd,data[n],nav,out,ref.tail(),ref.tail().head());
    else data[n].printMenu(pd,data[n],nav,out);
  }
  template<typename It,typename Nav,typename Out,Roles role=Roles::Item,OutOp op=OutOp::Printing>
  inline void printItems(It& it,Nav& nav,Out& out,Idx idx=0,Idx top=0,bool fullPrint=true) {
    _trace(MDO<<"IterableData::printItems"<<endl);
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
