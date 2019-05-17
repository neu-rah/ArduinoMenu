template<typename O,typename... OO>
class OutList:public OutList<O> {
  public:
    using This=OutList<O,OO...>;
    using O::O;
    //this works because printer head is never taken at this level
    //so dont do it!
    inline void newView() {
      // Serial<<"OutList::newView "<<This::onMenuRender()<<endl;
      O::newView();
      // if (Nav::onMenuRender()) next.newView();
    }
    inline void nl() {
      O::nl();
      next.nl();
    }
    template<typename Nav,typename T>
    inline void raw(T o) {
      O::template raw<Nav,T>(o);
      //without this global print hits only the first device
      //with it menus will chain printing to next devices creating chaos
      if (!Nav::onMenuRender()) next.template raw<Nav,T>(o);//chain printing to all devices!
    }
    template<typename Nav,typename Raw>
    inline void printMenu() {
      // Serial<<"..."<<This::onMenuRender()<<endl;
      OutList<O>::template printMenu<Nav,Raw>();
      next.newView();
      // Serial<<"..."<<endl;
      next.template printMenu<Nav,Raw>();
    }
    // template<typename P>
    // inline void printMenuRaw(MenuOut& menuOut,P p,Item&i) {
    //   assert(O::onMenuRender());
    //   O::printMenuRaw(menuOut,p,i);
    //   next.newView();
    //   next.printMenuRaw(next,PrintHead<OutList<OO...>>{/*next,*/next,0},i);
    // }
    // inline void setTarget(Item& i) {
    //   O::setTarget(i);
    //   assert(O::sharedNav());
    //   //next.setTarget(i);
    // }
  protected:
    OutList<OO...> next;
};

template<typename O>
class OutList<O>:public O {
  public:
    using O::O;
    template<typename Nav,typename Raw>
    inline void printMenu() {
      // Serial<<"OutList<O>::printMenu()"<<Nav::onMenuRender()<<endl;
      // assert(onMenuRender());
      O::template printMenu<Nav,Raw>();
    }
};
