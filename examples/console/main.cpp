#include <iostream>
using namespace std;

#include <menu.h>
#include <menu/IO/console.h>
#include <menu/fmt/text.h>

namespace Menu {

  // enum class Role {Item};

  template<typename O>
  struct ItemPrinter:public O {
    using This=ItemPrinter<O>;
    template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    static inline void printMenu(ItemHead& menu) {
      // cout<<"ItemPrinter::printMenu"<<endl;
      This::template fmtItem<NavHead,OutHead,ItemHead,true,idx>();
      menu.template printTo<NavHead,OutHead,ItemHead,idx>();
      This::template fmtItem<NavHead,OutHead,ItemHead,false,idx>();
    }
  };

  template<
    template<typename> class P=Id,
    template<typename> class M=Id,
    template<typename> class T=Id,
    template<typename> class B=Id,
    template<typename> class I=ItemPrinter,
    typename O=Void<>
  > struct PrintersCfg:public O {
    struct Printers {
      template<typename X> using Panel=P<X>;
      template<typename X> using Menu=M<X>;
      template<typename X> using Title=T<X>;
      template<typename X> using Body=B<X>;
      template<typename X> using Item=I<X>;
    };
  };

  template<typename O,typename... OO>
  class StaticList:public StaticList<O> {
    public:
      using This=StaticList<O>;
      using Next=StaticList<OO...>;
      template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
      inline void printTo() {
        // cout<<"StaticList<O,OO...>::printTo"<<endl;
        This::template printTo<NavHead,OutHead,This,idx>();
        next.template printTo<NavHead,OutHead,Next,idx>();
      };
    protected:
      static Next next;
  };

  template<typename O,typename... OO>
  StaticList<OO...> StaticList<O,OO...>::next;

  template<typename O>
  struct StaticList<O>:public O {
    using This=StaticList<O>;
    // template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    // static inline void printItem() {
    //   using ItemPrinter=typename OutHead::Printers::template Item<This>;
    //   ItemPrinter::template out<NavHead,OutHead,This>();
    // }
    template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    inline void printTo() {
      // cout<<"StaticList<O>::printTo"<<endl;
      using ItemPrinter=typename OutHead::Printers::template Item<OutHead>;
      ItemPrinter::template printMenu<NavHead,OutHead,O,idx>(*this);
      // This::template printItem<NavHead,OutHead,This,idx>();
    }
  };

};

//**************************************************
using namespace Menu;

/* TODO: test this things
- single field menu
- multiple outputs/home/azevedo/Sketchbook/LIBDEV/ArduinoMenu/examples/console/main.cpp
- arduino and console
- static and dynamic stuff
- use fmt's
- use printers
- use panels, viewports and free outputs
- common stream operators
- minimal draw (check changed)
- parent draw (delegate draw)
- partial draw (output can draw areas)
- full draw (output draws entire page)
- raster draw (output need to be rasterized)
*/

using Out=ItemPrinter<TextFmt<Console<cout,PrintersCfg<>>>>;

const char* op1_text="Op 1";
const char* op2_text="Op ...";
const char* op3_text="Op 3";

using MainMenu=StaticList<
  StaticText<&op1_text>,
  StaticText<&op2_text>,
  StaticText<&op3_text>
>;

using Nav=NavNode<Out,MainMenu>;
Nav nav;

int main(int,const char**) {
  cout<<"AM5 test std::ostream"<<endl;
  nav.printMenu();
  return 0;
}
