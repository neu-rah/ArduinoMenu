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

IterableData<ArrayData<IItem,menu_data,sizeof(menu_data)/sizeof(menu_data[0])>> mainMenu_data;

const char* mainMenu_title="Main menu";
StaticMenu<StaticText<&mainMenu_title>,decltype(mainMenu_data)> mainMenu;

//menu input --------------------------------------
LinuxKeyIn in;
//menu output -------------------------------------
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
  out.nl();
  ///////////////
  nav.printMenu(out);
  while(running) {
    if (nav.doInput(in)) nav.printMenu(out);
    cout.flush();
  };
  out.nl();
  return 0;
}
