/* -*- C++ -*- */
// a full automated SDCard file select
// plugin for arduino menu library
// IO: Serial
// Feb 2019 - Rui Azevedo [ruihfazevedo@gmail.com]

#include <menu.h>

using namespace Menu;

#ifndef MENU_USERAM
  #error "This requires a dynamic menu, not the default on AVR's, see wiki"
#else
  #include <SPI.h>
  #include <SD.h>
  // instead of allocating options for each file we will instead customize a menu
  // to print the files list, we can opt to use objects for each file for a
  // faster reopening.. but its working quite fast
  // avr's will be limited to 127 file (not a good idea)
  // this can be changed without breaking compatibility thou, but wasting more memory.
  // On this example we assume the existence of an esc button as we are not drawing
  // an exit option (or [..] as would be appropriate for a file system)
  // not the mennu presents it self as the menu and as the options
  // ands does all drawing navigation.
  class SDMenu:public menuNode {
  public:
    idx_t selIdx=0;//preserve selection context, because we preserve folder ctx too
    String folderName="/";//set this to other folder when needed
    String selectedFile="";
    // using menuNode::menuNode;//do not use default constructors as we wont allocate for data
    virtual ~SDMenu() {}
    SDMenu(constText* title,const char* at,Menu::action act=doNothing,Menu::eventMask mask=noEvent)
      :menuNode(title,0,NULL,act,mask,
        wrapStyle,(systemStyles)(_menuData|_canNav))
      ,folderName(at)
      {}

    //this requires latest menu version to virtualize data tables
    prompt& operator[](idx_t i) const override {return *(prompt*)this;}//this will serve both as menu and as its own prompt
    result sysHandler(SYS_FUNC_PARAMS) override;
    void doNav(navNode& nav,navCmd cmd) override;
    //count entries on folder (files and dirs)
    idx_t count();
    //get entry index by filename
    idx_t entryIdx(String name);
    //get folder content entry by index
    String entry(idx_t idx);
    //print menu and items as this is a virtual data menu
    Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t pn);
  };
#endif
