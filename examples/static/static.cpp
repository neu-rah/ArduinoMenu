#include <iostream>
using namespace std;

#include <menu.h>

using namespace AM5;

const char* op1_text="Op 1";
const char* op2_text="Op 2";
const char* op3_text="Op 3";

template<const char** text>
using Op=StaticTextDef<text,StdOutDef<cout>>;

const char* menu_title="Main menu";

using mainMenu=Cap<
  TextFmt<
    FullPrinterDef<
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

int main(int,char**) {
  mainMenu::printMenu();
}
