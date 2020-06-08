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

//===========================================================================

//some values for user data record
enum SelTest {Zero=0,One,Two};
enum ChooseTest {First=1,Second=2,Third=3,Last=-1};
constexpr int dataSz=3;
constexpr int nameSz=20;


//some user data record type to be edited by the menu
struct Data {
  char name[nameSz+1]="<name>              ";
  bool valid=0;
  SelTest selTest=Zero;
  ChooseTest chooseTest=First;
  //how to copy this data...
  Data& operator=(Data& o){
    strncpy(name,o.name,nameSz);
    valid=o.valid;
    selTest=o.selTest;
    chooseTest=o.chooseTest;
    return o;
  }
};

//THE DATA <-----------------------------------------
Data myTargets[dataSz];//our data

//a temporary to be edited
//the data type must have assignment operator
//because we will later on move data back and forth between the actual data and this temporary
//menu edit is wired to this temp. and menu events will deal with the copy-call (from/to)
Data target;

//characters allowed on name field
const char* constMEM alphaNum MEMMODE=" 0123456789.ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz,\\|!\"#$%&/()=?~*^+-{}[]€";
const char* constMEM alphaNumMask[] MEMMODE={alphaNum};

//some enumeration of values allowed on some fields
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

//a function to save the edited data record
result saveTarget(eventMask e,navNode& nav) {
  trace(MENU_DEBUG_OUT<<"saveTarget"<<endl);
  navNode& nn=nav.root->path[nav.root->level-1];
  idx_t n=nn.sel;//get selection of previous level
  myTargets[n]=target;
  return quit;
}

//if you want to print the data record name as the title,
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
//this will copy the selected recotd into the temp var
result targetEvent(eventMask e,navNode& nav);

//the customized print of records
//menu system wil use this to print the list of all records
struct TargetMenu:UserMenu{
  using UserMenu::UserMenu;

  // override sz() function to have variable/custom size
  // if using exit option the an extra element has to be considered...
  // inline idx_t sz() const override {return 0;}

  //customizing the print of user menu item (len is the availabe space)
  Used printItem(menuOut& out, int idx,int len) override {
    //just printing the string name to the menu output device
    //as an item representative
    return len?out.printText(myTargets[idx].name,len):0;
  }
};

//build the user menu object, optionally giving a sub menu
#ifdef MENU_USERAM
  //for non-AVR devices or when MENU_USERAM is defined at compiler level
  TargetMenu targetsMenu("Targets",dataSz,"<Back",targetEdit,targetEvent,enterEvent);
  // TargetMenu targetsMenu("Targets",dataSz,targetEdit,targetEvent,enterEvent);//no exit option
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
  TargetMenu targetsMenu(targetsMenuInfo,targetEdit,"<Back");
  // TargetMenu targetsMenu(targetsMenuInfo,targetEdit);//no exit option
#endif

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OBJ(targetsMenu)//attach the array edit menu on a macri build nenu
  ,EXIT("<Back")
);

result targetEvent(eventMask e,navNode& nav) {
  trace(MENU_DEBUG_OUT<<"copy data to temp target:"<<(int)nav.target<<"\n");
  if(nav.target==&targetsMenu)//only if we are on targetsMenu
    target=myTargets[nav.sel];
  //you can store nav.sel for future reference
  return proceed;
}

//////////////////////////////////////////////////////////////////////////////
// menu IO and root navigation control

#define MAX_DEPTH 4

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

serialIn serial(Serial);
NAVROOT(nav,mainMenu,MAX_DEPTH,serial,out);

//function for menu idle/timemout
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
