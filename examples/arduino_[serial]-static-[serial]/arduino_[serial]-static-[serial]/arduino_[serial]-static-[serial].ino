/***
example for arduino (generic)
using serial for menu IO
tested on nano
*/

//main include for ArduinoMenu
#include <staticMenu.h>
//input/output drivers --------------------------------------
// #include <menu/IO/Arduino/serialIn.h>
// #include <menu/IO/Arduino/serialOut.h>
#include <menu/IO/Arduino/serialIO.h>//include both serial in and out
//format specifyers -----------------------------------------
#include <menu/fmt/fullText.h>
#include <menu/fmt/titleWrap.h>
//components ------------------------------------------------
#include <menu/comp/endis.h>//enable/disable
// #include <menu/comp/numField.h>//numeric fields with name, value and unit
#include <menu/comp/Arduino/flashText.h>
using namespace Menu;

// some user code for example --------------------------------------------------------------

//some target variables
bool myLed=false;//target for toggle edit
int max_temp=80;
enum class TrigType {None,Rise,Fall,Both};
TrigType trigger=TrigType::None;
int choose_value=0;

//------------------------------
//menu action handlers
bool tog12();

bool op1_action() {
  Serial.println("Option 1 action called!");
  return true;//false would close the menu
}

bool op2_action() {
  Serial.println("Option 2 action called!");
  return true;
}

bool sub_action() {
  Serial.println("Sub-menu action called!");
  return true;
}

bool tog_action() {
  Serial.print("Toggling field myLed is ");
  Serial.println(myLed?"On":"Off");
  return true;
}

bool sel_action() {
  Serial.print("Selecting ");
  switch(trigger){
    case TrigType::None: Serial.println("none");break;
    case TrigType::Rise: Serial.println("rise");break;
    case TrigType::Fall: Serial.println("fall");break;
    case TrigType::Both: Serial.println("both");break;
  }
  return true;
}

bool pos_action() {
  Serial.print("Choosing ");
  switch(choose_value){
    case 0: Serial.println("first");break;
    case 1: Serial.println("second");break;
    case 2: Serial.println("third");break;
    case 3: Serial.println("last");break;
  }
  return true;
}

enum MyIds:Idx {
  id_mainMenu,
  id1,
  id2,
  id3
};

// define menu structure ---------------------------------------------------------------

//texts for menu
extern const char mainMenu_title[] PROGMEM="Main menu";
extern const char subMenu_title[] PROGMEM="Sub-menu";
extern const char op1_text[] PROGMEM="Option 1";
extern const char op2_text[] PROGMEM="Option 2";
extern const char tog12_text[] PROGMEM="toggle 1&2";
extern const char opn_text[] PROGMEM="Option ...";
extern const char exit_txt[] PROGMEM="<Exit";
extern const char max_temp_label[] PROGMEM="Max.";
extern const char max_temp_unit[] PROGMEM="ºC";

extern const char led_text[] PROGMEM="Led:";
extern const char on_text[] PROGMEM="On";
extern const char off_text[] PROGMEM="Off";

using On=Item<EnumValue<bool,true>::Part,FlashText<decltype(on_text),&on_text>::Part>;
using Off=Item<EnumValue<bool,false>::Part,FlashText<decltype(off_text),&off_text>::Part>;

extern const char sel_text[] PROGMEM="Trigger";
extern const char trig_none_text[] PROGMEM="None";
extern const char trig_rise_text[] PROGMEM="Rise";
extern const char trig_fall_text[] PROGMEM="Fall";
extern const char trig_both_text[] PROGMEM="Both";
using TrigNone=Item<EnumValue<TrigType,TrigType::None>::Part,FlashText<decltype(trig_none_text),&trig_none_text>::Part>;
using TrigRise=Item<EnumValue<TrigType,TrigType::Rise>::Part,FlashText<decltype(trig_rise_text),&trig_rise_text>::Part>;
using TrigFall=Item<EnumValue<TrigType,TrigType::Fall>::Part,FlashText<decltype(trig_fall_text),&trig_fall_text>::Part>;
using TrigBoth=Item<EnumValue<TrigType,TrigType::Both>::Part,FlashText<decltype(trig_both_text),&trig_both_text>::Part>;

extern const char choose_text[] PROGMEM="Choose";
extern const char pos_first_text[] PROGMEM="First";
extern const char pos_second_text[] PROGMEM="Second";
extern const char pos_third_text[] PROGMEM="Third";
extern const char pos_last_text[] PROGMEM="Last";
using ChooseFirst=Item<EnumValue<int,0>::Part,FlashText<decltype(pos_first_text),&pos_first_text>::Part>;
using ChooseSecond=Item<EnumValue<int,1>::Part,FlashText<decltype(pos_second_text),&pos_second_text>::Part>;
using ChooseThird=Item<EnumValue<int,2>::Part,FlashText<decltype(pos_third_text),&pos_third_text>::Part>;
using ChooseLast=Item<EnumValue<int,-1>::Part,FlashText<decltype(pos_last_text),&pos_last_text>::Part>;

//static menu structure
Item<
  IdTag<id_mainMenu>::Part,
  SetWalker::Part,
  StaticMenu<
    Item<FlashText<decltype(mainMenu_title),&mainMenu_title>::Part>,
    StaticData<
      Item<IdTag<id1>::Part,Action<op1_action>::Part,EnDis<>::Part,FlashText<decltype(op1_text),&op1_text>::Part>,
      Item<IdTag<id2>::Part,Action<op2_action>::Part,EnDis<false>::Part,FlashText<decltype(op2_text),&op2_text>::Part>,
      Item<IdTag<id3>::Part,Action<tog12>::Part,FlashText<decltype(tog12_text),&tog12_text>::Part>,
      Item<
        StaticNumField<
          FlashText<decltype(max_temp_label),&max_temp_label>::Part,//title
          int,max_temp,0,100,10,1,true,//parameters
          FlashText<decltype(max_temp_unit),&max_temp_unit>::Part//unit (optional)
        >::template Part
      >,
      Item<
        Handler<tog_action>::Part,//the handler
        ToggleField<
          bool,myLed,
          Item<FlashText<decltype(led_text),&led_text>::Part>,
          StaticData<On,Off>//the enumeration of options (text and values possibly)
        >::Part
      >,
      Item<
        Handler<sel_action>::Part,//the handler
        ActOnUpdate::Part,
        SelectField<
          TrigType,trigger,
          Item<FlashText<decltype(sel_text),&sel_text>::Part>,
          StaticData<TrigNone,TrigRise,TrigFall,TrigBoth>//the enumeration of options (text and values possibly)
        >::Part
      >,
      Item<
        Handler<pos_action>::Part,//the handler
        ActOnUpdate::Part,
        // WrapNav::Part,
        ChooseField<
          int,choose_value,
          Item<FlashText<decltype(choose_text),&choose_text>::Part>,
          StaticData<ChooseFirst,ChooseSecond,ChooseThird,ChooseLast>//the enumeration of options (text and values possibly)
        >::Part
      >,
      Item<
        StaticMenu<
          Item<FlashText<decltype(subMenu_title),&subMenu_title>::Part>,
          StaticData<
            Item<Action<sub_action>::Part,EnDis<>::Part,FlashText<decltype(op1_text),&op1_text>::Part>,
            Item<Action<sub_action>::Part,FlashText<decltype(op2_text),&op2_text>::Part>,
            Item<Action<sub_action>::Part,FlashText<decltype(opn_text),&opn_text>::Part>,
            Item<FlashText<decltype(exit_txt),&exit_txt>::Part>
          >
        >::Part
      >,
      Item<FlashText<decltype(opn_text),&opn_text>::Part>
    >
  >::Part
> mainMenu;//create menu object

//menu input --------------------------------------
StaticMenuIn<
  SerialIn<decltype(Serial),Serial>::Part,
  AMNavKeys<>::Part,
  Ids::Part
> in;//create input object (here serial)

//menu output (Serial)
using Out=StaticMenuOut<
  FullPrinter::Part,//print title and items
  TitleWrapFmt<>::Part,//put [] around menu title
  TextFmt::Part,//apply text formating
  SerialOut<decltype(Serial),Serial>::Part//raw output device
>;

Out out;//create output object (Serial)

//navigation root ---------------------------------
StaticNavRoot<Nav<decltype(mainMenu),3>::Part> nav(mainMenu);

//menu action handlers implementation
bool tog12() {
  Serial.println("Toggle enable/diable of options 1 and 2");
  mainMenu.enable(!mainMenu.enabled(Path<0>().ref()),Path<0>().ref());
  mainMenu.enable(!mainMenu.enabled(Path<1>().ref()),Path<1>().ref());
  return true;
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("ArduinoMenu 5");
  delay(500);
  nav.print(out);
}

void loop() {
  if (nav.doInput(in)) nav.print(out);
  delay(100);
}
