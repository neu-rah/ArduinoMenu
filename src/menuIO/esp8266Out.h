/* -*- C++ -*- */
#ifndef RSITE_ARDUINO_MENU_ESP8266OUT
  #define RSITE_ARDUINO_MENU_ESP8266OUT
  #include "../menu.h"

  namespace Menu {
    class esp8266Out:public menuOut {
    public:
      typedef const char* webColor;
      const colorDef<webColor> (&colors)[nColors];
      WiFiClient* client;
      esp8266Out(
        const colorDef<webColor> (&c)[nColors],
        idx_t* t,
        panelsList& p
      ):menuOut(t,p,redraw),colors(c) {}
      size_t write(uint8_t ch) override {return client->write(ch);}
      inline webColor getColor(colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) const {
        return (webColor)memPtr(&(stat==enabledStatus?colors[color].enabled[selected+edit]:colors[color].disabled[selected]));
      }
      menuOut& fill(
        int x1, int y1, int x2, int y2,char ch=' ',
        colorDefs color=bgColor,
        bool selected=false,
        status stat=enabledStatus,
        bool edit=false) override {
          /**client<<"<div styles=\""
            <<"background--color:"<<getColor(color,selected,stat,edit)
            <<" left:"<<x1<<" top:"<<y1<<" width:"<<x2-x1<<" height:"<<y2-y1;*/
          return *this;
      }
      void setCursor(idx_t x,idx_t y,idx_t panelNr=0) override {
        //*client<<"left:"<<x<<" top:"<<y;
      }
      void clear() override {panels.reset();}
      void clear(idx_t panelNr) override {
        setCursor(0,0,panelNr);
        panels.nodes[panelNr]=NULL;
      }
      void clearLine(
        idx_t ln,
        idx_t panelNr=0,
        colorDefs color=bgColor,
        bool selected=false,
        status stat=enabledStatus,
        bool edit=false
      ) override {
        const panel p=panels[panelNr];
        /**client<<"<li id=\"op"<<ln<<"\" styles=\" left:"<<p.x<<"px top:"<<ln+p.y
          <<"px background-color:"<<getColor(color,selected,stat,edit)
          <<" width:"<<p.x+p.w<<"px height:"<<p.y+ln<<"px\">";*/
      }
      void setColor(colorDefs c,bool selected=false,status s=enabledStatus,bool e=false) override {
        /**client<<" color:"<<getColor(c,selected,s,e);*/
      }
    };
  }//namespace Menu


#endif
