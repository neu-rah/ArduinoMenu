// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// LCD example with flash data (Arduino framework)
//
// ArduinoMenu libtary 5.x code example
// menu with associated actions
// Output: LCD
// flash data
// Input: user serial driver

#include <menu/def/arduino/avr.h>
#include <menu/IO/lcdOut.h>

using namespace Menu;

// LCD /////////////////////////////////////////
#define RS 2
#define RW 4
#define EN A4
LiquidCrystal lcd(RS, RW, EN, A0, A1, A2, A3);

//menu output ------------------------
MenuOut<AM5::LCDFmt<>::To<LCDOutDev<lcd>>> menuOut;

template<AM5::ActionHandler act=AM5::doNothing>
using Op=Prompt<AM5::Action<FlashText,act>>;

bool op1_handler() {
  lcd.clear();
  lcd.print(F("Op1 action!"));
  delay(1000);
  return true;
}

bool shared_handler() {
  lcd.clear();
  lcd.print(F("Shared handler!"));
  lcd.setCursor(0, 1);
  lcd.print(F("op:"));
  lcd.print(menuOut.pos()+1);
  delay(1000);
  return true;
}

const char op1_text[] PROGMEM="Op 1";
Op<op1_handler> op1(op1_text);

const char op2_text[] PROGMEM="Op 2";
Op<shared_handler> op2(op2_text);

const char op3_text[] PROGMEM="Op 3";
Op<shared_handler> op3(op3_text);

const char op4_text[] PROGMEM="Op 4";
Op<shared_handler> op4(op4_text);

const char op5_text[] PROGMEM="Op 5";
Op<shared_handler> op5(op5_text);

// Prompt<StaticMenu<5>> mainMenu("Main menu",&op1,&op2,&op3,&op4,&op5);
const char menuTitle_text[] PROGMEM="Main menu     ";
Op<> menuTitle(menuTitle_text);
constexpr AM5::FlashData data[5] {&op1,&op2,&op3,&op4,&op5};
Prompt<AM5::FlashMenuDef<data,5,FlashText>> mainMenu(menuTitle_text);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  lcd.begin(16,2);
  menuOut<<F("AM5 example ----")<<endl;
  menuOut<<F("<www.r-site.net>")<<endl;
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
