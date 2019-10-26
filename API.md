# ArduinoMenu 5

## Commands

_file_:`base.h`  
_type_:`enumeration`

_def_:
```c++
enum class Cmds:Idx {None=0,Activate=1,Enter=2,Esc=4,Up=8,Down=16,Left=32,Right=64};
```

Navigations commands represent user actions, they are issued at the navigation root and can bubble in the menu structure.
All items return a boolean in response to a command whos interpretation is subjective to the parent (sending) object.
Navigation root objecvt will probably send some command to the menu items, so that customized items can play theyr role.
Parent menu uses `canNav` function to help on its interpretations.

However, most of the time the parent object is a menu, so we will descrive some default behavior of a menu and its interpretation of returning results.

Navigation commands are to be used on navigation root objects when calling its `cmd` member function.

- **None** - no command available, to be return by input objects
- **Activate** - sent to a child item in response to an incoming Enter (see description below).
- **Enter** - user sent enter signal, send `Activate` command to selected child item
- **Esc** - this command is not sent to items, it is handled on navigation base by moving focus to previous level (preemptive escape).
- **Up** - user sent move up command
- **Down** - user sent move down command
- **Left** - user sent move left command
- **Right** -  user sent move right command
- ~**Select** - position focus by index~
- ~**Focus** - position focus by index~
- ~**Key** - user sent key code~

`None` and `Esc` commands are handled by the base navigation, they are not sent to menu items.

All other commands have subjective interpretations by the items, however the return value has some meaning for the navigation system.

`Activate` command is not obtained directly from input, is is used by active menu objects in translation of a enter and to query focused item about its ability to process commands.
Items that rerurn `true` and can process commands will be put on focus and will receive further commands. Items that dont process commands can return `false` to close the current menu (exit option implementation).

### Return meaning

The commands are sent by the navigation system to the active menu item.
Menu items can send some or new commands to child items, usually the selected item and do its own interpretation of the result.
The navigation system will then act upon the return value too.

**`Activate` command is sent to selected items**

|**Activate**|done|close||


**this commands are ment to be handled by the active item**

|Command|true|false||
|---|---|---|---|
|**Up**|done|proceed||
|**Down**|done|proceed||
|**Left**|done|proceed||
|**Right**|done|proceed||

_they can then call default behavior by letting the command proceed to the navigation object again and/or call navigation functions_

**when selected element can handle commands**

|Command|true|false||
|---|---|---|---|
|**Enter**|done|close|will send `Activate` to selected item and return true when changing focus|

**when selected element can NOT handle commands**

|Command|true|false||
|---|---|---|---|
|**Enter**|done|close|will send `Activate` to selected item and return true when changing focus|

**some items can interpret this commands, otherwise ignore**

|Command|true|false||
|---|---|---|---|
|**Select**|done|proceed|
|**Key**|done|proceed|

most of the commands (move commands) have an interpretation of proceed|done <=> false|true, meaning that the system will either:

- **proceed** proceed to send the command to next elements or whatever it does as a default action
- **done** terminate the command and proceed to the next, cosidering it has been somehow handled by the target item.

some commands have more specific behaviors

- **open** set focus to selected elements, the element will receive following commands
- **close** move focus to the parent element again
