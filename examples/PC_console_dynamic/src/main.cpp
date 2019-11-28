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

bool quit() {running=false;}

bool someAction() {
  cout<<"Some action!"<<endl;
  return true;
}

Prompt<Action<someAction,Text<>>> ok("Ok");
Prompt<Action<someAction,Text<>>> cancel("Cancel");

IItem* subMenu_data[]{&ok,&cancel};
IterableData<ArrayData<IItem,subMenu_data,sizeof(subMenu_data)/sizeof(decltype(subMenu_data[0]))>> subMenu_body;
Prompt<StaticMenu<Text<>,decltype(subMenu_body)>> subMenu("Mix-menu");

const char* op1_text="Option A";
Prompt<Action<someAction,StaticText<&op1_text>>> op1;
Prompt<Text<>> op2("Option B");
Prompt<Text<>> op3("Option C");
Prompt<Action<quit,Text<>>> op_quit("<Quit!");

IItem* mainMenu_data[]{&op1,&op2,&op3,&subMenu,&op_quit};
IterableData<ArrayData<IItem,mainMenu_data,sizeof(mainMenu_data)/sizeof(decltype(mainMenu_data[0]))>> mainMenu_body;

const char* mainMenu_title="Main menu";
StaticMenu<StaticText<&mainMenu_title>,decltype(mainMenu_body)> mainMenu;

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
NavRoot<Nav<decltype(mainMenu),mainMenu,2>> nav;

int main() {
  // ok.print(out);
  // out.nl();
  // ok.activate();
  // out.nl();
  ///////////////
  nav.printMenu(out);
  nav.enter();
  while(running) {
    if (nav.doInput(in,nav)) nav.printMenu(out);
    cout.flush();
  };
  out.nl();
  return 0;
}
