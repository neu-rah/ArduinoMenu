/* -*- C++ -*- */

/**************
UTouch driver for arduino menu
turning utouch data into a menu input stream
checks menu option clicked or do menu scroll on drag

www.r-site.net

Dec. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

UTouch library from:
  http://henningkarlsen.com/electronics/library.php?id=56
***/
#ifndef RSITE_ARDUINO_MENU_UTOUCH
  #define RSITE_ARDUINO_MENU_UTOUCH

  #include <UTouch.h>
  #include "../menu.h"

  namespace Menu {

    class menuUTouch:public menuIn {
    public:
      UTouch& touch;
      navRoot& root;
      gfxOut& out;
      int startX,startY;
      int scrlY;
      bool touching;
      bool dragging;
      unsigned long evTime;
      menuUTouch(UTouch& t,navRoot& root,gfxOut& out):touch(t),root(root),out(out),touching(false),dragging(false) {}
      int available(void) {return touch.dataAvailable()?1:touching;}
      int peek(void) {return -1;}
      int read() {
        menuNode& m=root.active();
        panel p=out.panels[out.panels.cur];
        if (touch.dataAvailable()) {
          if (root.sleepTask) return enterCmd;
          evTime=millis();
          touch.read();
          startX=touch.getX()-p.x*out.resX;
          if (startX>out.maxX()*out.resX) return -1;
          int y=touch.getY()-p.y*out.resY;//-menuNode::activeNode->oy;
          if (y<0||y>out.maxY()*out.resY) return -1;
          //within menu box
          if (touching) {//might be dragging
            int d=scrlY-y;
            int ad=abs(d);
            if (ad>(out.resY>>1)&&(ad<<1)>out.resY) {
              dragging=true;//we consider it a drag
              scrlY-=(d>0?1:-1)*(out.resY/2);
              //prompt* nf=root.navFocus;
              /*if (nf->isMenu()&&!nf->isVariant()) {
                Serial<<"utouch scrolling "<<(d>0?"Up":"Down")<<endl;
                return (d>0?scrlUpCmd:scrlDownCmd);
              } else {*/
                //Serial<<"utouch moving "<<(d>0?"Up":"Down")<<endl;
                return (d>0?upCmd:downCmd);
              //}
            }
          }  else {//start new touching
            touching=true;
            dragging=false;
            startY=y;
            scrlY=y;
            evTime=millis();
          }
        } else {
          if (millis()-evTime<200) return -1;//debouncing
          touching=false;//touch ending
          if (dragging) return -1;
          int st=root.showTitle?1:0;
          if (root.navFocus->isMenu()&&!root.navFocus->parentDraw()) {
            int at=startY/out.resY;
            //Serial<<"utouch index select "<<((at>=st&&at<(m.sz()+st))?at-st+out.top(root.node())+'1':-1)<<endl;
            //Serial<<"canNav:"<<root.navFocus->canNav()<<"isVariant:"<<root.navFocus->isVariant()<<endl;
            return (at>=st&&at<(m.sz()+st))?at-st+out.top(root.node())+'1':-1;
          } else {//then its some sort of field
            prompt& a=m;//root.active();
            //Serial<<"utouch "<<(memStrLen(a.shadow->text)*out.resX<startX?"enter":"escape")<<endl;
            return
              ((int)(memStrLen(a.getText())*out.resX))<startX?
                enterCmd:
                escCmd;
          }
        }
        return -1;
      }
      void flush() {}
      size_t write(uint8_t v) {return 0;}
    };

  }//namespace Menu

#endif
