#include <Wire.h>

//main include for ArduinoMenu
#include <menu.h>
//input/output drivers --------------------------------------
#include <menu/IO/Arduino/serialIn.h>
#include <menu/IO/Arduino/serialOut.h>
#include <menu/IO/Arduino/u8x8Out.h>
//format specifyers -----------------------------------------
// #include <menu/fmt/text.h>
#include <menu/fmt/titleWrap.h>
#include <menu/fmt/textEditMode.h>
#include <menu/fmt/textCursor.h>
#include <menu/fmt/textItem.h>
//components ------------------------------------------------
#include <menu/comp/endis.h>
#include <menu/comp/numField.h>

//wemos lolin 32 + oled + u8x8
#define SDA 5
#define SCL 4
// #define fontName u8x8_font_5x7_f
// #define fontName u8x8_font_artossans8_r
#define fontName u8x8_font_pcsenior_f
// #define fontName u8x8_font_amstrad_cpc_extended_f

U8X8_SSD1306_128X64_VCOMH0_HW_I2C u8x8(U8X8_PIN_NONE, SCL, SDA);//allow contrast change

// some user code for example --------------------------------------------------------------
bool running=true;
int year=1967;
int vcc=3;

//------------------------------
//menu action handlers
bool tog12();

//handler for "Quit" option
bool quit() {
  //just signal program exit
  trace(MDO<<"Quit!"<<endl);
  running=false;
  return true;
}

// define menu structure ---------------------------------------------------------------

//a single rogue test, not part of a menu
Text<> test("Just a test");

//texts for menu
const char* mainMenu_title="Main";
const char* subMenu_title="Sub-menu";
const char* op1_text="Option 1";
const char* op2_text="Option 2";
const char* tog12_text="toggle 1&2";
const char* opn_text="Option ...";
const char* exit_txt="<Exit";
const char* yr_txt="Year";
const char* vcc_txt="VCC";
const char* volts_txt="V";

//static menu structure
StaticMenu<
  Item<StaticText<&mainMenu_title>>,
  StaticData<
    Item<EnDis<StaticText<&op1_text>>>,
    Item<EnDis<StaticText<&op2_text>>>,
    Item<Action<StaticText<&tog12_text>,tog12>>,
    Item<NumField<StaticText<&yr_txt>,int,year,1900,2100,10,1>>,//this is NOT good, changing limits generates new code->TODO: add a translation
    Item<NumField<StaticText<&vcc_txt>,decltype(vcc),vcc,0,100,1,0,StaticText<&volts_txt>>>,
    Item<StaticText<&opn_text>>,
    StaticMenu<
      Item<StaticText<&subMenu_title>>,
      StaticData<
        Item<EnDis<StaticText<&op1_text>>>,
        Item<StaticText<&op2_text>>,
        Item<StaticText<&opn_text>>,
        Item<StaticText<&opn_text>>,
        Item<StaticText<&opn_text>>,
        Item<Exit<StaticText<&exit_txt>>>
      >
    >
  >
> mainMenu;//create menu object

//menu input --------------------------------------
SerialIn<decltype(Serial),Serial> in;//create input object (here serial)

//menu output (u8x8)
using Out=FullPrinter<//print title and items
  Fmt<//formating API
    BigTitleU8x8Out<//print title using u8x8::draw2x2String()
      U8x8Fmt<//specific u8x8 formatings (clearing and inverting text)
        TextCursorFmt<//draw text cursor
          TextEditModeFmt<//draw edit mode text cursor
            TextItemFmt<//add newline after each item
              RangePanel<//scroll content on output geometry
                StaticPanel<0,0,128/8,64/8-1,U8x8Out<decltype(u8x8),u8x8>>//describe output geometry and device
              >
            >
          >
        >
      >
    >
  >
>;

Out out;//create output object (u8x8)

//navigation root ---------------------------------
Nav<decltype(mainMenu),mainMenu,2> nav;

//menu action handlers implementation -------------------------
bool tog12() {
  //TODO: add a better reference constructor
  mainMenu.enable(!mainMenu.enabled(0),0);
  mainMenu.enable(!mainMenu.enabled(1),1);
  return true;
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Wire.begin(SDA,SCL);
  u8x8.setFont(fontName);
  u8x8.begin();
  u8x8.setContrast(255);
  u8x8.drawString(0,0,"www.r-site.net");
  delay(500);
  u8x8.clear();
  // test.print(out);//printing single field
  // Console::nl();
  mainMenu.enable(false,1);//disable second option
  nav.printMenu(out);
}

void loop() {
  if (nav.doInput(in)) nav.printMenu(out);
  u8x8.drawString(0,64/8-1,"www.r-site.net");
}
