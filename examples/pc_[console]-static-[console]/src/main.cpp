#include <iostream>
using namespace std;

#include <staticMenu.h>
#include <menu/fmt/fullText.h>
#include <menu/fmt/titleWrap.h>
#include <menu/IO/linuxKeyIn.h>
#include <menu/IO/consoleOut.h>
#include <menu/comp/endis.h>//enable/disable
using namespace Menu;

//sketch control and actions ------------------------
bool running=true;//exit program when false
int max_temp=80;//target for numeric field edit
bool myLed=false;//target for toggle edit
enum class TrigType {None,Rise,Fall,Both};
TrigType trigger=TrigType::None;
int choose_value=0;

bool quit() {
  //just signal program exit
  _trace(MDO<<"Quit!"<<endl);
  running=false;
  return true;
}
bool tog12();//implemented later because we need to access mainMenu

bool op1_action() {
  cout<<"Option 1 action called!"<<endl;
  return true;//false would close the menu
}

bool op2_action() {
  cout<<"Option 2 action called!"<<endl;
  return true;
}

bool sub_action() {
  cout<<"Sub-menu action called!"<<endl;
  return true;
}

bool tog_action() {
  cout<<"Toggling field myLed is "<<(myLed?"On":"Off")<<endl;
  return false;
}

bool sel_action() {
  cout<<"Selecting ";
  switch(trigger){
    case TrigType::None: cout<<"none";break;
    case TrigType::Rise: cout<<"rise";break;
    case TrigType::Fall: cout<<"fall";break;
    case TrigType::Both: cout<<"both";break;
  }
  cout<<endl;
  return true;
}

bool pos_action() {
  cout<<"Choosing ";
  switch(choose_value){
    case 0: cout<<"first";break;
    case 1: cout<<"second";break;
    case 2: cout<<"third";break;
    case 3: cout<<"last";break;
  }
  cout<<endl;
  return true;
}

//menu texts -------------------------
const char* subText="Sub-menu";
const char* sub1_text="Sub 1";
const char* sub2_text="Sub 2";
const char* tog12_text="Toggle 1&2";
const char* subn_text="Sub...";
const char* exit_text="<Exit";

const char* mainText="Main menu";
const char* max_temp_label="Max.";
const char* max_temp_unit="ºC";
const char* op1_text="Option 1";
const char* op2_text="Option 2";
const char* opn_text="Option...";
const char* quit_text="<Quit.";

const char* led_text="Led:";
const char* on_text="On";
const char* off_text="Off";
using On=Item<EnumValue<bool,true>::Part,StaticText<&on_text>::Part>;
using Off=Item<EnumValue<bool,false>::Part,StaticText<&off_text>::Part>;

const char* sel_text="Trigger";
const char* trig_none_text="None";
const char* trig_rise_text="Rise";
const char* trig_fall_text="Fall";
const char* trig_both_text="Both";
using TrigNone=Item<EnumValue<TrigType,TrigType::None>::Part,StaticText<&trig_none_text>::Part>;
using TrigRise=Item<EnumValue<TrigType,TrigType::Rise>::Part,StaticText<&trig_rise_text>::Part>;
using TrigFall=Item<EnumValue<TrigType,TrigType::Fall>::Part,StaticText<&trig_fall_text>::Part>;
using TrigBoth=Item<EnumValue<TrigType,TrigType::Both>::Part,StaticText<&trig_both_text>::Part>;

const char* choose_text="Choose";
const char* pos_first_text="First";
const char* pos_second_text="Second";
const char* pos_third_text="Third";
const char* pos_last_text="Last";
using ChooseFirst=Item<EnumValue<int,0>::Part,StaticText<&pos_first_text>::Part>;
using ChooseSecond=Item<EnumValue<int,1>::Part,StaticText<&pos_second_text>::Part>;
using ChooseThird=Item<EnumValue<int,2>::Part,StaticText<&pos_third_text>::Part>;
using ChooseLast=Item<EnumValue<int,-1>::Part,StaticText<&pos_last_text>::Part>;

//menu static structure ---------------------------
using MainMenu=Item<
  // SetWalker::Part,
  StaticMenu<
    Item<StaticText<&mainText>::Part,Mutable::Part>,
    StaticData<
      Item<Action<op1_action>::Part,EnDis<>::Part,StaticText<&op1_text>::Part,Mutable::Part>,
      Item<Action<op2_action>::Part,EnDis<false>::Part,StaticText<&op2_text>::Part,Mutable::Part>,
      Item<Action<tog12>::Part,StaticText<&tog12_text>::Part,Mutable::Part>,
      Item<
        StaticNumField<
          StaticText<&max_temp_label>::Part,//title
          int,max_temp,0,100,10,1,true,//parameters
          StaticText<&max_temp_unit>::Part//unit (optional)
        >::template Part
      >,
      Item<StaticText<&opn_text>::Part,Mutable::Part>,
      Item<
        Handler<tog_action>::Part,//the handler
        ToggleField<
          bool,myLed,
          Item<StaticText<&led_text>::Part>,
          StaticData<On,Off>//the enumeration of options (text and values possibly)
        >::Part
      >,
      Item<
        Handler<sel_action>::Part,//the handler
        ActOnUpdate::Part,
        SelectField<
          TrigType,trigger,
          Item<StaticText<&sel_text>::Part>,
          StaticData<TrigNone,TrigRise,TrigFall,TrigBoth>//the enumeration of options (text and values possibly)
        >::Part
      >,
      Item<
        Handler<pos_action>::Part,//the handler
        ActOnUpdate::Part,
        // WrapNav::Part,
        ChooseField<
          int,choose_value,
          Item<StaticText<&choose_text>::Part>,
          StaticData<ChooseFirst,ChooseSecond,ChooseThird,ChooseLast>//the enumeration of options (text and values possibly)
        >::Part
      >,
      Item<
        // WrapNav::Part,
        StaticMenu<
          Item<StaticText<&subText>::Part,Mutable::Part>,
          StaticData<
            Item<Action<sub_action>::Part,StaticText<&sub1_text>::Part,Mutable::Part>,
            Item<Action<sub_action>::Part,StaticText<&sub2_text>::Part,Mutable::Part>,
            Item<StaticText<&exit_text>::Part,Mutable::Part>
          >
        >::Part
      >,
      Item<Action<quit>::Part,StaticText<&quit_text>::Part,Mutable::Part>
    >
  >::Part
>;

MainMenu mainMenu;

//menu output ---------------------------------------
StaticMenuOut<
  FullPrinter::Part,//print all parts, title, index, text cursor
  TitleWrapFmt<>::Part,//wrap title in []
  TextFmt::Part,//format the text parts, use `>` as text cursor`
// #ifdef MENU_DEBUG
//   PartialDraw::Part,//just for testing, because console is not of this cathegory
//   PanelTarget::Part,//detect target (menu) changes
//   RangePanel<>::Part,//control vertical scrolling
//   StaticPanel<0,0,20,4>::Part,//define output geometry
// #endif
  Console::Part,//the raw output device to use
  TextMeasure<>::Part//default monometric text measure
> out;

StaticNavRoot<Nav<MainMenu,3>::Part> nav(mainMenu);

//menu input --------------------------------------
StaticMenuIn<
  LinuxKeyIn::Part,
  PCArrows::Part
> in;

bool tog12() {
  _trace(MDO<<"Toggle Enable/Disable of options 1 and 2"<<endl);
  mainMenu.enable(!mainMenu.enabled(Path<0>().ref()),Path<0>().ref());
  mainMenu.enable(!mainMenu.enabled(Path<1>().ref()),Path<1>().ref());
  return true;
}

int main() {
  // nav.path[0]=5;
  // nav.enter();
  nav.print(out);
  while(running) if (nav.doInput(in)) nav.print(out);
  return 0;
}
