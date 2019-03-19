#include <streamFlow.h>
#include <menu/framework/arduino.h>

namespace ArduinoMenu{
  namespace Framework {
    namespace Arduino {
      #include <menu/tiny.h>
    }
  }
}

using namespace ArduinoMenu::Framework::Arduino;

////////////////////////////////////////////
// some ideas
//core, the printing engine
template<typename Out,typename O>
struct EndlDef:public O {
  using O::O;
  Out& out(Out& o) const {//TODO: add non-const versions
    O::out(o);
    return o<<endl;
  }
};

template<typename O>
using Endl=EndlDef<Out,O>;

template<typename Out,typename O> //,template<typename> class Fmt>
struct PrintBody:public O {
  using O::O;
  Out& out(Out& o) const {//TODO: add non-const versions
    for(size_t n=0;n<O::size();n++)
      o<<operator[](n);//the only place to store this is?
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

const char* op1_text="Op 1";
Op<&op1_text> op1;

const char* op2_text="Op 2";
Op<&op2_text> op2;

const char* menu_title="Main menu";
Item<
  PrintBody<
    Out,
    Menu<&menu_title,2>
  >
> mainMenu(&op1,&op2);

inline Out& operator<<(Out& o,const Base& i) {
  i.operator<<(o);
  return o;
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("AM5 Tiny def test");
  Serial<<mainMenu<<endl;
}

void loop() {}
