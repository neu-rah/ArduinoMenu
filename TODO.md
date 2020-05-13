# AM5 TODO's

**notes**

enumerated items need to draw focused element, but they meed to know if they are selected...

a) use events => not good for asyn API

b) capture cmds => nor good for async API

c) embue into target => its ok for async
  still target need to be aware of focus change
  this is similar to events (localized events)
  and implies capturing commands
  however havingthe functionality embedded into ther target makes it safe for async API

  => need a call to act() instead of activate
  because we need to call the functionality without changing the active element

  target needs a way to signal its selection beyond the CRTP obj()... still dont know how to do this

  this kind of tharget gotta have a reference to the grouping... use a grouping CRTP?

## Code improve
- [ ] separate item classes into functional and data *****  \
  _this is essential to reuse the functionality._
- [ ] abstract composition (as in Aqua code) ****
- [ ] need a better debug system (lower level)
- [ ] make focus and selected the same comcept, and use also active concept

## new AM5 features
- [ ] groupijng CRTP                              *****
- [ ] events                                      ****
- [ ] multiline items and custom formats          ****
- [ ] deached array data/array data sources       ***
- [ ] full and partial buffers (u8g2)             ***
- [ ] liquid layout                               ***
- [ ] device coordinates                          ***
- [ ] bluethooth enumeration for characteristics  **
- [ ] multilanguage                               **
- [ ] id buttons                                  **
- [ ] documentation                               *
- [ ] common mixins                               *
- [ ] AM4 import                                  *
- [ ] icons menu                                  *

## missing AM4 features
- [ ] fields need to have associated action       *****
- [ ] optionally update on seletion or focus      *****
- [ ] array/virtual/std custom draw               *****
- [x] enumerated fields                           *****
- [ ] pad menus                                   ****
- [ ] async api                                   ****
- [ ] idle                                        ***
- [ ] minimal draw                                ***
- [ ] display drivers                             ***
- [ ] navigation wrap                             **
- [ ] field wrap                                  **
- [ ] xml export (as AM5 fmt)                     **
- [ ] json export (as AM5 fmt)                    **
- [ ] web interface                               **
- [ ] http output                                 **
- [ ] websocket output                            **
- [ ] AM4 events                                  *
- [ ] panels                                      *
- [ ] buttons                                     *
