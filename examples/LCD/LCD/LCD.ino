#include <menu.h>
using namespace AM5;

// LCD /////////////////////////////////////////
#define RS 2
#define RW 4
#define EN A4
LiquidCrystal lcd(RS, RW, EN, A0, A1, A2, A3);

//the output description
using Out=StaticPanel<0,0, 10,4,LCDOut<lcd>>;

Out menuOut;

template<typename I>
Out& operator<<(Out& o,I i) {o.raw(i);return o;}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  lcd.begin(16,2);
  menuOut<<F("AM5 example ---")<<endl;
  delay(1500);
}

void loop() {}
