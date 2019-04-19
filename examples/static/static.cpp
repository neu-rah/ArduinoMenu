#include <menu.h>
using namespace AM5;

//static strings
const char* op1_text="Op 1";
const char* op2_text="Op 2";
const char* op3_text="Op 3";
const char* menu_title="Main menu";

using Out=FullPrinterDef<
  TextFmt<
    RangePanel<
      StaticPanel<0,0,10,4,StdOutDef<cout>>
    >
  >
>;

template<const char** text>
using Op=StaticTextDef<text>;

using MainMenu=StaticTextDef<&menu_title,
  StaticMenuDataDef<
    Op<&op1_text>,
    Op<&op2_text>,
    Op<&op3_text>
  >
>;

using Nav=NavDef<Out,MainMenu>;
Nav::Root<Nav::PosDef<>> nav;

int main(int,char**) {
  nav.printMenu();
}
