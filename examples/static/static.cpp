#include <iostream>
using namespace std;

#include <menu.h>

using namespace AM5;

using Out=StaticPanel<0,0, 10,4,StdOutDef<cout>>;

const char* op1_text="Op 1";
const char* op2_text="Op 2";
const char* op3_text="Op 3";

template<const char** text>
using Op=StaticTextDef<text>;

const char* menu_title="Main menu";

using MainMenu=Cap<
  FullPrinterDef<
    TextFmt<
      NavPosDef<
        StaticTextDef<&menu_title,
          StaticMenuDataDef<
            Op<&op1_text>,
            Op<&op2_text>,
            Op<&op3_text>
          >
        >
      >
    >
  >
>;

MainMenu mainMenu;

int main(int,char**) {
  mainMenu.template printMenu<Out>();
}
