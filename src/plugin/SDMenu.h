/* -*- C++ -*- */
#pragma once
// a full automated SDCard file select
// plugin for arduino menu library
// requires a dynamic menu (MENU_USERAM)
// IO: Serial
// Feb 2019 - Rui Azevedo [ruihfazevedo@gmail.com]
#include <menu.h>

using namespace Menu;

#ifdef MENU_USERAM
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
  template<typename SDC>
  class SDMenu:public menuNode {
  public:
    SDC& SD;
    idx_t selIdx=0;//preserve selection context, because we preserve folder ctx too
    String folderName="/";//set this to other folder when needed
    String selectedFile="";
    // using menuNode::menuNode;//do not use default constructors as we wont allocate for data
    virtual ~SDMenu() {}
    SDMenu(SDC& sd,constText* title,const char* at,Menu::action act=doNothing,Menu::eventMask mask=noEvent)
      :menuNode(title,0,NULL,act,mask,
        wrapStyle,(systemStyles)(_menuData|_canNav))
      ,SD(sd)
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

  template<typename SDC>
  result SDMenu<SDC>::sysHandler(SYS_FUNC_PARAMS) {
    switch(event) {
      case enterEvent:
        if (nav.root->navFocus!=nav.target) {//on sd card entry
          nav.sel=((SDMenu*)(&item))->selIdx;//restore context
        }
    }
    return proceed;
  }

  template<typename SDC>
  void SDMenu<SDC>::doNav(navNode& nav,navCmd cmd) {
    switch(cmd.cmd) {
      case enterCmd: {
          String selFile=entry(nav.sel);
          if (selFile.endsWith("/")) {
            // Serial.print("\nOpen folder...");
            //open folder (reusing the menu)
            folderName+=selFile;
            dirty=true;//redraw menu
            nav.sel=0;
          } else {
            selIdx=nav.sel;
            //Serial.print("\nFile selected:");
            //select a file and return
            selectedFile=selFile;
            nav.root->node().event(enterEvent);
            menuNode::doNav(nav,escCmd);
          }
          return;
        }
      case escCmd:
        if(folderName=="/")//at root?
          menuNode::doNav(nav,escCmd);//then exit
        else {//previous folder
          idx_t at=folderName.lastIndexOf("/",folderName.length()-2)+1;
          String fn=folderName.substring(at,folderName.length()-1);
          folderName.remove(folderName.lastIndexOf("/",folderName.length()-2)+1);
          // Serial.println(folderName);
          dirty=true;//redraw menu
          nav.sel=entryIdx(fn);
        }
        selIdx=nav.sel;
        return;
    }
    menuNode::doNav(nav,cmd);
    selIdx=nav.sel;
  }

  template<typename SDC>
  idx_t SDMenu<SDC>::count() {
    File dir=SD.open(folderName.c_str());
    int cnt=0;
    while(true) {
      File file=dir.openNextFile();
      if (!file) {
        file.close();
        dir.close();
        break;
      }
      file.close();
      cnt++;
    }
    dir.close();
    return cnt;
  }

  template<typename SDC>
  idx_t SDMenu<SDC>::entryIdx(String name) {
    File dir=SD.open(folderName.c_str());
    int cnt=0;
    while(true) {
      File file=dir.openNextFile();
      if (!file) {
        file.close();
        break;
      }
      if(name==file.name()) {
        file.close();
        dir.close();
        return cnt;
      }
      file.close();
      cnt++;
    }
    dir.close();
    return 0;//stay at menu start if not found
  }

  template<typename SDC>
  String SDMenu<SDC>::entry(idx_t idx) {
    File dir=SD.open(folderName.c_str());
    idx_t cnt=0;
    while(true) {
      File file=dir.openNextFile();
      if (!file) {
        file.close();
        break;
      }
      if(idx==cnt++) {
        String n=String(file.name())+(file.isDirectory()?"/":"");
        file.close();
        dir.close();
        return n;
      }
      file.close();
    }
    dir.close();
    return "";
  }

  template<typename SDC>
  Used SDMenu<SDC>::printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t pn) {
    ((menuNodeShadow*)shadow)->sz=count();
    if(root.navFocus!=this) {//show given title or filename if selected
      return selectedFile==""?
        menuNode::printTo(root,sel,out,idx,len,pn):
        out.printRaw(selectedFile.c_str(),len);
    }
    else if(idx==-1)//when menu open (show folder name)
      return out.printRaw(folderName.c_str(),len);
    //drawing options
    len-=out.printRaw(entry(idx).c_str(),len);
    return len;
  }

#endif
