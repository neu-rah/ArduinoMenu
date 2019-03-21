#include <streamFlow.h>

#include <menu/arduino/avr.h>
using namespace AM5;
using namespace AM5::Arduino;
using namespace AM5::Arduino::Avr;

// on a C/C++ std machine, but NOT on this .ino file
// #include <menu/std.h>
// using namespace AM5;
// using namespace AM5::Std;

using Root=RootDef<Framework,Platform>;
using Item=Item<Root>;
using Out=RawOut<decltype(Serial),Serial>;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial<<F("AM5 Tiny def test ---------------")<<endl;
  Serial<<F("Framework:")<<Framework::Name<<endl;
  Serial<<F("Platform:")<<Platform::Name<<" "<<(sizeof(Platform::Idx)<<3)<<"bits"<<endl;
  Serial<<F("test end ---------------")<<endl;
}

void loop() {}
