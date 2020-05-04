/********************
Arduino generic menu system

Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

output: Serial
input: Serial

user defined array menu (userMenu plugin)
*/

#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>
#include <plugin/userMenu.h>

using namespace Menu;

#define MAX_DEPTH 4

enum SelTest {Zero=0,One,Two};
enum ChooseTest {First=1,Second=2,Third=3,Last=-1};
constexpr int dataSz=3;
constexpr int nameSz=20;

struct Data {
  char name[nameSz+1]="<name>              ";
  bool valid=0;
  SelTest selTest=Zero;
  ChooseTest chooseTest=First;
  //how to copy this data, being a simple data c++ will generate this for you
  // Data& operator=(Data& o){
  //   strcpy(name,o.name);
  //   valid=o.valid;
  //   selTest=o.selTest;
  //   chooseTest=o.chooseTest;
  //   return o;
  // }
};

Data myTargets[dataSz];//our data
//a temp. to be edited
//the data type must have assignment operator
//because we will later on move data back and forth between the actual data and this template
//menu edit is wired to this temp. and menu eventd will deal with the copy-call
Data target;


char* constMEM alphaNum MEMMODE=" 0123456789.ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz,\\|!\"#$%&/()=?~*^+-{}[]€";
char* constMEM alphaNumMask[] MEMMODE={alphaNum};

TOGGLE(target.valid,editValid,"Valid: ",doNothing,noEvent,noStyle//,doExit,enterEvent,noStyle
  ,VALUE("On",HIGH,doNothing,noEvent)
  ,VALUE("Off",LOW,doNothing,noEvent)
);

SELECT(target.selTest,selMenu,"Select",doNothing,noEvent,noStyle
  ,VALUE("Zero",Zero,doNothing,noEvent)
  ,VALUE("One",One,doNothing,noEvent)
  ,VALUE("Two",Two,doNothing,noEvent)
);

CHOOSE(target.chooseTest,chooseMenu,"Choose",doNothing,noEvent,noStyle
  ,VALUE("First",First,doNothing,noEvent)
  ,VALUE("Second",Second,doNothing,noEvent)
  ,VALUE("Third",Third,doNothing,noEvent)
  ,VALUE("Last",Last,doNothing,noEvent)
);

result saveTarget(eventMask e,navNode& nav) {
  _trace(MENU_DEBUG_OUT<<"saveTarget"<<endl);
  idx_t n=nav.root->path[nav.root->level-1].sel;//get selection of previous level
  myTargets[n]=target;
  return quit;
}

//if you want to print the recird name as the title,
//then you MUST create a customized print menu to replace this default one
MENU(targetEdit,"Target edit",doNothing,noEvent,wrapStyle
  ,EDIT("Name",target.name,alphaNumMask,doNothing,noEvent,noStyle)
  ,SUBMENU(editValid)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  ,OP("Save",saveTarget,enterEvent)
  ,EXIT("<Back")
);

//handling the user menu selection
//
result targetEvent(eventMask e,navNode& nav) {
  _trace(MENU_DEBUG_OUT<<"copy data to temp.\n");
  target=myTargets[nav.sel];
  //you can store nav.sel for future reference
  return proceed;
}

struct TargetMenu:UserMenu{
  using UserMenu::UserMenu;

  // override sz() function to have variable/custom size
  // inline idx_t sz() const override {return 0;}

  //customizing the print of user menu
  Used printItem(menuOut& out, int idx,int len) override {
    return len?out.printText(myTargets[idx].name,len):0;
  }
};

//build the user menu object, optinally giving a sub menu
#ifdef MENU_USERAM
  //for non-AVR devices or when MENU_USERAM is defined at compiler level
  TargetMenu targetsMenu("Targets",dataSz,targetEdit,targetEvent,enterEvent);
#else
  //menu allocation compatible with AVR flash ---------------------------------
  constMEM char targetsMenuTitle[] MEMMODE="Targets";
  constMEM menuNodeShadowRaw targetsMenuInfoRaw MEMMODE={
    (callback)targetEvent,
    (systemStyles)(_menuData|_canNav),
    targetsMenuTitle,
    enterEvent,
    wrapStyle,
    dataSz,
    NULL
  };
  constMEM menuNodeShadow& targetsMenuInfo=*(menuNodeShadow*)&targetsMenuInfoRaw;
  TargetMenu targetsMenu(targetsMenuInfo,targetEdit);
#endif

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OBJ(targetsMenu)//use in a macro built menu
  ,EXIT("<Back")
);

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

serialIn serial(Serial);
NAVROOT(nav,mainMenu,MAX_DEPTH,serial,out);

result idle(menuOut &o, idleEvent e) {
  switch(e) {
    case idleStart:o.println("suspending menu!");break;
    case idling:o.println("suspended...");break;
    case idleEnd:o.println("resuming menu.");
      nav.reset();
      break;
  }
  return proceed;
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("menu 4.x test =================");Serial.flush();
  nav.timeOut=3000;
  nav.idleTask=idle;//point a function to be used when menu is suspended
}

void loop() {
  nav.poll();
  delay(100);//simulate a delay when other tasks are done
}
