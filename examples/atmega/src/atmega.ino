#include <Arduino.h>
#include <menu.h>
#include <menu/IO/pcArrows.h>
#include <menu/IO/AM4Keys.h>
#include <menu/IO/Arduino/serialIO.h>
#include <menu/in.h>
#include <menu/nav.h>
#include <menu/framework/arduino.h>

using namespace Menu;

// template<typename O=Nil> 
// struct ArduinoFramework:O {
//   inline static uint32_t ms() {return millis();}
//   inline static uint32_t us() {return micros();}
// };

struct Config {
  template<typename O=Nil>
  struct Part:TinyPlatform<O> {
    using Idx=word;
  };
};

using Cfg=Config::Part<ArduinoFramework<>>;
CONFIG_MENU_LIB(Cfg)

using Out=OutDef<
  // OnFmt<Fmt::Title,PrintWrap<>::Part>::Part,
  // RangePanel<5>::Part,
  TitlePrinter::Part,
  BodyPrinter::Part,
  BasePrinter::Part,
  TextEditMode::Part,
  TextFmt::Part,//format translation
  // FullPrinter::Part,//format emit
  // XmlFmt::Part,
  // StdOut::Part//raw output device
  SerialOut<decltype(Serial),Serial>::Part
>; Out out;

bool menu_action() {
  Serial.println("menu action called");
  return false;
}

bool submenu_action() {
  Serial.println("sub-menu action called");
  return true;
}

bool op1_action() {
  Serial.println("op 1 action.");
  return false;
}

bool op2_action() {
  Serial.println("op 2 action.");
  return false;
}

bool sub1_action() {
  Serial.println("sub 1 action.");
  return true;
}

bool sub2_action() {
  Serial.println("sub 2 action.");
  return false;
}

bool imain_action() {
  Serial.println("iMainMenu action.");
  return true;
}

bool tog12_action();

bool running=true;
int perc=50;
bool quit() {return running=false;}

ConstText quit_text="<Quit!";
using Quit=ItemDef<Action<quit>::Part,StaticText<&quit_text>::Part>;
using IQuit=ItemDef<Prompt::Part,Action<quit>::Part,StaticText<&quit_text>::Part>;

ConstText exit_text="<Exit";
using Exit=ItemDef<StaticText<&exit_text>::Part>;
using IExit=ItemDef<Prompt::Part,StaticText<&exit_text>::Part>;

ConstText title_text="Main menu";
ConstText op2_text="op2";
ConstText tog12_text="Toggle En/Dis";
ConstText opn_text="...";
ConstText celsius="℃ ";
ConstText addr_label="Address";

ConstText op1_text="op1";
ConstText temp_text="Temp.";
ConstText subMenu_title_text="Sub-menu";
ConstText sub1_text="Sub op 1";
ConstText sub2_text="Sub op 2";

ConstText trig_text="Trigger";
ConstText trig0_text="None";
ConstText trig1_text="Rise";
ConstText trig2_text="Fall";
ConstText trig3_text="Both";
enum class Trig {None,Rise,Fall,Both};
// template<typename O>
// O& operator<<(O& out,Trig t) {
//   switch(t) {
//     case Trig::None:return out<<"None";
//     case Trig::Rise:return out<<"Rise";
//     case Trig::Fall:return out<<"Fall";
//     case Trig::Both:return out<<"Both";
//     default: return out;
//   }
// }
Trig trig=Trig::None;

char addr_buffer[]="000";//buffer to be edited
const char* addr_mask[]={"0123456789ABCDEF"};//edit mask for previous buffer
const char* ox="0x";//hex prefix

using TrigOptions=StaticMenuBody<
  ItemDef<StaticText<&trig0_text>::Part,Value<Trig,Trig::None>::Part>
  ,ItemDef<StaticText<&trig1_text>::Part,Value<Trig,Trig::Rise>::Part>
  ,ItemDef<StaticText<&trig2_text>::Part,Value<Trig,Trig::Fall>::Part>
  ,ItemDef<StaticText<&trig3_text>::Part,Value<Trig,Trig::Both>::Part>
>;

using MainMenu=StaticMenu<
  ItemDef<StaticText<&title_text>::Part>
  ,WrapNav::Part<//this menu will wrap
    StaticMenuBody<
      ItemDef<EnDis<true>::Part,Action<op1_action>::Part,StaticText<&op1_text>::Part>
      ,ItemDef<EnDis<false>::Part,Action<op2_action>::Part,StaticText<&op2_text>::Part>
      ,ItemDef<Action<tog12_action>::Part,StaticText<&tog12_text>::Part>
      ,ItemDef<
        StaticLabel<&addr_label>::Part,
        AsEditMode::Part,
        StaticText<&ox>::Part,//some special text between the cursor and the data
        AsValue::Part,
        TextEditBase<addr_buffer,sizeof(addr_buffer)/sizeof(addr_label[0]),addr_mask>::Part
      >
      ,ItemDef<
        StaticLabel<&trig_text>::Part
        ,AsValue::Part 
        ,Toggle<
          StaticMenu<
            ItemDef<StaticLabel<&trig_text>::Part>::Part
            ,WrapNav::Part<TrigOptions>
          >
        >::Part
        ,FieldRef<Trig,trig,Trig::None>::Part
      >
      ,ItemDef<
        StaticLabel<&trig_text>::Part
        ,AsEditMode::Part
        ,AsValue::Part
        ,Select<
          StaticMenu<
            ItemDef<StaticLabel<&trig_text>::Part>::Part
            ,WrapNav::Part<TrigOptions>
          >
        >::Part
        ,FieldRef<Trig,trig,Trig::None>::Part
      >
      ,ItemDef<
        StaticLabel<&trig_text>::Part
        ,AsEditMode::Part
        ,AsValue::Part 
        ,Choose<
          StaticMenu<
            ItemDef<StaticLabel<&trig_text>::Part>
            ,TrigOptions
          >
        >::Part
        ,FieldRef<Trig,trig,Trig::None>::Part
      >
      ,ItemDef<
        StaticLabel<&temp_text>::Part
        ,AsEditMode::Part
        ,AsValue::Part
        ,Chain<
          NumRef<decltype(perc),perc>::Part,
          StaticSTRange<decltype(perc),0,100,10,1>::Part
        >::Part
        ,AsUnit::Part
        ,StaticText<&celsius>::Part
      >
      ,Action<submenu_action>::Part<
        StaticMenu<
          ItemDef<StaticText<&subMenu_title_text>::Part>
          ,StaticMenuBody<
            ItemDef<Action<sub1_action>::Part,StaticText<&sub1_text>::Part>
            ,ItemDef<Action<sub2_action>::Part,StaticText<&sub2_text>::Part>
            ,ItemDef<StaticText<&opn_text>::Part>
            ,ItemDef<StaticText<&opn_text>::Part>
            ,Exit
          >
        >
      >
      ,ItemDef<StaticText<&opn_text>::Part>
      // ,Quit
    >
  >
>;

MainMenu mainMenu;

// togle enable/disable on first 2 options
bool tog12_action() {
  // _trace(mdo<<"toggling 1&2\n\r");
  bool en1=Path<0>().walk<MainMenu,Enabled>(mainMenu);
  bool en2=Path<1>().walk<MainMenu,Enabled>(mainMenu);
  Path<0>().walk<MainMenu,Enable,bool>(mainMenu,!en1);
  Path<1>().walk<MainMenu,Enable,bool>(mainMenu,!en2);
  return true;
}

//navigation API
using Nav=NavDef<StaticNav<MainMenu,4>::Part>;
Nav nav(mainMenu);

// NavDef<StaticNav<IMainMenu,3>::Part> iNav(iMainMenu);
//menu input --------------------------------------
InDef<
  ArduinoSerialIn<>::Part,
  AM4Parser::Part,//use AM4 navigation keys `+-*/`
  PCArrows::Part//using PC keys
> in;

void setup() {
  Serial.begin(115200);
  while(!Serial);
}

void loop() {
  if(nav.doInput(in)) nav.printTo(out);
  // in.relax();//use poll instead...
  // nav.relax();//TODO: only when idling (reset otherwise)
}