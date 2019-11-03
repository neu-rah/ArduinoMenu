/* -*- C++ -*- */
#pragma once
// #include <lpp.h>
#include "api.h"

/// ActionHanlder, type of action functions to associate with items
using ActionHandler=bool (*)();

/**
* The Action class associates an actikon function with a menu item.
*/
template<typename I,ActionHandler act>
struct Action:I {
  using I::I;
  using This=Action<I,act>;
  using I::cmd;
  template<Cmds c,typename It,typename Nav>
  inline bool cmd(It& it,Nav& nav) {return act();}
};

template<const char** text,typename I=Empty<>>
struct StaticText:I {
  using I::print;
  template<typename Out> inline static void print(Out& out) {out.raw(text[0]);}
};

//wrap an item with static prefix/suffix content
template<typename Of,typename Prefix,typename Suffix=Empty<>>
struct StaticWrap:Of {
  using Of::Of;
  using Of::print;
  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline void print(It& it,Nav& nav,Out& out) {
    trace(MDO<<"StaticWrap::print"<<endl);
    Prefix::template print<Prefix,Nav,Out,P>(*reinterpret_cast<Prefix*>(&it),nav,out);
    Of::template print<It,Nav,Out,P>(it,nav,out);
    Suffix::template print<Suffix,Nav,Out,P>(*reinterpret_cast<Suffix*>(&it),nav,out);
  }
};

////////////////////////////////////////////////////////////////////////////////////////////
// compile time list
template<typename I,typename... II>
struct StaticData:StaticData<I> {
  using This=StaticData<I,II...>;
  using Head=I;
  using Base=StaticData<I>;
  using Tail=StaticData<II...>;
  using Base::size;
  using Base::Base;
  Tail next;

  inline static constexpr Idx size() {return Tail::size()+1;}
  inline static constexpr Idx size(Ref ref) {
    return ref.len?size(ref,ref.head()):size();
  }
  inline static constexpr Idx size(Ref ref,Idx n) {
    return
      n?Tail::size(ref,n-1):
        ref.len?
          I::size(ref.tail(),ref.tail().head()):
          size();
  }
  using I::parentDraw;
  inline static constexpr bool parentDraw(Idx n) {
    return n?Tail::parentDraw(n-1):I::parentDraw();
  }
  using I::enabled;
  inline bool enabled(Idx n) const {return n?next.enabled(n-1):enabled();}
  inline bool enabled(Ref ref,Idx n) const {
    return n?next.enabled(ref,n-1):
      ref.len? enabled(ref.tail(),ref.head()):enabled();
  }

  using I::enable;
  inline void enable(Idx n,bool b) {return n?next.enable(n-1,b):enable(b);}
  inline void enable(Ref ref,Idx n,bool e) {
    if (n) next.enable(ref,n-1,e);
    else if(ref.len) enable(ref.tail(),ref.head(),e);
    else enable(e);
  }

  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline void printItem(It& it,Nav& nav,Out& out,Idx n,Idx p=0) {
    if (!Out::freeY()) return;
    if (p) next.template printItem<It,Nav,Out,P>(it,nav,out,n,p-1);
    StaticData<I>::template printItem<I,Nav,Out>(*this,nav,out,n);
    next.Tail::template printItem<It,Nav,Out>(it,nav,out,n+1);
  }

  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline void printItems(It& it,Nav& nav,Out& out) {
    trace(MDO<<"StaticData<I,II...>::printItems"<<endl);
    Out::posTop(nav);
    if (!Out::freeY()) return;
    Out::clrLine(Out::posY());
    Out::template fmt<P,true,Nav,Out>(nav,out);
    printItem<This,Nav,Out>(*this,nav,out,Out::top(),Out::top());
    Out::template fmt<P,false,Nav,Out>(nav,out);
  }

  template<typename It,typename Nav,typename Out>
  inline void printMenu(It& it,Nav& nav,Out& out,Ref ref,Idx n) {
    if (n) next.template printMenu<It,Nav,Out>(it,nav,out,ref,n-1);
    else if (ref.len)
      I::template printMenu<I,Nav,Out>(*reinterpret_cast<I*>(this),nav,out,ref.tail(),ref.tail().head());
    else Out::template printMenu<I,Nav,Out>(*reinterpret_cast<I*>(this),nav,out);
  }

  using I::cmd;
  using StaticData<I>::doNav;
  template<Cmds c,typename It,typename Nav>
  inline bool cmd(It& it,Nav& nav,Ref ref,Idx n) {
    assert(c!=Cmds::Esc);
    if (n) {
      return next.Tail::template cmd<c,It,Nav>(it,nav,ref,n-1);
    } else if (ref.len) {
      return I::template cmd<c,I,Nav>(*reinterpret_cast<I*>(this),nav,ref.tail(),ref.tail().head());
    } else return Base::template doNav<c,It,Nav>(it,nav);
  }
  using StaticData<I>::canNav;
  inline static bool canNav(Ref ref,Idx n) {
    if (n) return Tail::canNav(ref,n-1);
    else if (ref.len) return I::canNav(ref.tail(),ref.head());
    else return StaticData<I>::canNav();
  }
};

/////////////////////////////////////////////////////////////////////////////////////////
// compile time list termination
template<typename I>
struct StaticData<I>:I {
  using This=StaticData<I>;
  using I::parentDraw;
  using I::I;
  inline static constexpr bool parentDraw(Idx n) {
    return I::parentDraw();
  }
  inline static constexpr bool canNav() {return true;}
  inline static constexpr bool canNav(Ref ref,Idx n) {return n?false:canNav();}
  inline static constexpr Idx size() {return 1;}
  inline static constexpr Idx size(Ref ref) {
    return ref.len?size(ref,ref.head()):size();
  }
  inline static constexpr Idx size(Ref ref,Idx n) {
    return n?1:ref.len?I::size(ref.tail(),ref.tail().head()):I::size()+1;
  }
  using I::enabled;
  inline bool enabled(Idx n) const {return n?true:enabled();}

  using I::enable;
  inline void enable(Idx n,bool b) {if(!n) I::enable(b);}


  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline void printItem(It& it,Nav& nav,Out& out,Idx n,Idx p=0) {
    if (!Out::freeY()) return;
    trace(MDO<<"printItem #"<<n<<" role:"<<p<<"=>");
    // Out::clrLine(Out::posY());
    it.template fmt<Roles::Prompt,true ,Nav,Out>(nav,out,n);
    it.template fmt<Roles::Index, true ,Nav,Out>(nav,out,n);
    it.template fmt<Roles::Index, false,Nav,Out>(nav,out,n);
    it.template fmt<Roles::Cursor,true ,Nav,Out>(nav,out,n);
    it.template fmt<Roles::Cursor,false ,Nav,Out>(nav,out,n);
    reinterpret_cast<I*>(this)->template print<I,Nav,Out,Roles::Prompt>(
        *reinterpret_cast<I*>(this),nav,out
    );
    it.template fmt<Roles::Prompt,false,Nav,Out>(nav,out,n);
  }
  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline void printItems(It& it,Nav& nav) {
    Out::posTop(nav);
    it.template fmt<P,true,Nav,Out>(nav,0);
    printItem<This,Nav,Out>(*this,nav,Out::top(),Out::top());
    it.template fmt<P,false,Nav,Out>(nav,0);
  }

  template<typename It,typename Nav,typename Out>
  inline void printMenu(It& it,Nav& nav,Out& out,Ref ref,Idx n) {
    assert(!n);
    if (n) return;
    else if (ref.len) reinterpret_cast<I*>(this)->I::template printMenu<I,Nav,Out>(*reinterpret_cast<I*>(this),nav,out,ref.tail(),ref.head());
    else Out::template printMenu<I,Nav,Out>(*reinterpret_cast<I*>(this),nav,out);
  }
  template<Cmds c,typename It,typename Nav>
  inline bool cmd(It& it,Nav& nav,Ref ref,Idx n) {
    assert(c!=Cmds::Esc);
    if (n)
      assert(!n);
    if (n||!enabled()) return false;
    if (ref.len) return I::template cmd<c,I,Nav>(*reinterpret_cast<I*>(this),nav,ref,n);
    return doNav<c,It,Nav>(it,nav);
  }
  template<Cmds c,typename It,typename Nav>
  inline bool doNav(It& it,Nav& nav) {
    if(c==Cmds::Enter){
      if(I::template cmd<Cmds::Activate,It,Nav>(it,nav)) {
        if (I::canNav()) {nav.open();return true;}
      } else if (!I::canNav()) {nav.close();return true;}
    }
    return Empty<>::template cmd<c,It,Nav>(it,nav);//will pass cmd back to navigation
  }
};

template<typename T,typename B>
struct StaticMenu:B {
  using This=StaticMenu<T,B>;
  using Title=T;
  using Body=B;
  using B::B;
  template<typename Out> inline static void print(Out& out) {T::template print<Out>(out);}
  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline static void print(It& it,Nav& nav,Out& out) {
    Title::template print<Out>(out);
  }
  using B::cmd;
  template<Cmds c,typename It,typename Nav>
  inline constexpr bool cmd(It& it,Nav& nav) {return true;}

  using B::printMenu;
  template<typename It,typename Nav,typename Out>
  inline void printMenu(It& it,Nav& nav,Out& out,Ref ref,Idx n) {
    if (ref.len==1&&B::parentDraw(n)) Out::template printParent<It,Nav,Out>(it,nav,out);
    else if (ref.len) B::template printMenu<It,Nav,Out>(it,nav,out,ref,n);
    else Out::template printMenu<This,Nav,Out>(*this,nav,out);
  }
};

template<typename I>
class Item:public IItem,public I {
  public:
    using This=Item<I>;
    using I::I;
    // inline void enable(Idx n,bool e) const override {I::enable(n,e);}
    inline void enable(bool e) const override  {I::enable(e);}
    inline bool enabled() const override {return I::enabled();}
    inline bool enabled(Ref ref,Idx n) const override {return I::enabled(ref,n);};
    inline bool canNav() const override {return I::canNav();}
    inline bool parentDraw() const override {return I::parentDraw();}
    inline bool cmd(Cmds c,INav& nav) override {
      switch(c) {
        case Cmds::Activate: return I::template cmd<Cmds::Activate, This,INav>(*this,nav);
        case Cmds::Enter:    return I::template cmd<Cmds::Enter,    This,INav>(*this,nav);
        case Cmds::Esc:      return I::template cmd<Cmds::Esc,      This,INav>(*this,nav);
        case Cmds::Up:       return I::template cmd<Cmds::Up,       This,INav>(*this,nav);
        case Cmds::Down:     return I::template cmd<Cmds::Down,     This,INav>(*this,nav);
        case Cmds::Left:     return I::template cmd<Cmds::Left,     This,INav>(*this,nav);
        case Cmds::Right:    return I::template cmd<Cmds::Right,    This,INav>(*this,nav);
        case Cmds::None:
        default: return I::template cmd<Cmds::None,Item<I>,INav>(*this,nav);
      }
      // return I::template cmd<c,Item<I>,INav>(*this,nav,(Ref)nav,(Idx)nav);
    }
    using I::print;
    inline Idx size() const override {return I::size();}
    inline Idx size(Ref r) const {return I::size(r);}
    inline void print(IMenuOut& out) override {I::template print<IMenuOut>(out);}
    inline void printMenu(INav& nav,IMenuOut& out) override {
      I::template printMenu<Item,INav,IMenuOut>(*this,nav,out,nav,nav);
    }
    inline void printItems(INav& nav,IMenuOut& out) override {
      I::template printItems<IItem,INav,IMenuOut>(*this,nav,out);
    }
    void fmt(Roles r,bool io,INav& nav,IMenuOut& out,Idx n) override {
      switch(r) {
        case Roles::Panel:  io?I::template fmt<Roles::Panel ,true,INav,IMenuOut>(nav,out,n):I::template fmt<Roles::Panel ,false,INav,IMenuOut>(nav,out,n);break;
        case Roles::Menu:   io?I::template fmt<Roles::Menu  ,true,INav,IMenuOut>(nav,out,n):I::template fmt<Roles::Menu  ,false,INav,IMenuOut>(nav,out,n);break;
        case Roles::Title:  io?I::template fmt<Roles::Title ,true,INav,IMenuOut>(nav,out,n):I::template fmt<Roles::Title ,false,INav,IMenuOut>(nav,out,n);break;
        case Roles::Body:   io?I::template fmt<Roles::Body  ,true,INav,IMenuOut>(nav,out,n):I::template fmt<Roles::Body  ,false,INav,IMenuOut>(nav,out,n);break;
        case Roles::Prompt: io?I::template fmt<Roles::Prompt,true,INav,IMenuOut>(nav,out,n):I::template fmt<Roles::Prompt,false,INav,IMenuOut>(nav,out,n);break;
        case Roles::Index:  io?I::template fmt<Roles::Index ,true,INav,IMenuOut>(nav,out,n):I::template fmt<Roles::Index ,false,INav,IMenuOut>(nav,out,n);break;
        case Roles::Cursor: io?I::template fmt<Roles::Cursor,true,INav,IMenuOut>(nav,out,n):I::template fmt<Roles::Cursor,false,INav,IMenuOut>(nav,out,n);break;
        case Roles::Name:   io?I::template fmt<Roles::Name  ,true,INav,IMenuOut>(nav,out,n):I::template fmt<Roles::Name  ,false,INav,IMenuOut>(nav,out,n);break;
        case Roles::Mode:   io?I::template fmt<Roles::Mode  ,true,INav,IMenuOut>(nav,out,n):I::template fmt<Roles::Mode  ,false,INav,IMenuOut>(nav,out,n);break;
        case Roles::Value:  io?I::template fmt<Roles::Value ,true,INav,IMenuOut>(nav,out,n):I::template fmt<Roles::Value ,false,INav,IMenuOut>(nav,out,n);break;
        case Roles::Unit:   io?I::template fmt<Roles::Unit  ,true,INav,IMenuOut>(nav,out,n):I::template fmt<Roles::Unit  ,false,INav,IMenuOut>(nav,out,n);break;
        default:assert(false);break;
      }
    }
};

template<typename I=Empty<>>
class Menu:public I {
  public:
    using This=Menu<I>;
    Menu(IItem& title, IItem** data,size_t sz):title(title),data(data),sz(sz) {}
    template<typename Out> inline void print(Out& out) {title.print<Out>(out);}
    template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
    inline void print(It& it,Nav& nav,Out& out) {title.print<It,Nav,Out,P>(it,nav,out);}
    template<typename It,typename Nav,typename Out>
    inline void printMenu(It& it,Nav& nav,Out& out,Ref ref,Idx n) {
      trace(MDO<<"Menu::printMenu"<<endl);
      out.template printMenu<It,Nav,Out>(it,nav);
    }
    template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
    inline void printItems(It& it,Nav& nav,Out& out) {
      out.posTop(nav);
      trace(MDO<<"Menu::printItems out.top:"<<out.top()<<" free:"<<out.freeY()<<" sz:"<<sz<<endl);
      for(Idx n=out.top();n<sz&&out.freeY();n++) {
        out.clrLine(out.posY());
        out.template fmt<P,true,Nav,Out>(nav,out);
        // if (!Out::freeY()) return;
        // Out::clrLine(Out::posY());
        it.template fmt<Roles::Prompt,true ,Nav,Out>(nav,out,n);
        it.template fmt<Roles::Index, true ,Nav,Out>(nav,out,n);
        it.template fmt<Roles::Index, false,Nav,Out>(nav,out,n);
        it.template fmt<Roles::Cursor,true ,Nav,Out>(nav,out,n);
        it.template fmt<Roles::Cursor,false ,Nav,Out>(nav,out,n);
        data[n]->print<Out>(out);
        // reinterpret_cast<I*>(this)->template print<I,Nav,Out,Roles::Prompt>(
        //     *reinterpret_cast<I*>(this),nav,out
        // );
        it.template fmt<Roles::Prompt,false,Nav,Out>(nav,out,n);

        out.template fmt<P,false,Nav,Out>(nav,out);
      }
    }
    inline Idx size() const {return sz;}
    inline constexpr Idx size(Ref ref) const {return ref.len>1?size(ref.tail(),ref.head()):size();}
    inline constexpr Idx size(Ref ref,Idx n) const {return ref.len>1?data[n]->size(ref):size();}
  protected:
    IItem& title;
    IItem** data;
    size_t sz;
};
