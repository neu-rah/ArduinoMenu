#include <menu.h>
#include <menu/sys/stdMenu.h>
#include <menu/IO/linuxKeyIn.h>
#include <menu/IO/pcArrows.h>
#include <menu/in.h>
#include <menu/nav.h>
#include "lang.h"
#include <array>
#include <initializer_list>
using namespace Menu;
using namespace std;

#ifdef ARDUINO
  template<decltype(Serial)& dev=Serial>
  struct SerialOut {
    template<typename O>
    void print(O o) {dev.print(o);}
    void nl() {dev.println();}
  };
#else
  #include <iostream>
  using namespace std;
  #define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#endif

#include <menu/IO/stdOut.h>

template<typename O=Nil> 
struct Native:O {
  using Idx=size_t;
};
template<typename Cfg>
struct Config {
  template<typename O=Nil>
  struct Part:Cfg,O {
  };
};

using Cfg=Config<Native<>>;
CONFIG_MENU_LIB(Cfg)

using Out=OutDef<
  OnFmt<Fmt::Title,PrintWrap<>::Part>::Part,
  RangePanel<5>::Part,
  TitlePrinter::Part,
  BodyPrinter::Part,
  BasePrinter::Part,
  TextEditMode::Part,
  TextFmt::Part,//format translation
  // FullPrinter::Part,//format emit
  // XmlFmt::Part,
  StdOut::Part//raw output device
>; Out out;

using IMenuOut=OutDef<
  MenuOut::Part,
  FullPrinter::Part,//format emit
  TextFmt::Part,//format translation
  // XmlFmt::Part,
  StdOut::Part//raw output device
>;
IMenuOut iOut;

//-------------------------

// Langs selectedLang=Langs::en;
// template<TextId id>
// struct Lang {
//   template<typename O=Empty<Nil>>
//   struct Part:O {
//     using Base=O;
//     using This=Part<Base>;
//     using Base::Base;
//     template<typename Out>
//     void printTo(Nav& nav,Out o) {
//       o.print(langs[selectedLang][id]);
//       Base::printTo(o);
//     }
//   };
// };

//dynamic text
Text::Part<> op1("op1");

// Lang<txtOk>::Part<> ok;

bool menu_action() {
  cout<<"menu action called\r"<<endl;
  return false;
}

bool submenu_action() {
  cout<<"sub-menu action called\r"<<endl;
  return true;
}

bool op1_action() {
  cout<<"op 1 action.\r"<<endl;
  return false;
}

bool op2_action() {
  cout<<"op 2 action.\r"<<endl;
  return false;
}

bool sub1_action() {
  cout<<"sub 1 action.\r"<<endl;
  return true;
}

bool sub2_action() {
  cout<<"sub 2 action.\r"<<endl;
  return false;
}

bool imain_action() {
  cout<<"iMainMenu action.\r"<<endl;
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
template<typename O>
O& operator<<(O& out,Trig t) {
  switch(t) {
    case Trig::None:return out<<"None";
    case Trig::Rise:return out<<"Rise";
    case Trig::Fall:return out<<"Fall";
    case Trig::Both:return out<<"Both";
    default: return out;
  }
}
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
        ,Toggle<
          StaticMenu<
            ItemDef<StaticLabel<&trig_text>::Part>::Part
            ,WrapNav::Part<TrigOptions>
          >
        >::Part
        ,AsValue::Part 
        ,FieldRef<Trig,trig,Trig::None>::Part
      >
      ,ItemDef<
        StaticLabel<&trig_text>::Part
        ,Select<
          StaticMenu<
            ItemDef<StaticLabel<&trig_text>::Part>::Part
            ,WrapNav::Part<TrigOptions>
          >
        >::Part
        ,AsEditMode::Part
        ,AsValue::Part
        ,FieldRef<Trig,trig,Trig::None>::Part
      >
      ,ItemDef<
        StaticLabel<&trig_text>::Part
        ,Choose<
          StaticMenu<
            ItemDef<StaticLabel<&trig_text>::Part>
            ,TrigOptions
          >
        >::Part
        ,AsEditMode::Part
        ,AsValue::Part 
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
      ,Quit
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


// ConstText std_sub="std sub menu";
// using StdSub=ItemDef<Prompt::Part,StdMenu<array<IItem*,3>>::Part,StaticText<&std_sub>::Part>;
// StdSub stdSub{
//   new ItemDef<Prompt::Part,Action<sub1_action>::Part,Text::Part>("sub 1"),
//   new ItemDef<Prompt::Part,Action<sub2_action>::Part,Text::Part>("sub 2"),
//   new IExit()
// };

// ConstText std_title="array menu";
// using IMainMenu=ItemDef<
//   Prompt::Part,
//   StdMenu<array<IItem*,4>>::Part,
//   Action<imain_action>::Part,
//   StaticText<&std_title>::Part
// >;

// IMainMenu iMainMenu{
//   new ItemDef<Prompt::Part,Action<op1_action>::Part,Text::Part>("Yupi!"),
//   new ItemDef<Prompt::Part,Action<op2_action>::Part,Text::Part>(".zZZz."),
//   &stdSub,
//   new IQuit()
// };

// ItemDef<Prompt::Part,Text::Part> test("ok");

//navigation API
using Nav=NavDef<StaticNav<MainMenu,4>::Part>;
Nav nav(mainMenu);

// NavDef<StaticNav<IMainMenu,3>::Part> iNav(iMainMenu);
//menu input --------------------------------------
InDef<
  LinuxKeyIn::Part,
  PCArrows::Part
> in;

// using EnFld=ItemDef<
//   Choose<
//     StaticMenu<
//       ItemDef<AsLabel<StaticText<&trig_text>::Part>::Part>::Part
//       ,StaticMenuBody<
//         ItemDef<StaticText<&trig0_text>::Part,Value<Trig,Trig::None>::Part>
//         ,ItemDef<StaticText<&trig1_text>::Part,Value<Trig,Trig::Rise>::Part>
//         ,ItemDef<StaticText<&trig2_text>::Part,Value<Trig,Trig::Fall>::Part>
//         ,ItemDef<StaticText<&trig3_text>::Part,Value<Trig,Trig::Both>::Part>
//       >
//     >
//   >::Part,
//   // FieldRef<Trig,trig,Trig::None>::Part
//   AsValue< 
//     FieldRef<Trig,trig,Trig::None>::Part
//   >::Part
// >;

// EnFld enFld;

// void inspect(Idx l,Idx* path) {
//   PathRef ref(l,path);
//   mdo<<"inpecting path "<<ref;
//   mdo<<" Styles:"<<ref.template walk<MainMenu,Styles>(mainMenu)<<" ";
//   ref.template walk<MainMenu,PrintTo,Nav&,Out&,bool,bool>(mainMenu,nav,out,false,false);
//   mdo<<"\n\r";
// }

// using Test=ItemDef<
//   // AsLabel<StaticText<&addr_label>::Part>::Part,
//   TextEdit<addr_buffer,sizeof(addr_buffer),addr_mask>::Part
// >;

int main() {
  // Idx p[]{};
  // Idx p0[]{0};
  // Idx p1[]{1};
  // Idx p01[]{0,1};
  // inspect(0,p);
  // inspect(1,p0);
  // inspect(1,p1);
  // inspect(2,p01);

  // PathRef(nav.level,nav.path).template walk<MainMenu,PrintMenuTo,Nav&,Out&,bool,bool,int>(mainMenu,nav,out,nav.focus(),nav.tune(),0);
  // PrintMenuTo().template operator()<MainMenu,Nav&,Out&>(mainMenu,nav,out,nav.focus(),nav.tune(),0);

  // Test().template printTo<decltype(nav),decltype(out),true>(nav,out,false,false);
  nav.printTo(out);
  while(running) if(nav.doInput(in)) {nav.printTo(out);out.nl();}

  // running=true;
  // iNav.printTo(iOut);
  // while(running) if(iNav.doInput(in)) iNav.printTo(iOut);

  // enFld.printTo(out);
  // enFld.cmd<Cmd::Enter>();
  return 0;
}
