#include <iostream>
using namespace std;
#include <unity.h>
#include <menu.h>
#include <menu/IO/consoleOut.h>
#include <menu/IO/stringOut.h>
#include <menu/fmt/text.h>
#include <menu/fmt/titleWrap.h>
#include <menu/comp/endis.h>

const char* mainMenu_title="Main menu";
const char* subMenu_title="Sub-menu";
const char* op1_text="Option 1";
const char* op2_text="Option 2";
const char* opn_text="Option ...";

StaticMenu<
  StaticText<&mainMenu_title>,
  StaticData<
    Item<StaticText<&op1_text>>,
    Item<StaticText<&op2_text>>,
    Item<EnDis<StaticText<&opn_text>>>,
    StaticMenu<
      StaticText<&subMenu_title>,
      StaticData<
        Item<StaticText<&op1_text>>,
        Item<EnDis<StaticText<&op2_text>>>,
        Item<StaticText<&opn_text>>
      >
    >
  >
> mainMenu;

void test_function_menu_size() {
  TEST_ASSERT_EQUAL(4,mainMenu.size());
}

void test_function_menu_size_ref() {
  Idx path[1]{3};
  Ref ref{sizeof(path)/sizeof(Idx),path};
  TEST_ASSERT_EQUAL(3,mainMenu.size(ref));
}

void test_function_menu_endis_ref() {
  Idx path[1]{2};
  Ref ref{sizeof(path)/sizeof(Idx),path};
  bool before=mainMenu.enabled(ref);
  mainMenu.enable(false,ref);
  bool after=mainMenu.enabled(ref);
  TEST_ASSERT_EQUAL(false,before&&!after);
}

FullPrinter<TitleWrapFmt<TextFmt<StringOut<>>>> out;
Nav<decltype(mainMenu),mainMenu,1> nav;

void test_function_menu_print() {
  out.newView();
  mainMenu.printMenu(nav,out);
  TEST_ASSERT_EQUAL(0,out.str().compare(0,strlen(mainMenu_title),mainMenu_title));
}

void test_function_menu_print_ref() {
  out.newView();
  Idx path[1]{3};
  Ref ref{sizeof(path)/sizeof(Idx),path};
  mainMenu.printMenu(nav,out,ref);
  // cout<<out.str();
  TEST_ASSERT_EQUAL(0,out.str().compare(0,strlen(subMenu_title),subMenu_title));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_function_menu_size);
    RUN_TEST(test_function_menu_size_ref);
    RUN_TEST(test_function_menu_endis_ref);
    RUN_TEST(test_function_menu_print_ref);
    UNITY_END();
    return 0;
}
