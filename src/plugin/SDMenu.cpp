#include "SDMenu.h"

result SDMenu::sysHandler(SYS_FUNC_PARAMS) {
  switch(event) {
    case enterEvent:
      if (nav.root->navFocus!=nav.target) {//on sd card entry
        nav.sel=((SDMenu*)(&item))->selIdx;//restore context
      }
  }
  return proceed;
}

void SDMenu::doNav(navNode& nav,navCmd cmd) {
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

idx_t SDMenu::count() {
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

idx_t SDMenu::entryIdx(String name) {
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

String SDMenu::entry(idx_t idx) {
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

Used SDMenu::printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t pn) {
  ((menuNodeShadow*)shadow)->sz=count();
  if(root.navFocus!=this) //show given title
    return menuNode::printTo(root,sel,out,idx,len,pn);
  else if(idx==-1)//when menu open (show folder name)
    return out.printRaw(folderName.c_str(),len);
  //drawing options
  len-=out.printRaw(entry(idx).c_str(),len);
  return len;
}
