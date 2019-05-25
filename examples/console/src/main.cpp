////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// ArduinoMenu console menu example

#include <menu.h>
#include <menu/comp/endis.h>
#include <menu/comp/vector.h>
#include <menu/comp/numField.h>
#include <menu/IO/consoleOut.h>
#include <menu/fmt/titleWrap.h>
#include <menu/fmt/textFmt.h>

// using namespace Menu;

template<typename O>
using TitleWrap=TitleWrapFmt<O>;//default wrap

using Out=Chain<
  TextFmt,
  TitleWrap,
  FullPrinter,
  Viewport,
  RangePanel
>::To<StaticPanel<0,0,20,5,Console<>>>;

const char* singleOp_text="Just testing";
using SingleOp=EnDis<StaticText<&singleOp_text>>;

bool hey() {
  cout<<"Hey!"<<endl;
  return false;
}

bool grrr() {
  cout<<"This should not be called as the option is disabled"<<endl;
  return false;
}

int year=2019;

const char* op1_text="Op 1";
const char* op2_text="Op ...";
const char* op3_text="Op 3";
const char* year_text="Year";
const char* zzz="zZz";
const char* extra_text="extra option";
const char* mainMenu_title="Main menu";
template<const char**text,typename O=Empty<>>
using Op=EnDis<StaticText<text,O>>;
using MainMenu=
  StaticText<
    &mainMenu_title,
    StaticMenu<
      Action<Op<&op1_text>,hey>,
      Action<Op<&op2_text>,grrr>,
      Op<
        &year_text,
        AsMode<
          AsValue<
            NavHandler<
              NumField<
                int,
                year,
                1900,2100,
                10,1,
                AsUnit<StaticText<&zzz>>
              >
            >
          >
        >
      >,
      Op<&op2_text>,
      Op<&op2_text>,
      Op<&op2_text>,
      Op<&op2_text>,
      Op<&op2_text>,
      Op<&op2_text>,
      Op<&op2_text>,
      Op<&op3_text>
    >
  >;

using DynaMenu=
  Prompt<
    StaticText<
      &mainMenu_title,
      VectorMenu<>
    >
  >;

DynaMenu dynaMenu(
  new Prompt<Action<Op<&op1_text>,hey>>(),
  new Prompt<Action<Op<&op2_text>,grrr>>(),
  new Prompt<Op<&year_text>>()
);

Out out;//to use with single option
StaticNav<Out,SingleOp> singleNav;
NavRoot<
  ItemNav<
    StaticNav<Out,MainMenu,NavPos<>>
  >
> nav;
DynamicNav<
  MenuOutDef<Out>,
  DynaMenu,
  ItemNav<NavPos<>>
> dyNav(dynaMenu);

int main() {
  cout<<"AM5 tests"<<endl;
  dynaMenu.enable(1,false);

  cout<<"{single option test}"<<endl;
  SingleOp().print(singleNav,out);
  cout<<endl;

  cout<<"{static menu test}"<<endl;
  nav.enable(1,false);//disable second option
  nav.up();
  nav.up();
  nav.printMenu();
  nav.enter();
  nav.up();
  nav.up();
  cout<<endl;

  cout<<"{dynamic menu test}"<<endl;
  dyNav.enable(1,false);//disable second option
  dyNav.printMenu();
  cout<<"{adding option}"<<endl;
  dynaMenu.push_back(new Prompt<StaticText<&extra_text>>());
  dyNav.printMenu();
  dyNav.enter();
  cout<<endl<<"-------"<<endl;
  cout<<"{navigate up (index)}"<<endl;
  dyNav.up();
  dyNav.printMenu();
  dyNav.enter();
  dyNav.up();
  dyNav.printMenu();
  dyNav.enter();
  cout<<endl<<"-------"<<endl;
}
