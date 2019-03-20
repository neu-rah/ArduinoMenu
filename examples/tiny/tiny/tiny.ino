#include <streamFlow.h>
#include <menu/framework/arduino.h>

namespace ArduinoMenu{
  namespace Framework {
    namespace Arduino {
      #include <menu/def/tiny.h>
      #include <menu/fmt/text.h>
    }
  }
}

using namespace ArduinoMenu::Framework::Arduino;

////////////////////////////////////////////
// some ideas
//core, the printing engine
template<typename O>
struct EndlDef:public O {
  using Out=typename O::OutDef;
  using OutDef=Out;
  using O::O;
  Out& out(Out& o) const {//TODO: add non-const versions
    O::out(o);
    return o<<endl;
  }
};

template<typename O>
using Endl=EndlDef<O>;

//this is on the chain, we know about the menu type but not about
//the item types, we can not template virtuals, but...
//we might template a virtual class... will generate a lot of vtables
//still avoiding runtime composition until none left to explore....
template<typename O> //,template<typename> class Fmt>
struct PrintBody:public O {
  using Out=typename O::OutDef;
  using OutDef=Out;
  using O::O;
  Out& out(Out& o) const {//TODO: add non-const versions
    for(size_t n=0;n<O::size();n++)
      o<<O::operator[](n);//the only place to store this is?
    //explore alternative where this is also the format handler or composed with (not just a param)
    return o;
  }
};

//the printer!
//should start the print process and at same time handle role printing
//this way we keep both parts togheter
template<typename O>
struct Fmt {
  template<Roles role>
  static inline Out& out(Out& o) {return O::out(o);}
};

////////////////////////////////////////////////////////

const char op1_text[] PROGMEM="Op 1";
Op op1(op1_text);

const char op2_text[] PROGMEM="Op 2";
Op op2(op2_text);

const char menu_title[] PROGMEM="Main menu";
Item<PrintBody<Menu<2>>> mainMenu(menu_title,&op1,&op2);

inline Out& operator<<(Out& o,const Base& i) {
  i.operator<<(o);
  return o;
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial<<F("AM5 Tiny def test ---------------")<<endl;
  Serial<<mainMenu<<endl;
  Serial<<F("test end ---------------")<<endl;
}

void loop() {}
