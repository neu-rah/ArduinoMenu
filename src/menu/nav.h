/* -*- C++ -*- */
#pragma once
#include "api.h"

template<typename Idx=Idx,typename N=Drift<>>
struct NavPos:N {
  template<typename It,typename Nav,typename Out>
  inline static void print(It& it,Nav& nav,Out& out,Ref ref,Idx n) {
    it.template print<It,Nav,Out>(it,nav,out,ref,n);
  }
  inline Idx pos() const {return at;}
  inline void setPos(Idx n) {at=n;}
  inline bool selected(Idx idx) const {return at==idx;}
  operator Idx() const {return pos();}
  template<Cmds c,typename It,typename Nav>
  inline bool _cmd(It& it,Nav& nav) {
    trace(MDO<<"NavPos::_cmd "<<c<<" from "<<at<<" size:"<<it.size((Ref)nav)<<endl);
    switch(c) {
      case Cmds::Up:
        if (at<(it.size((Ref)nav)-1)) at++;
        else return false;
        return true;
      case Cmds::Down:
        if (at>0) at--;
        else return false;
        return true;
    }
    trace(MDO<<"NavPos::_cmd to:"<<at<<endl);
    return N::template _cmd<c,It,Nav>(it,nav);
  }
  Idx at=0;
};

template<typename Data,Idx max_depth,typename N=Drift<>>
class NavTreeBase:public N {
  public:
    inline NavTreeBase(Data& o):data(o) {}
    operator Ref() const {return Ref{level,path};}
    inline Modes mode() const {return state;}
    inline void setMode(Modes m) {state=m;}
    inline Idx pos() const {return path[level];}
    inline void setPos(Idx n) {path[level]=n;}
    inline bool selected(Idx idx) const {return path[level]==idx;}
    inline operator Idx() const {return pos();}
    inline Idx size() {return size(operator Ref());}
    inline Idx size(Ref ref) {return data.size(ref);}
    template<Cmds c,typename It,typename Nav>
    inline bool _cmd(It& it,Nav& nav) {
      trace(MDO<<"NavTreeBase::_cmd "<<c<<" from "<<pos()<<" size:"<<size()<<endl);
      switch(c) {
        case Cmds::Up:
          if (path[level]<(it.size()-1)) path[level]++;
          else return false;
          return true;
        case Cmds::Down:
          if (path[level]>0) path[level]--;
          else return false;
          return true;
      }
      return N::template _cmd<c,It,Nav>(it,nav);
    }

    inline void debug_path() {
      trace(
        MDO<<"  nav path:[";
        for(int n=0;n<=level;n++) MDO<<(n?",":"")<<path[n];
        MDO<<"]"<<endl;
        MDO<<"  target size:"<<size((Ref)*this)<<endl;
      );
    }
    inline bool enabled(Idx n) const {return data.enabled(*this,n);}
    inline bool enabled() const {return data.enabled(pos());}

    inline void open() {
      trace(MDO<<"NavTreeBase::open"<<endl);
      trace({
        Ref ref=*this;
        // assert(data.canNav(*this));
        MDO<<"yeah it handles cmds"<<endl;
        MDO<<"and we have depth level to focus"<<endl;
      })
      assert(level<max_depth-1);
      if (level>=max_depth-1) return;
      level++;
      path[level]=0;
      // debug_path();
    }

    inline void close() {
      trace(MDO<<"NavTreeBase::close"<<endl);
      state=Modes::Normal;
      if (level>0) level--;
    }
    Idx level=0;
  protected:
    Idx path[max_depth];
    Data& data;
    Modes state=Modes::Normal;
};

template<typename Data,Idx max_depth,typename N=Drift<>>
struct StaticNavTree:public NavTreeBase<Data,max_depth,N> {
  // inline StaticNavTree(Data& o):data(o) {}
  using This=StaticNavTree<Data,max_depth,N>;
  using Base=NavTreeBase<Data,max_depth,N>;
  using Base::Base;
  template<typename Nav,typename Out>
  inline void print(Nav& nav,Out& out) {
    Ref ref=nav;
    Base::data.template printMenu<Data,Nav,Out>(Base::data,nav,out,ref,ref.head());
  }
  template<Cmds c,typename Nav>
  inline bool cmd(Nav& nav) {
    if (c==Cmds::Esc) {
      This::close();
      return true;
    }
    Ref ref=*this;
    trace(MDO<<"Data->cmd:"<<c<<" to level:"<<level<<" idx:"<<path[level]<<endl);
    // debug_path();
    bool res=Base::data.template cmd<c,Data,Nav>(Base::data,nav,ref,ref.head());
    if (c==Cmds::Enter&&!res) {
      trace(MDO<<"StaticNavTree calling close by "<<c<<" returning false"<<endl);
      This::close();
    }
    return true;
  }

};

template<Idx max_depth,typename N=Drift<>>
struct NavTree:NavTreeBase<IItem,max_depth,N> {
  using This=NavTree<max_depth,N>;
  using Base=NavTreeBase<IItem,max_depth,N>;
  using Base::Base;
  // template<typename Nav,typename Out>
  inline void print(INav& nav,IMenuOut& out) {
    Base::data.printMenu(nav,out);//,ref,ref.head());
  }
  inline bool cmd(Cmds c,INav& nav) {
    if (c==Cmds::Esc) {
      This::close();
      return true;
    }
    Ref ref=*this;
    trace(MDO<<"Data->cmd:"<<c<<" to level:"<<Base::level<<" idx:"<<Base::path[Base::level]<<endl);
    // debug_path();
    bool res=Base::data.cmd(c,nav,nav,nav);
    // bool res=Base::data.template cmd<c,IItem,This>(Base::data,nav,(Ref)nav,(Idx)nav);
    if (c==Cmds::Enter&&!res) {
      trace(MDO<<"StaticNavTree calling close by "<<c<<" returning false"<<endl);
      This::close();
    }
    return true;
  }

};

template<typename N>
struct StaticRoot:N {
  using N::N;
  using This=StaticRoot<N>;

  using N::print;
  template<typename Out> inline void print(Out& out) {
    N::template print<This,Out>(*this,out);
  }

  inline bool enter() {return N::template cmd<Cmds::Enter,This>(*this);}
  inline bool esc() {return N::template cmd<Cmds::Esc,This>(*this);}
  inline bool up() {return N::template cmd<Cmds::Up,This>(*this);}
  inline bool down() {return N::template cmd<Cmds::Down,This>(*this);}
  inline bool left() {return N::template cmd<Cmds::Left,This>(*this);}
  inline bool right() {return N::template cmd<Cmds::Right,This>(*this);}

  template<typename In>
  inline bool doInput() {
    return In::cmd(*this);
  }
};

template<typename N>
class NavRoot:public INav,public N {
  public:
    using This=NavRoot<N>;
    using N::N;
    virtual inline Modes mode() const {return N::mode();}
    virtual inline void setMode(Modes m) {N::setMode(m);}
    inline bool selected(Idx i) const override {return N::selected(i);}
    inline bool enabled(Idx i) const override {return N::enabled(i);}
    inline Idx pos() const override {return N::pos();};
    inline void setPos(Idx n) override {N::setPos(n);};
    inline void printOn(IMenuOut& out) override {N::print(*this,out);};
    using N::cmd;
    inline bool cmd(Cmds c) override {return N::cmd(c,*this);};
    inline bool cmd(Cmds c,IItem& it) override {
      switch(c) {
        case Cmds::Activate:return N::template _cmd<Cmds::Activate,IItem,This>(it,*this);
        case Cmds::Enter:return N::template _cmd<Cmds::Enter,IItem,This>(it,*this);
        case Cmds::Esc:return N::template _cmd<Cmds::Esc,IItem,This>(it,*this);
        case Cmds::Up:return N::template _cmd<Cmds::Up,IItem,This>(it,*this);
        case Cmds::Down:return N::template _cmd<Cmds::Down,IItem,This>(it,*this);
        case Cmds::Left:return N::template _cmd<Cmds::Left,IItem,This>(it,*this);
        case Cmds::Right:return N::template _cmd<Cmds::Right,IItem,This>(it,*this);
        case Cmds::None:
        default: return N::template _cmd<Cmds::None,IItem,This>(it,*this);
      }
    }
    inline void open() override {N::open();};
    inline void close() override {N::close();};
    inline bool doInput(IMenuIn& in) {return in.cmd(*this);}
    operator Ref() const override {return N::operator Ref();}
};
