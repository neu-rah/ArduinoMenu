/*
dynamic menu tests (experimental)

this is a somehow dynamic menu, its ok to use it on non AVR's
or with USE_RAM defined

the dynamic support is somehow limited
we do not support adding or removing options
however changing the list of options should be allowed

or replacing options

the user is responsible for all option allocation
deleting them if created with new, etc...

this can turn out to be a mess

objects construction is not realy tested beyond this example
please let me know if you have any interest on this kind of things

this is nice to support dynamic options like a list of detected wifi networks
or a list of files+folders from a folder/file system
*/

#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>
// #include <Streaming.h>

using namespace Menu;

#ifndef LED_BUILTIN
  #define LED_BUILTIN 13
#endif

#define LED LED_BUILTIN

#ifndef USING_RAM
#error "This menu example does not work on flash memory versions (MENU_USEPGM)"
#error "Library must be compiled with MENU_USERAM defined (default for non AVR's)"
#error "ex: passing -DMENU_USERAM to the compiler"
#endif

//choose field and options -------------------------------------
int duration=0;//target var
prompt* durData[]={
  new menuValue<int>("Off",0),
  new menuValue<int>("Short",1),
  new menuValue<int>("Medium",2),
  new menuValue<int>("Long",3)
};
choose<int>& durMenu =*new choose<int>("Duration",duration,sizeof(durData)/sizeof(prompt*),durData);

//select field and options -------------------------------------
enum Fxs {Fx0,Fx1,Fx2} selFx;//target var
prompt* fxData[]={
  new menuValue<Fxs>("Default",Fx0),
  new menuValue<Fxs>("Pop",Fx1),
  new menuValue<Fxs>("Rock",Fx2)
};
Menu::select<Fxs>& fxMenu =*new Menu::select<Fxs>("Fx",selFx,sizeof(fxData)/sizeof(prompt*),fxData);

//toggle field and options -------------------------------------
bool led=false;//target var
void setLed() {digitalWrite(LED,!led);}
prompt* togData[]={
  new menuValue<bool>("On",true),
  new menuValue<bool>("Off",false)
};
toggle<bool>& ledMenu
  =*new toggle<bool>("LED:",led,sizeof(togData)/sizeof(prompt*),togData,(Menu::callback)setLed,enterEvent);

//the submenu -------------------------------------
prompt* subData[]={
  new prompt("Sub1"),
  new prompt("Sub2"),
  new Exit("<Back")
};
menuNode& subMenu=*new menuNode("sub-menu",sizeof(subData)/sizeof(prompt*),subData);

//pad menu --------------------
uint16_t year=2017;
uint16_t month=10;
uint16_t day=7;

prompt* padData[]={
  new menuField<typeof(year)>(year,"","",1900,3000,20,1,doNothing,noEvent),
  new menuField<typeof(month)>(month,"/","",1,12,1,0,doNothing,noEvent),
  new menuField<typeof(day)>(day,"/","",1,31,1,0,doNothing,noEvent)
};
menuNode& padMenu=*new menuNode(
  "Date",
  sizeof(padData)/sizeof(prompt*),
  padData,
  doNothing,
  noEvent,
  noStyle,
  (systemStyles)(_asPad|Menu::_menuData|Menu::_canNav|_parentDraw)
);

//the main menu -------------------------------------
void op1Func() {Serial.println("Op 1 executed");}
uint8_t test=55;//target var for numerical range field

//edit text field info
const char* hexDigit MEMMODE="0123456789ABCDEF";//a text table
const char* hexNr[] MEMMODE={"0","x",hexDigit,hexDigit};//text validators
char buf1[]="0x11";//text edit target

prompt* mainData[]={
  new prompt("Op 1",(Menu::callback)op1Func,enterEvent),
  new prompt("Op 2"),//we can set/change text, function and event mask latter
  new menuField<typeof(test)>(test,"Bright","",0,255,10,1,doNothing,noEvent),
  new textField("Addr",buf1,4,hexNr),
  &subMenu,
  &durMenu,
  &fxMenu,
  &ledMenu,
  &padMenu,
  new Exit("<Exit.")
};
menuNode& mainMenu=*new menuNode("Main menu",sizeof(mainData)/sizeof(prompt*),mainData/*,doNothing,noEvent,wrapStyle*/);

#define MAX_DEPTH 2

//input -------------------------------------
serialIn in(Serial);

//serial output -------------------------------------
idx_t tops[MAX_DEPTH];
serialOut out(Serial,tops);

//outputs -------------------------------------
menuOut* constMEM outList[] MEMMODE={&out};
outputsList outs(outList,sizeof(outList)/sizeof(menuOut*));

//navigation control -------------------------------------
navNode path[MAX_DEPTH];
navRoot nav(mainMenu,path,MAX_DEPTH,in,outs);

void setup() {
  pinMode(LED,OUTPUT);
  setLed();
  Serial.begin(115200);
  while(!Serial);
  Serial.println("press any key...");
  while(!Serial.available());
  Serial.println("menu 4.x test");
  Serial.flush();
  mainMenu[0].shadow->text="Changed";
}

void loop() {
  nav.poll();//this device only draws when needed
  if(nav.sleepTask) {
    Serial.println();
    Serial.println("menu is suspended");
    Serial.println("presse [select] to resume.");
  }
  delay(100);//simulate a delay when other tasks are done
}
