# ArduinoMenu 5

**Generic menu/interactivity system for the arduino framework**

**AM5 is now framework and platform agnostic, can run even on PC's**

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

TODO: once API becomes stable we need to ease composition into common combinations and AM4 compatibility
