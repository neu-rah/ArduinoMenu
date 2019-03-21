#include <streamFlow.h>

#include <menu/arduino/avr.h>
// using namespace AM5;
using namespace AM5::Arduino;
using namespace AM5::Arduino::Avr;

// on a C/C++ std machine, but NOT on this .ino file
// #include <menu/std.h>
// using namespace AM5;
// using namespace AM5::Std;

using Root=AM5::RootDef<Framework,Platform>;
using Item=Root::Item;
using Out=AM5::RawOut<
  decltype(Serial),
  Serial,
  Root::Fmt
>;

Out SerialOut;

inline Out& operator<<(Out& o, Item& i) {
  o.out(i);
  return o;
}

template<typename T>
inline Out& operator<<(Out& o, T& i) {
  o.raw(i);
  return o;
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial<<F("AM5 Tiny def test ---------------")<<endl;
  Serial<<F("Framework:")<<Framework::Name<<endl;
  Serial<<F("Platform:")<<Platform::Name<<" "<<(sizeof(Platform::Idx)<<3)<<"bits"<<endl;
  SerialOut<<"Ok"<<endl;
  Serial<<F("test end ---------------")<<endl;
}

void loop() {}
