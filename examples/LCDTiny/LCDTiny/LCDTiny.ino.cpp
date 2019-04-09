# 1 "/tmp/tmpp3hE8o"
#include <Arduino.h>
# 1 "/home/azevedo/Sketchbook/LIBDEV/ArduinoMenu/examples/LCDTiny/LCDTiny/LCDTiny.ino"
# 10 "/home/azevedo/Sketchbook/LIBDEV/ArduinoMenu/examples/LCDTiny/LCDTiny/LCDTiny.ino"
#include <menu/def/tinyArduino.h>
#include <menu/IO/lcdOut.h>

using namespace Menu;


#define RS 2
#define RW 4
#define EN A4
LiquidCrystal lcd(RS, RW, EN, A0, A1, A2, A3);


MenuOut<AM5::LCDFmt<>::To<LCDOutDev<lcd>>> menuOut;

using Op=Prompt<FlashText>;

const char op1_text[] PROGMEM="Op 1";
Op op1(op1_text);

const char op2_text[] PROGMEM="Op 2";
Op op2(op2_text);

const char op3_text[] PROGMEM="Op 3";
Op op3(op3_text);

const char op4_text[] PROGMEM="Op 4";
Op op4(op4_text);

const char op5_text[] PROGMEM="Op 5";
Op op5(op5_text);


const char menuTitle_text[] PROGMEM="Main menu";
Op menuTitle(menuTitle_text);
constexpr AM5::FlashData data[5] {&op1,&op2,&op3,&op4,&op5};
Prompt<AM5::FlashMenuDef<data,5,FlashText>> mainMenu(menuTitle_text);
void setup();
bool keys(int key);
void loop();
#line 47 "/home/azevedo/Sketchbook/LIBDEV/ArduinoMenu/examples/LCDTiny/LCDTiny/LCDTiny.ino"
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