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
    virtual void _printMenu(INav& nav,IMenuOut& out)=0;
    virtual void left()=0;
    virtual void right()=0;
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
    void _printMenu(INav& nav,IMenuOut& out) override {
      N::_printMenu(nav,out);
    }
    // inline void printMenu(IMenuOut& out);

    void left() override {N::level--;}
    void right() override {N::level++;}
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
    using N::printMenu;
    template<typename Out>
    inline void printMenu(Out& out) {
      trace(MDO<<"INav::printMenu(Out)"<<endl);
      out.printOn(*this,out);}
};

//output interface -----------------------------------------------------
class IMenuOut {
  public:
    virtual void print(const char* o,Roles role=Roles::Raw,bool toPrint=true)=0;
    virtual void printMenu(IItem& it,INav& nav,IMenuOut& out,OutOp op=OutOp::Printing)=0;
    virtual void printItem(IItem& it,IMenuOut& out,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal,bool toPrint=true)=0;
    virtual void printParent(IItem& it,INav& nav,IMenuOut& out)=0;
    virtual bool partialDraw()=0;
    //-----------------------
    template<typename T,typename Out,bool toPrint>
    inline void print(T o,Out& out,Roles role=Roles::Raw) {print(o,role,toPrint);}
    template<typename It,typename Nav,typename Out,OutOp op=OutOp::Printing>
    inline void printMenu(It& it,Nav& nav,Out& out);//TODO: remove this
    template<typename It,typename Out,bool toPrint=true>
    void printItem(It& it,Out& out,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
      printItem(it,out,n,s,e,m,toPrint);
    }
    // void printOn(INav& nav,IMenuOut& out) {nav._printMenu(nav,out);}
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
    void printParent(IItem& it,INav& nav,IMenuOut& out) override {O::printParent(it,nav,out);}
    bool partialDraw() override {return O::partialDraw;}
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
    template<typename It,typename Nav,typename Out,Roles role=Roles::Item,OutOp op=OutOp::Printing>
    inline void printItems(It& it,Nav& nav,Out& out,Idx idx=0,Idx top=0,bool fullPrint=true) {
      _trace(MDO<<"IItem::printItems"<<endl);
      printItems(nav,out,idx,top,fullPrint);
    }
    // template<Cmds c,typename Nav>
    // inline void cmd(Nav& nav) {cmd(c,nav,nav);}
    // template<Cmds c,typename Nav>
    // inline void cmd(Nav& nav,Ref ref,Idx n=0) {cmd(c,nav,nav);}
};

//item virtual cap
template<typename I>
class Prompt:public IItem,public Item<I> {
  public:
    using Base=Item<I>;
    using Base::Base;
    using IItem::size;
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
    bool parentDraw(Ref ref,Idx n=0) const override {return Base::parentDraw(ref,n);}

    using I::printMenu;
    void printMenu(bool pd,IItem& it,INav& nav,IMenuOut& out) override {
      _trace(MDO<<"Prompt::printMenu"<<endl);
      Base::printMenu(pd,it,nav,out);}
    // void printMenu(bool pd,IItem& it,INav& nav,IMenuOut& out,Ref ref) override {
    //   Base::printMenu(pd,it,nav,out,ref);}
    void printMenu(bool pd,IItem& it,INav& nav,IMenuOut& out,Ref ref,Idx n=0) override {
      Base::printMenu(pd,it,nav,out,ref,n);}
    void print(IMenuOut& out,Roles role=Roles::Raw,bool toPrint=true) override;
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
   void printItem(IMenuOut& out,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal,Roles role=Roles::Item,bool toPrint=true) override;
   using IItem::cmd;
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
  template<typename It,typename Nav,typename Out>
  inline void printMenu(bool pd,It& it,Nav& nav,Out& out,OutOp op=OutOp::Printing) {
    switch(op) {
      case OutOp::Measure:
        out.template printMenu<It,Nav,Out,OutOp::Measure>(it,nav,out);
        break;
      case OutOp::Printing:
        out.template printMenu<It,Nav,Out,OutOp::Printing>(it,nav,out);
        break;
      case OutOp::ClearChanges:
        out.template printMenu<It,Nav,Out,OutOp::ClearChanges>(it,nav,out);
        break;
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
  template<Cmds c,typename Nav> inline void cmd(Nav& nav) {
    I::template cmd<c,Nav>(nav);
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
