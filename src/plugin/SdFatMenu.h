/* -*- C++ -*- */
#pragma once
// a full automated SDCard file select
// plugin for arduino menu library
// requires a dynamic menu (MENU_USERAM)
// IO: Serial
// Feb 2019 - Rui Azevedo [ruihfazevedo@gmail.com]

//using SdFat library
//from: https://github.com/greiman/SdFat

#include <menu.h>

using namespace Menu;

#define FN_SIZE 255

//minimalist SD Card driver (using arduino SD)
//we avoid allocating memory here, instead we read all info from SD
template<typename SDC,int fnSize=FN_SIZE>
class FSO {
public:
  char fname[FN_SIZE+1];
  using Type=SDC;
  Type& sdc;
  //idx_t selIdx=0;//preserve selection context, because we preserve folder ctx too
  //we should use filename instead! idx is useful for delete operations thou...

  File dir;

  FSO(Type& sdc):sdc(sdc) {}
  virtual ~FSO() {dir.close();}
  //get file name
  //open a folder
  bool goFolder(String folderName) {
    dir.close();
    // Serial.println("reopen dir, context");
    dir=sdc.open(folderName.c_str());
    return dir;
  }
  //count entries on folder (files and dirs)
  long count() {
    // Serial.print("count:");
    dir.rewindDirectory();
    int cnt=0;
    while(true) {
      File file=dir.openNextFile();
      if (!file) {
        file.close();
        break;
      }
      file.close();
      cnt++;
    }
    // Serial.println(cnt);
    return cnt;
  }

  //get entry index by filename
  long entryIdx(String name) {
    dir.rewindDirectory();
    int cnt=0;
    while(true) {
      File file=dir.openNextFile();
      if (!file) {
        file.close();
        break;
      }
      if(file.getName(fname,fnSize)&&name==fname) {
        file.close();
        return cnt;
      }
      file.close();
      cnt++;
    }
    return 0;//stay at menu start if not found
  }

  //get folder content entry by index
  String entry(long idx) {
    dir.rewindDirectory();
    idx_t cnt=0;
    while(true) {
      File file=dir.openNextFile();
      if (!file) {
        file.close();
        break;
      }
      if(idx==cnt++) {
        file.getName(fname,fnSize);
        String n=String(fname)+(file.isDirectory()?"/":"");
        file.close();
        return n;
      }
      file.close();
    }
    return "";
  }

};

//////////////////////////////////////////////////////////////////////
// SD Card cached menu
template<typename SDC,idx_t maxSz,int fnSize=FN_SIZE>
class CachedFSO:public FSO<SDC,fnSize> {
public:
  using Type=SDC;
  long cacheStart=0;
  String cache[maxSz];
  long size=0;//folder size (count of files and folders)
  CachedFSO(Type& sdc):FSO<SDC>(sdc) {}
  void refresh(long start=0) {
    if (start<0) start=0;
    // Serial.print("Refreshing from:");
    // Serial.println(start);
    cacheStart=start;
    FSO<SDC>::dir.rewindDirectory();
    size=0;
    while(true) {
      File file=FSO<SDC>::dir.openNextFile();
      if (!file) {
        file.close();
        break;
      }
      if (start<=size&&size<start+maxSz) {
        file.getName(FSO<SDC>::fname,fnSize);
        cache[size-start]=String(FSO<SDC>::fname)+(file.isDirectory()?"/":"");
      }
      file.close();
      size++;
    }
  }
  //open a folder
  bool goFolder(String folderName) {
    if (!FSO<SDC>::goFolder(folderName)) return false;
    refresh();
    return true;
  }
  long count() {return size;}

  long entryIdx(String name) {
    idx_t sz=min(count(),(long)maxSz);
    for(int i=0;i<sz;i++)
      if (name==cache[i]) return i+cacheStart;
    long at=FSO<SDC>::entryIdx(name);
    //put cache around the missing item
    refresh(at-(maxSz>>1));
    return at;
  }
  String entry(long idx) {
    if (0>idx||idx>=size) return "";
    if (cacheStart<=idx&&idx<(cacheStart+maxSz)) return cache[idx-cacheStart];
    refresh(idx-(maxSz>>1));
    return entry(idx);
  }
};

////////////////////////////////////////////////////////////////////////////
#include <SdFat.h>
// instead of allocating options for each file we will instead customize a menu
// to print the files list, we can opt to use objects for each file for a
// faster reopening.. but its working quite fast
// On this example we assume the existence of an esc button as we are not drawing
// an exit option (or [..] as would be appropriate for a file system)
// not the mennu presents it self as the menu and as the options
// ands does all drawing navigation.
//TODO: we can specialize input too, for typing filename select
  #define USE_BACKDOTS 1

template<typename FS>
class SDMenuT:public menuNode,public FS {
public:
  String folderName="/";//set this to other folder when needed
  String selectedFolder="/";
  String selectedFile="";
  // using menuNode::menuNode;//do not use default constructors as we wont allocate for data
  SDMenuT(typename FS::Type& sd,constText* title,const char* at,Menu::action act=doNothing,Menu::eventMask mask=noEvent)
    :menuNode(title,0,NULL,act,mask,
      wrapStyle,(systemStyles)(_menuData|_canNav))
    ,FS(sd)
    {}

  void begin() {FS::goFolder(folderName);}

  //this requires latest menu version to virtualize data tables
  prompt& operator[](idx_t i) const override {return *(prompt*)this;}//this will serve both as menu and as its own prompt
  result sysHandler(SYS_FUNC_PARAMS) override {
    switch(event) {
      case enterEvent:
        if (nav.root->navFocus!=nav.target) {//on sd card entry
          nav.sel=((SDMenuT<FS>*)(&item))->entryIdx(((SDMenuT<FS>*)(&item))->selectedFile)+USE_BACKDOTS;//restore context
        }
    }
    return proceed;
  }

  void doNav(navNode& nav,navCmd cmd) {
    switch(cmd.cmd) {
      case enterCmd: if (nav.sel>=USE_BACKDOTS) {
          String selFile=SDMenuT<FS>::entry(nav.sel-USE_BACKDOTS);
          if (selFile.endsWith("/")) {
            // Serial.print("\nOpen folder...");
            //open folder (reusing the menu)
            folderName+=selFile;
            SDMenuT<FS>::goFolder(folderName);
            dirty=true;//redraw menu
            nav.sel=0;
          } else {
            //Serial.print("\nFile selected:");
            //select a file and return
            selectedFile=selFile;
            selectedFolder=folderName;
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
          SDMenuT<FS>::goFolder(folderName);
          dirty=true;//redraw menu
          nav.sel=SDMenuT<FS>::entryIdx(fn)+USE_BACKDOTS;
        }
        return;
    }
    menuNode::doNav(nav,cmd);
  }

  //print menu and items as this is a virtual data menu
  Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t pn) {
    if(root.navFocus!=this) {//show given title or filename if selected
      return selectedFile==""?
        /*menuNode::printTo(root,sel,out,idx,len,pn)*/
        out.printRaw(shadow->text,len):
        out.printRaw(selectedFile.c_str(),len);
    } else if(idx==-1) {//when menu open (show folder name)
      ((menuNodeShadow*)shadow)->sz=SDMenuT<FS>::count()+USE_BACKDOTS;
      idx_t at=folderName.lastIndexOf("/",folderName.length()-2)+1;
      String fn=folderName.substring(at,folderName.length()-1);
      return out.printRaw(fn.c_str(),len);
      // return out.printRaw(folderName.c_str(),len);
      // return out.printRaw(SDMenuT<FS>::dir.name(),len);
    }
    //drawing options
    out.setColor(fgColor, sel);
    idx_t i=out.tops[root.level]+idx;
    if (i<USE_BACKDOTS) len-=out.printRaw("[..]",len);
    else len-=out.printRaw(SDMenuT<FS>::entry(out.tops[root.level]+idx-USE_BACKDOTS).c_str(),len);
    return len;
  }
};

// class SDMenu:public SDMenuT<FSO<decltype(SdFat)>> {
// public:
//   SDMenu(SdFat& sd,constText* title,const char* at,Menu::action act=doNothing,Menu::eventMask mask=noEvent)
//     :SDMenuT<FSO<decltype(SdFat)>>(SD,title,at,act,mask) {}
// };
//
// template<idx_t cacheSize>
// class CachedSDMenu:public SDMenuT<CachedFSO<decltype(SdFat),cacheSize>> {
// public:
//   CachedSDMenu(constText* title,const char* at,Menu::action act=doNothing,Menu::eventMask mask=noEvent)
//     :SDMenuT<CachedFSO<decltype(SdFat),cacheSize>>(SD,title,at,act,mask) {}
// };
