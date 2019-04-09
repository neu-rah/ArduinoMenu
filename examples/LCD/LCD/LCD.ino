// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// store text on flash (Arduino framework)
//
// ArduinoMenu libtary 5.x code example
// Output: Serial+LCD
// Input: user serial driver

#include <menu/def/tinyArduino.h>
#include <menu/IO/serialOut.h>
#include <menu/IO/lcdOut.h>
#include <menu/comp/numField.h>

using namespace Menu;

// LCD /////////////////////////////////////////
#define RS 2
#define RW 4
#define EN A4
LiquidCrystal lcd(RS, RW, EN, A0, A1, A2, A3);

//common nav node
using CommonNav=AM5::ItemNav<AM5::NavNode<>>;
CommonNav commonNav;

//to attach the nav node to output devices
template<typename O>
using Nav=AM5::SharedNavNode<O,CommonNav,commonNav>;

//menu output ------------------------
//define multiple outputs as one device
AM5::MenuOutCap<
  AM5::OutList<
    AM5::SerialFmt<Nav>::To<SerialOutDev<>>,
    AM5::LCDFmt<Nav>::To<LCDOutDev<lcd>>
  >
> menuOut;

using Op=Prompt<Text>;

int myvar=50;
Prompt<AM5::NumField<int>> fld(myvar,0,100,10,1);

// quick define menu
Prompt<StaticMenu<4>> mainMenu(
  "Main menu"
  ,new Op("Op 1")
  ,new Op("Op 2")
  ,&fld
  ,new Op("...")
);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  lcd.begin(16,2);
  menuOut<<"AM5 example ---";
  delay(1500);
  lcd.clear();
  menuOut.setTarget(mainMenu);
  menuOut.printMenu();
}

//handle serial keys to navigate menu
bool keys(int key) {
  switch(key) {
    case '+': return menuOut.up();;
    case '-': return menuOut.down();;
    case '*': return menuOut.enter();;
    case '/': return menuOut.esc();;
  }
  return false;
}

void loop() {
  if (Serial.available()) {
    if (keys(Serial.read())) menuOut.printMenu();
  }
}
