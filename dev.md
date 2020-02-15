# development notes


## TODO

### Code
- ~~rename files use static instead of dyn~~
- ~add namespace~~

### Print/Display/Format
+ ~~minimal draw and viewports~~
+ ~~check last drawn menu to draw titles (nav. back)~~
+ ~~printMenu should be delivered to the item after checking !parentPrint~~
- colors
- multiline options/title
- simpler printers
- frame borders
- review ANSI fmt

### Nav
+ ~~default "no action" item act as "Exit"~~
- ~~do not send activation for disabled items~~
- 2D nav

### Fields/Items
+ ~~enable/disable items~~
+ ~~add ::Part to all for consistency~~
- review default composition parts, we want max optionals
- add default debug part to all items (with empty expression on non-debug versions)
- numeric fields
- enumerated fields
- numeric input accel. and easing
- text fields
- inline menus (Pads)

### Input
- alternative inputs for composition, this requires the input to be passed, in ex. numeric accel. => doInput should check item before calling input handler

### Examples
- Arduino ANSI
- PC NCurses
