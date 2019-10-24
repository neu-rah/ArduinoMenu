# ArduinoMenu 5

## Commands

_file_:`base.h`  
_type_:`enumeration`

_def_:
```c++
enum class Cmds:Idx {None=0,Activate=1,Enter=2,Esc=4,Up=8,Down=16,Left=32,Right=64};
```

Navigation commands are to be used on navigation root objects when calling its `cmd` member function.
This commands describe a navigation action, most of them are then sent to the active menu item.

Input devices might return some of this codes when queried by yhe navigation system.

- **None** - no command available, to be return by input objects
- **Activate** - sent to a child item to query about focus acceptance, items that can handle commands should - return true, they will then be on focus and receive next commands
- **Enter** - user sent enter signal,
- **Esc** - this command is not sent to items, it is handled on navigation base by moving focus to previous level.
- **Up** - user sent move up command
- **Down** - user sent move down command
- **Left** - user sent move left command
- **Right** -  user sent move right command
- ~**Select** - position focus by index~
- ~**Focus** - position focus by index~
- ~**Key** - user sent key code~

`None` and `Esc` commands are handled by the base navigation, they are not sent to menu items.

All other commands habe subjective interpretations by the items, however the return value has some meaning for the navigation system.

`Activate` command is not obtained directly from input, is is used by active menu objects in translation of a enter and to query focused item about its ability to process commands.
Items that rerurn true will be put on focus, if using default behavior of a parent menu, and will receive the next commands.

### Return meaning

This is some expectable behavior, however system elements are free to implement otherwise.

The commands are sent by the navigation system to the active menu item. The navigation system will then act upon the return value.

**Active command is sent to selected items, quering focus candidate (can handle commanda)**

|**Activate**|done|close||


**this commands are ment to be handled by the active item**

|Command|true|false||
|---|---|---|---|
|**Up**|done|proceed||
|**Down**|done|proceed||
|**Left**|done|proceed||
|**Right**|done|proceed||

*when selected element can handle commands*

|Command|true|false||
|---|---|---|---|
|**Enter**|done|close|will send `Activate` to selected item and return true|

*when selected element can NOT handle commands*

|Command|true|false||
|---|---|---|---|
|**Enter**|done|close|will send `Activate` to selected item and return true|

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
