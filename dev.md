# development notes

## TODO

+ default "no action" item act as "Exit"
+ enable/disable
- minimal draw and viewports
- fields
- 2d nav
- multiline options/title
- simpler printers

## what about...

Empty::print<Op>()
  Op::Printing     -> obj().draw
  Op::Measure      -> obj().measure
  Op::ClearChanges -> obj().clear

split print into operations?
- clear should follow the print for consistency
- sending ops down to output print makes ez measuring for text
- spliting ops makes ClearChanges easy, less code
