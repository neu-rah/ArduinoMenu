# development notes

## TODO

+ default "no action" item act as "Exit"
+ enable/disable
+ minimal draw and viewports
- check last drawn menu to draw titles (nav. back)
- printMenu should be delivered to the item after checking !parentPrint
- do not send activation for disabled items
- colors
- rename files use static instead on dyn
- fields
- 2d nav
- multiline options/title
- simpler printers
- alternative inputs for composition, this requires the input to be passed, in ex. numeric accel. => doInput should check item before calling input handler

## what about...

Empty::print<Op>()
  Op::Printing     -> obj().draw
  Op::Measure      -> obj().measure
  Op::ClearChanges -> obj().clear

split print into operations?
- clear should follow the print for consistency
- sending ops down to output print makes ez measuring for text
- spliting ops makes ClearChanges easy, less code
