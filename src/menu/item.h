/* -*- C++ -*- */
#pragma once

#include "api.h"

template<typename I>
struct Mutable:I {
  bool hasChanged=true;
  using I::I;
  inline bool changed() const {return hasChanged;}
  inline void changed(bool o) {hasChanged=o;}
  // inline void changed(Idx,bool o) {changed(o);}
};

/// ActionHanlder, type of action functions to associate with items
using ActionHandler=bool (*)();

/**
* The Action class associates an actikon function with a menu item.
*/
template<ActionHandler act,typename I>
struct Action:I {
  using Base=I;
  using I::I;
  using This=Action<act,I>;
  inline static bool activate() {return act();}
  // inline static bool activate(PathRef ref,Idx n) {return ref?false:act();}
};

template<const char** text>
struct StaticText:Obj<StaticText<text>> {
  template<typename I>
  struct As:I {
    template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
    inline static void print(Nav& nav,Out& out,PathRef ref=self) {out.raw(text[0]);}
  };
};

template<typename I>
struct Text:I {
  using Base=I;
  const char* text;
  inline Text(const char*o):text(o) {}
  template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
  inline void print(Nav& nav,Out& out,PathRef ref=self) {
    if (op==Op::Printing) out.raw(text);
    I::template print<Nav,Out,op>(nav,out);
  }
};

/**
* The Item class encapsulates a composition to be a stratic menu item.
*/
template<Expr... I>
struct Item:Chain<Mutable,I...,Empty>::template To<Obj<Item<I...>>> {
  using Base=typename Chain<Mutable,I...,Empty>::template To<Obj<Item<I...>>>;
  using This=Item<I...>;
  using Base::Base;
};

template<typename F,typename S>
struct Pair {
  template<typename I>
  struct As:I {
    using Base=I;
    using This=Pair<F,S>::As<I>;
    using I::I;
    S tail;
    //size ---------------------------------------------------------------------
    // inline constexpr Idx size() const {return tail.size()+1;}
  };
};

template<typename Title,typename Body,Title& title,Body& body>
struct StaticMenu {
  template<typename I>
  struct As:I {
    using This=StaticMenu<Title,Body,title,body>::As<I>;
    using I::I;

    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void printMenu(Nav& nav,Out& out) {
      trace(MDO<<"StaticMenu::printMenu "<<op<<endl);
      out.template printMenu<decltype(I::obj()),Nav,op>(I::obj(),nav);
    }

    template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
    inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0,PathRef ref=self)
      {body.template printItems<Nav,Out,op,role>(nav,out,idx,top,ref);}

    template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
    inline void print(Nav& nav,Out& out,PathRef ref=self) {
      trace(MDO<<"StaticMenu::print "<<role<<endl);
      if (role&(Roles::Title|Roles::Raw)) title.print(nav,out);
      // else out.printMenu(I::obj(),nav);
    }

  };
};

// template<typename Title,typename Body,Title& title,Body& body>
// struct StaticMenu {
//   template<typename I=Title>
//   struct As:Chain<Mutable,Pair<Title,Body>::template As>::template To<Obj<StaticMenu<Title,Body,title,body>::As<I>>> {
//     using This=typename StaticMenu<Title,Body,title,body>::template As<I>;
//     using Base=typename Chain<Mutable,Pair<Title,Body>::template As>::template To<Obj<StaticMenu<Title,Body,title,body>::As<I>>>;
//     template<typename Nav,typename Out,Op op=Op::Printing>
//     inline void printMenu(Nav& nav,Out& out) {
//       _trace(MDO<<"StaticMenu::printMenu "<<op<<endl);
//       out.template printMenu<decltype(body),Nav,op>(body,nav);
//     }
//
//     template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
//     inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0) {
//       body.template printItems<Nav,Out,op,role>(nav,out,idx,top);
//     }
//     template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
//     inline void print(Nav& nav,Out& out,PathRef ref=self) {
//       _trace(MDO<<"StaticMenu::print "<<role<<endl);
//       if (role&(Roles::Title|Roles::Raw)) title.print(nav,out);
//       // else out.printMenu(I::obj(),nav);
//     }
//   };
//   template<typename O> using Open=As<O>;
// };

struct IItem {
  // virtual inline Idx len(PathRef=self)=0;
  virtual inline void printMenu(INav& nav,IOut& out,Op op=Op::Printing)=0;
  virtual inline void printItems(INav&,IOut&,Idx=0,Idx=0,PathRef=self,Op op=Op::Printing,Roles role=Roles::Raw)=0;
  virtual inline void print(INav&,IOut&,Op op,Roles role,PathRef=self)=0;
  virtual inline bool enabled(PathRef=self)=0;
  virtual inline void enable(bool,PathRef=self)=0;
  virtual inline bool changed() const=0;
  virtual inline void changed(bool o)=0;

  template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
  inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0,PathRef ref=self)
    {printItems(nav,out,idx,top,ref,op,role);}
  template<typename Nav,typename Out,Op op=Op::Printing>
  inline void printMenu(Nav& nav,Out& out) {printMenu(nav,out,op);}
  template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
  inline void print(Nav& nav,Out& out,PathRef ref=self) {print(nav,out,op,role,ref);}
};

template<Expr... I>
struct Prompt:IItem,Chain<Mutable,I...,Empty>::template To<Obj<Prompt<I...>>> {
  using Base=typename Chain<Mutable,I...,Empty>::template To<Obj<Prompt<I...>>>;
  using This=Prompt<I...>;
  using Base::Base;
  using Base::printItems;
  // using Base::printMenu;
  using Base::print;
  // inline Idx len(PathRef ref=self) override {return O::len(ref);}
  inline void printMenu(INav& nav,IOut& out,Op op=Op::Printing) override {
    switch(op) {
      case Op::Measure: Base::template printMenu<INav,IOut,Op::Measure>(nav,out);break;
      case Op::Printing: Base::template printMenu<INav,IOut,Op::Printing>(nav,out);break;
      case Op::ClearChanges: Base::template printMenu<INav,IOut,Op::ClearChanges>(nav,out);break;
    }
  }
  inline void printItems(INav& nav,IOut& out,Idx idx=0,Idx top=0,PathRef ref=self,Op op=Op::Printing,Roles role=Roles::Raw) override {
    trace(MDO<<"Prompt::printItems"<<endl);
    switch(op) {
      case Op::Measure:
      switch(role) {
        case Roles::Raw: Base::template printItems<INav,IOut,Op::Measure,Roles::Raw>(nav,out,idx,top,ref);break;
        case Roles::Panel: Base::template printItems<INav,IOut,Op::Measure,Roles::Panel>(nav,out,idx,top,ref);break;
        case Roles::Menu: Base::template printItems<INav,IOut,Op::Measure,Roles::Menu>(nav,out,idx,top,ref);break;
        case Roles::Title: Base::template printItems<INav,IOut,Op::Measure,Roles::Title>(nav,out,idx,top,ref);break;
        case Roles::Body: Base::template printItems<INav,IOut,Op::Measure,Roles::Body>(nav,out,idx,top,ref);break;
        case Roles::Item: Base::template printItems<INav,IOut,Op::Measure,Roles::Item>(nav,out,idx,top,ref);break;
        case Roles::Index: Base::template printItems<INav,IOut,Op::Measure,Roles::Index>(nav,out,idx,top,ref);break;
        case Roles::Cursor: Base::template printItems<INav,IOut,Op::Measure,Roles::Cursor>(nav,out,idx,top,ref);break;
        case Roles::Name: Base::template printItems<INav,IOut,Op::Measure,Roles::Name>(nav,out,idx,top,ref);break;
        case Roles::Mode: Base::template printItems<INav,IOut,Op::Measure,Roles::Mode>(nav,out,idx,top,ref);break;
        case Roles::Value: Base::template printItems<INav,IOut,Op::Measure,Roles::Value>(nav,out,idx,top,ref);break;
        case Roles::Unit: Base::template printItems<INav,IOut,Op::Measure,Roles::Unit>(nav,out,idx,top,ref);break;
      };break;
      case Op::Printing:
      switch(role) {
        case Roles::Raw: Base::template printItems<INav,IOut,Op::Printing,Roles::Raw>(nav,out,idx,top,ref);break;
        case Roles::Panel: Base::template printItems<INav,IOut,Op::Printing,Roles::Panel>(nav,out,idx,top,ref);break;
        case Roles::Menu: Base::template printItems<INav,IOut,Op::Printing,Roles::Menu>(nav,out,idx,top,ref);break;
        case Roles::Title: Base::template printItems<INav,IOut,Op::Printing,Roles::Title>(nav,out,idx,top,ref);break;
        case Roles::Body: Base::template printItems<INav,IOut,Op::Printing,Roles::Body>(nav,out,idx,top,ref);break;
        case Roles::Item: Base::template printItems<INav,IOut,Op::Printing,Roles::Item>(nav,out,idx,top,ref);break;
        case Roles::Index: Base::template printItems<INav,IOut,Op::Printing,Roles::Index>(nav,out,idx,top,ref);break;
        case Roles::Cursor: Base::template printItems<INav,IOut,Op::Printing,Roles::Cursor>(nav,out,idx,top,ref);break;
        case Roles::Name: Base::template printItems<INav,IOut,Op::Printing,Roles::Name>(nav,out,idx,top,ref);break;
        case Roles::Mode: Base::template printItems<INav,IOut,Op::Printing,Roles::Mode>(nav,out,idx,top,ref);break;
        case Roles::Value: Base::template printItems<INav,IOut,Op::Printing,Roles::Value>(nav,out,idx,top,ref);break;
        case Roles::Unit: Base::template printItems<INav,IOut,Op::Printing,Roles::Unit>(nav,out,idx,top,ref);break;
      };break;
      case Op::ClearChanges:
      switch(role) {
        case Roles::Raw: Base::template printItems<INav,IOut,Op::ClearChanges,Roles::Raw>(nav,out,idx,top,ref);break;
        case Roles::Panel: Base::template printItems<INav,IOut,Op::ClearChanges,Roles::Panel>(nav,out,idx,top,ref);break;
        case Roles::Menu: Base::template printItems<INav,IOut,Op::ClearChanges,Roles::Menu>(nav,out,idx,top,ref);break;
        case Roles::Title: Base::template printItems<INav,IOut,Op::ClearChanges,Roles::Title>(nav,out,idx,top,ref);break;
        case Roles::Body: Base::template printItems<INav,IOut,Op::ClearChanges,Roles::Body>(nav,out,idx,top,ref);break;
        case Roles::Item: Base::template printItems<INav,IOut,Op::ClearChanges,Roles::Item>(nav,out,idx,top,ref);break;
        case Roles::Index: Base::template printItems<INav,IOut,Op::ClearChanges,Roles::Index>(nav,out,idx,top,ref);break;
        case Roles::Cursor: Base::template printItems<INav,IOut,Op::ClearChanges,Roles::Cursor>(nav,out,idx,top,ref);break;
        case Roles::Name: Base::template printItems<INav,IOut,Op::ClearChanges,Roles::Name>(nav,out,idx,top,ref);break;
        case Roles::Mode: Base::template printItems<INav,IOut,Op::ClearChanges,Roles::Mode>(nav,out,idx,top,ref);break;
        case Roles::Value: Base::template printItems<INav,IOut,Op::ClearChanges,Roles::Value>(nav,out,idx,top,ref);break;
        case Roles::Unit: Base::template printItems<INav,IOut,Op::ClearChanges,Roles::Unit>(nav,out,idx,top,ref);break;
      };break;
    }
  }
  inline void print(INav& nav,IOut& out,Op op,Roles role,PathRef ref=self) override {
    trace(MDO<<"Prompt::print"<<endl);
    switch(op) {
      case Op::Measure:
        switch(role) {
          case Roles::Raw: Base::template print<INav,IOut,Op::Measure,Roles::Raw>(nav,out,ref);break;
          case Roles::Panel: Base::template print<INav,IOut,Op::Measure,Roles::Panel>(nav,out,ref);break;
          case Roles::Menu: Base::template print<INav,IOut,Op::Measure,Roles::Menu>(nav,out,ref);break;
          case Roles::Title: Base::template print<INav,IOut,Op::Measure,Roles::Title>(nav,out,ref);break;
          case Roles::Body: Base::template print<INav,IOut,Op::Measure,Roles::Body>(nav,out,ref);break;
          case Roles::Item: Base::template print<INav,IOut,Op::Measure,Roles::Item>(nav,out,ref);break;
          case Roles::Index: Base::template print<INav,IOut,Op::Measure,Roles::Index>(nav,out,ref);break;
          case Roles::Cursor: Base::template print<INav,IOut,Op::Measure,Roles::Cursor>(nav,out,ref);break;
          case Roles::Name: Base::template print<INav,IOut,Op::Measure,Roles::Name>(nav,out,ref);break;
          case Roles::Mode: Base::template print<INav,IOut,Op::Measure,Roles::Mode>(nav,out,ref);break;
          case Roles::Value: Base::template print<INav,IOut,Op::Measure,Roles::Value>(nav,out,ref);break;
          case Roles::Unit: Base::template print<INav,IOut,Op::Measure,Roles::Unit>(nav,out,ref);break;
        };break;
      case Op::Printing:
        switch(role) {
          case Roles::Raw: Base::template print<INav,IOut,Op::Printing,Roles::Raw>(nav,out,ref);break;
          case Roles::Panel: Base::template print<INav,IOut,Op::Printing,Roles::Panel>(nav,out,ref);break;
          case Roles::Menu: Base::template print<INav,IOut,Op::Printing,Roles::Menu>(nav,out,ref);break;
          case Roles::Title: Base::template print<INav,IOut,Op::Printing,Roles::Title>(nav,out,ref);break;
          case Roles::Body: Base::template print<INav,IOut,Op::Printing,Roles::Body>(nav,out,ref);break;
          case Roles::Item: Base::template print<INav,IOut,Op::Printing,Roles::Item>(nav,out,ref);break;
          case Roles::Index: Base::template print<INav,IOut,Op::Printing,Roles::Index>(nav,out,ref);break;
          case Roles::Cursor: Base::template print<INav,IOut,Op::Printing,Roles::Cursor>(nav,out,ref);break;
          case Roles::Name: Base::template print<INav,IOut,Op::Printing,Roles::Name>(nav,out,ref);break;
          case Roles::Mode: Base::template print<INav,IOut,Op::Printing,Roles::Mode>(nav,out,ref);break;
          case Roles::Value: Base::template print<INav,IOut,Op::Printing,Roles::Value>(nav,out,ref);break;
          case Roles::Unit: Base::template print<INav,IOut,Op::Printing,Roles::Unit>(nav,out,ref);break;
        };break;
      case Op::ClearChanges:
        switch(role) {
          case Roles::Raw: Base::template print<INav,IOut,Op::ClearChanges,Roles::Raw>(nav,out,ref);break;
          case Roles::Panel: Base::template print<INav,IOut,Op::ClearChanges,Roles::Panel>(nav,out,ref);break;
          case Roles::Menu: Base::template print<INav,IOut,Op::ClearChanges,Roles::Menu>(nav,out,ref);break;
          case Roles::Title: Base::template print<INav,IOut,Op::ClearChanges,Roles::Title>(nav,out,ref);break;
          case Roles::Body: Base::template print<INav,IOut,Op::ClearChanges,Roles::Body>(nav,out,ref);break;
          case Roles::Item: Base::template print<INav,IOut,Op::ClearChanges,Roles::Item>(nav,out,ref);break;
          case Roles::Index: Base::template print<INav,IOut,Op::ClearChanges,Roles::Index>(nav,out,ref);break;
          case Roles::Cursor: Base::template print<INav,IOut,Op::ClearChanges,Roles::Cursor>(nav,out,ref);break;
          case Roles::Name: Base::template print<INav,IOut,Op::ClearChanges,Roles::Name>(nav,out,ref);break;
          case Roles::Mode: Base::template print<INav,IOut,Op::ClearChanges,Roles::Mode>(nav,out,ref);break;
          case Roles::Value: Base::template print<INav,IOut,Op::ClearChanges,Roles::Value>(nav,out,ref);break;
          case Roles::Unit: Base::template print<INav,IOut,Op::ClearChanges,Roles::Unit>(nav,out,ref);break;
        };break;
    }
  }
  inline bool enabled(PathRef ref=self) override {return Base::enabled(ref);}
  inline void enable(bool o,PathRef ref=self) override {Base::enable(o,ref);}
  virtual inline bool changed() const {return Base::changed();}
  virtual inline void changed(bool o) {Base::changed(o);}
};
