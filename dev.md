# development notes

## TODO

### Code
- ~~rename files use static instead of dyn~~
- ~~add namespace~~

### Print/Display/Format
+ ~~minimal draw and viewports~~
+ ~~check last drawn menu to draw titles (nav. back)~~
+ ~~printMenu should be delivered to the item after checking !parentPrint~~
+ ~~clear title line before title print~~
+ ~~reviewed print chain params~~
+ ~~pass boolean template params to normal params on non-trivial function to reduce code footprint~~
+ ~~multiple parallel outputs~~
- check usage of native device resolution instead of text-box
- colors
- multiline options/title
- simpler printers
- frame borders
- review ANSI fmt
- xml format
- bluetooth characteristic format
- json format
- websockets output

### Nav
+ ~~default "no action" item act as "Exit"~~
- ~~do not send activation for disabled items~~
- idle timeout
- 2D nav => ignore parent draw -> focus => activation -> left/right => validate+exit+nav up/down

### Fields/Items
+ ~~enable/disable items~~
+ ~~add ::Part to all components for consistency~~
+ ~~review default composition parts, we want max optionals~~
+ ~~numeric fields~~
- reflexivity => doInput will check changes
- numeric field decimal places format
- numeric field unit scales
- numeric field easing
- generic nav easing?
- add default debug part to all items (with empty expression on non-debug versions)
- enumerated fields
- enumerated fields out of scale handling
- numeric input accel. and easing
- text fields
- inline menus (Pads)
- eeprom and file text fields
- c++ array menus
- text list menus
- file menus
- file system menus


### Input
- alternative inputs for composition, this requires the input to be passed, in ex. numeric accel. => doInput should check item before calling input handler
- index accel
- custom accels


### Examples
+ ~~i2c LCD~~
- Arduino ANSI
- PC NCurses
