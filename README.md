# ArduinoMenu 5

**Generic menu/interactivity system for the arduino framework**

## Development discussion about next menu version

This is an experimental area, please contribute with ideas, experience or code. Thank you.

### Why a new version

In a word, **size**.

Things I wish were available:
- C++14 or +
- AVR stl

## Current state

_tiny.ino_ example is using a single option print-out chain

with composing menu items, role description tags and role tag catch on output format

output is also a composition, we can compose role tag format handlers and translations.

```c++
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// LCD example with flash data (Arduino framework)
//
// ArduinoMenu libtary 5.x code example
// Output: LCD
// flash data
// Input: user serial driver

#include <menu/def/tinyArduino.h>
#include <menu/IO/lcdOut.h>

using namespace Menu;

// LCD /////////////////////////////////////////
#define RS 2
#define RW 4
#define EN A4
LiquidCrystal lcd(RS, RW, EN, A0, A1, A2, A3);

//menu output ------------------------
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

// Prompt<StaticMenu<5>> mainMenu("Main menu",&op1,&op2,&op3,&op4,&op5);
const char menuTitle_text[] PROGMEM="Main menu";
Op menuTitle(menuTitle_text);
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
```

**footprint:**
```text
current:
ATA:    [==        ]  18.3% (used 374 bytes from 2048 bytes)
PROGRAM: [==        ]  15.3% (used 4704 bytes from 30720 bytes)

previous:
DATA:    [==        ]  19.1% (used 392 bytes from 2048 bytes)
PROGRAM: [==        ]  17.1% (used 5242 bytes from 30720 bytes)
```

_tinyArduino.h_ defines `SerialOut`, `Op` and `FlashOp` as:
```c++
#include <streamFlow.h>//https://github.com/neu-rah/streamFlow
#include "../../menu.h"
#include "../printers.h"
#include "../comp/flashText.h"
#include "../comp/flashMenu.h"

namespace Menu {

  using namespace Menu;

  template<typename O>
  using MenuOut=AM5::MenuOutCap<O>;

  using FlashText=AM5::FlashTextDef<AM5::Empty>;

  using Text=AM5::Text<AM5::Empty>;

  using Item=AM5::Item;

  template<typename O>
  using Prompt=AM5::Prompt<O>;

  template<size_t n>
  using StaticMenu=AM5::StaticMenu<n,Text>;

  template<size_t n>
  using FlashMenu=AM5::StaticMenu<n,FlashText>;
};
```

https://gitter.im/ArduinoMenu/Lobby

### Embedded systems

The AM4 approach to library development is not sufficient for embedded systems, modularity by includes is not enough as a means of optimization. As we start adding features soon the menu becomes tight on small devices.
So we need to seek modularity even further.

After some research and experimentation here are some considerations about various aspects of menu systems with focus on embedding.

This can also be achieved with C style defines and code exclusion, both approaches are hard. Hopefully this one can be more succinct.

### Is it possible?

You might be using a single line display, therefore printing a menu title is useless and inconvenient.  
instead of having a run-time config and code checking if title enabled and skipping title prints on single line devices even if active make a menu system easy to use but also makes it heavier. Examples like this are behind all assumptions we make about a menu system.  
So instead of having extra runtime check/config we opt instead on having compile-time compositions, think it like, if you want the title on your menus you can simply include that part on the construct.  
Shifting the burden to compile time reduces the run-time checking, code size and increases speed.

_**technical:** using type to guide the composition decision, not used code vanish at compile time._

#### Mixed content

Instead of setting a menu structure to reside on flash or on ram we can use them mixed. And this is working.

#### No assumptions

Assumptions reveal most of the time, a trade or a burden, assuming that all prompts/options will have a text might not be correct, useful or sufficient, some systems might need multiple texts for multi-language.
Same goes to all assumptions about menus, even associated actions.

#### Composition (type level)

The key to obtain better modularity is by code composition using C++ mixins.

We make type level compositions that define a menu system type. That can be adequate for simple or complex system.

##### How things compose

Composition is done at type level using templates and open derivation chain, this is quite loose but this level is not intended for the final user, still it allow a great customization.

It would be useless if we could not escape the type level. We want this different options of different type to co-exist on the same list. Therefor the interface usage. It defines a base menu item and its behavior.

Even not being enforced by C++ we adopted an interface description with a middle composing stage comprised of multiple classes/functionalities with inline static members, this kind of members tend to vanish if not used.

The composed types then are used to construct a specific version for the interface, virtual functions will ensure that the correct type is used covering the composition as a monolith type.

Some compositions are weight-less and therefor should be included always. That is the case of role description tags, all members should have one, they are transparent and no trace of them would be left, but they can influenciate the code generated for certain print devices or output formats if they, the format or device, choose to do so.

**generic pattern**

Adapt(Interface)<Comp1<Comp2<Comp3<Terminal>>>>

```c++
//define common functionalities
struct Interface {
  //some virtual functions here
};

//make static composition adhere to the interface (as a top level cap)
template<typename O>
struct Adapt:public Interface,public O {
  //redirect virtual call to the correct type (because we know it)
};

// composition parts ----------------------

//this is the minimal composition part and acts as an interface for the composing parts
// however nothing requires you to derive from it as the members are `inline static`
// consider it just a guide, deriving from it is a discipline
struct Empty {
  //add base functionality to derived items, not enforced but handy
};

template<typename O=Empty>
class Text:public O {
protected:
  const char* text;
public:
  Text(const char* t):text(t) {}
  //... add specific implementations
};

//composing thing for user
using Op=Prompt<Text>;
//using Op=Prompt<FlashText>;//with this def we can put all Op's into flash (because they share constructor parameter format)

Op op1("Op 1");//now we can simply build an option like this

```
we can implement other building blocks _a la carte_  
they contain the functionality and its code is vacuous if not used

we might add some sugar on top of this construction methods and build more elaborated blocks for each system.

**extending**

on a separate file, meaning the library can be extended without changing library files

```c++
  template<typename O>
  class FlashTextDef:public O {
  protected:
    const char *text PROGMEM;
  public:
    FlashTextDef(PGM_P t):text(t) {}
    template<typename Out>
    inline size_t out(Out& o) {
      o.raw(reinterpret_cast<const __FlashStringHelper *>(text));
      return O::out(o);//chain the call
    }
  };
```

### Lessons learned

From previous version we have been adding functionalities and adapting the system design, however always over the same assumptions... Instead keep everything functional and interface contained.

Input manipulation should be independent, the system should only respond to navigation commands.

Output should be single device, multiple device should be implemented as a special device.

Finally found a decent initialization schema so that we can drop the macros (maybe keep them for compatibility/porting)

Target framework should not be limited to Arduino (essay on Linux went well)

Keep menu definitions platform agnostic as was on AM4, make them also framework agnostic.

On version 4 we shifted the complexity to a central core, this makes IO drivers easier to implement an is more efficient when using multiple outputs, but makes an extremely heavy core with many if's and considering too many cases, we need to break this down. Type level composition was the way.

Avoid castings, macros and other bad style c++

Work at type level whenever possible
