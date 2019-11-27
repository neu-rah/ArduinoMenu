#include <iostream>
using namespace std;

//main include for ArduinoMenu
#include <menu.h>

//input/output drivers --------------------------------------
#include <menu/IO/consoleOut.h>
#include <menu/IO/linuxKeyIn.h>
//format specifyers -----------------------------------------
#include <menu/fmt/fullText.h>//to draw index and text cursors (nav and edit)
#include <menu/fmt/ANSI.h>//to draw index and text cursors (nav and edit)
#include <menu/fmt/titleWrap.h>
//components ------------------------------------------------
#include <menu/comp/endis.h>
#include <menu/comp/numField.h>

bool running=true;

bool someAction() {
  cout<<"Some action!"<<endl;
  return true;
}

Prompt<Action<someAction,Text<>>> ok("Ok");
Prompt<Action<someAction,Text<>>> cancel("Cancel");

IItem* menu_data[]{&ok,&cancel};

template<typename T,Idx sz,typename I=Empty>
struct ArrayData:I {
  T** data;
  inline constexpr Idx size() const {return sz;}
  T& operator[](Idx n) const {return *data[n];}
};

template<typename T,typename I=Empty>
struct IterableData:I {
  T data;
  inline Idx size() const {return data.size();}
  template<typename Nav,typename Out,Roles role=Roles::Item,OutOp op=OutOp::Printing>
  inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0,bool fullPrint=true) {
    for(int n=top;n<size()&&out.freeY();n++) {
      switch(op) {
        case OutOp::Measure:
          out.template printItem<IItem,Out,false>(data[n],out,idx,nav.selected(idx),data[n].enabled(),nav.mode());
          break;
        case OutOp::Printing:
          if (fullPrint||data[n].changed()||!out.partialDraw())
            out.template printItem<IItem,Out,true>(data[n],out,idx,nav.selected(idx),data[n].enabled(),nav.mode());
          else
            out.template printItem<IItem,Out,false>(data[n],out,idx,nav.selected(idx),data[n].enabled(),nav.mode());
          break;
        case OutOp::ClearChanges:
          data[n].changed(false);
      }
    }
  }
};

IterableData<ArrayData<IItem,sizeof(menu_data)/sizeof(menu_data[0])>> mainMenu_data;

const char* mainMenu_title="Main menu";
StaticMenu<StaticText<&mainMenu_title>,decltype(mainMenu_data)> mainMenu;

using Out=MenuOut<
  FullPrinter<//print title and items
    Fmt<//formating API
      TitleWrapFmt<//put [] around menu title
        TextFmt<//apply text formating
          RangePanel<//scroll content on output geometry
            StaticPanel<50,2,20,6,Console>//describe output geometry and device
          >
        >
      >
    >
  >
>;

Out out;

//navigation root ---------------------------------
Nav<decltype(mainMenu),mainMenu,2> nav;

int main() {
  ok.print(out);
  out.nl();
  ok.activate();

  mainMenu.printItems<decltype(nav),Out>(nav,out);
  return 0;
}
