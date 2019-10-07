# ArduinoMenu 5

**Generic menu/interactivity system for the arduino framework**

## Development discussion about next menu version

This is an experimental area, please contribute with ideas, experience or code. Thank you.

### Why a new version

In a word, **size**.

Things I wish were available/standard:
- C++14 or +
- AVR stl

### Whats new

_**how we manage to solve size issue**_

In a word, **composition**

We made menu definitions static composable, this way small systems can use simpler versions and still have menu functionality.

**Composition instead of settings**

Settings require code to support the functionalities to be always present and settings values to be checked at runtime to finetune a feature.

Composition allows features to be added as needed, code will be presente only if needed, and we need not check at runtime for the functionality, eventually a feature might choose to use runtime settings, its allowed.

Composition also allows mixing of components, previous menus versions had a global setting to use ram or flash memory. Now we can mix data from either origin and even add extra origins such as file systems and eeproms.

TODO: this code base has only static structure menus, dynamic structures still missing.

## Example

**ATTiny85 menu**

```c++
#include <menu.h>
#include <menu/IO/arduino/printOut.h>
#include <menu/IO/arduino/streamIn.h>
#include <menu/comp/arduino/flashText.h>
#include <menu/fmt/text.h>
#include "fpsTicks.h"

using Out=MenuOut<//define oue menu output
  FullPrinter<//we want full printing (for the moment its the only printer)
    TextFmt<//we want text format, this text formats prints accel index, text cursor and ends items wirh text newline
      RangePanel<//we want the navigation to be retrained verticaly, scrolling if needed
        StaticPanel<0,0,20,4,ArduinoPrintOut<>>//will be using arduino default `Serial` port on a panel of 20x4 characters, so that we can test scroll
      >
    >
  >
>;

Out out;

const char op1_txt[] PROGMEM="Option 1";
const char op2_txt[] PROGMEM="Option 2";
const char op3_txt[] PROGMEM="...";
const char main_txt[] PROGMEM="Main menu";
const char sub_txt[] PROGMEM="Sub-menu";
using Op1=FlashText<decltype(op1_txt),&op1_txt>;
using Op2=FlashText<decltype(op2_txt),&op2_txt>;
using Op3=FlashText<decltype(op3_txt),&op3_txt>;

using MainMenu=StaticMenu<
  FlashText<decltype(main_txt),&main_txt>,
  StaticData<
    Op1,
    Op2,
    Op3,
    Op3,
    Op3,
    StaticMenu<
      FlashText<decltype(sub_txt),&sub_txt>,
      StaticData<Op1,Op2,Op3>
    >,
    Op3
  >
>;

MainMenu mainMenu;

StaticRoot<
  StaticNavTree<MainMenu,2,NavPos<>>
> nav(mainMenu);

using SerialIn=ArduinoStreamIn<>;

FPS<25> menuFps;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println(F("AM5 AVR Test."));
  Serial.println(F("This is a test for size, expect minimalist menu"));
  nav.print<Out>();
}

void loop() {
  if (menuFps&&nav.doInput<SerialIn>()) nav.print<Out>();
}
```

**footprints**
```text
current: Oct 2019
ATMega328, static tree menu with some associated actions
DATA:    [=         ]  10.2% (used 209 bytes from 2048 bytes)
PROGRAM: [=         ]  13.4% (used 4118 bytes from 30720 bytes)

ATTiny85 simpler static tree menu with some associated actions
DATA:    [==        ]  19.5% (used 100 bytes from 512 bytes)
PROGRAM: [====      ]  36.7% (used 3010 bytes from 8192 bytes)

previous:
ATA:    [==        ]  18.3% (used 374 bytes from 2048 bytes)
PROGRAM: [==        ]  15.3% (used 4704 bytes from 30720 bytes)

DATA:    [==        ]  19.1% (used 392 bytes from 2048 bytes)
PROGRAM: [==        ]  17.1% (used 5242 bytes from 30720 bytes)
```

### Embedded systems

The AM4 approach to library development is not sufficient for embedded systems, modularity by includes is not enough as a means of optimization. As we start adding features soon the menu becomes tight on small devices.
So we need to seek modularity even further.

After some research and experimentation here are some considerations about various aspects of menu systems with focus on embedding.

This can also be achieved with C style defines and code exclusion, both approaches are hard. Hopefully this one can be more succinct.

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

_(waiting for c++ `concept`'s, we migh reinforce this on C++11 when api is stable)_

It would be useless if we could not escape the type level. We want this different options of different type to co-exist on the same list. Therefor the interface usage. It defines a base menu item and its behavior.

### Lessons learned

From previous version we have been adding functionalities and adapting the system design, however always over the same assumptions... keep everything functional and interface contained instead.

Input manipulation should be independent, the system should only respond to navigation commands.

Output should be single device, multiple device should be implemented as a special device.

Finally found a decent initialization schema so that we can drop the macros (maybe keep them for compatibility/porting)

Target framework ~should not be~ _is not_ limited to Arduino (essay on Linux went well)

Keep menu definitions platform agnostic as was on AM4, make them also framework agnostic.

On version 4 we shifted the complexity to a central core, this makes IO drivers easier to implement and more efficient when using multiple outputs, but makes an extremely heavy core with many if's and considering too many cases, we need to break this down. Type level composition was the way.

Avoid castings, macros and other bad style c++

Work at type level whenever possible
