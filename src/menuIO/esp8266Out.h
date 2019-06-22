/* -*- C++ -*- */

#ifndef RSITE_ARDUINO_MENU_ESP8266OUT
  #define RSITE_ARDUINO_MENU_ESP8266OUT
  #ifdef ESP8266
    #include "../menuDefs.h"
    #include <ESP8266WiFi.h>
    // based on WebServer:
    //   https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer
    //   https://github.com/Links2004/arduinoWebSockets
    #include <WebSocketsServer.h>
    #include <ESP8266WebServer.h>
    #include <vector>
    #include "xmlFmt.h"

    using namespace std;

    namespace Menu {

      class esp8266Out:public webOut {
        public:
          esp8266Out(
            idx_t* t,
            panelsList& p,
            menuOut::styles styles=(menuOut::styles)(redraw|expandEnums)
          ):webOut(t,p,styles) {}
          menuOut& fill(
            int x1, int y1, int x2, int y2,char ch=' ',
            colorDefs color=bgColor,
            bool selected=false,
            status stat=enabledStatus,
            bool edit=false) override {return *this;}
          void setCursor(idx_t x,idx_t y,idx_t panelNr=0) override {}
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
          ) override {}
          void setColor(colorDefs c,bool selected=false,status s=enabledStatus,bool e=false) override {};
          //template<typename T> esp8266Out& operator<<(T t)=0;
      };

      class esp8266BufferedOut:public esp8266Out {
        public:
          String response;
          using esp8266Out::esp8266Out;
          template<typename T> inline esp8266BufferedOut& operator<<(T t) {response<<t;return *this;}
          size_t write(uint8_t ch) override {response+=(char)ch;return 1;}
      };

      menuOut& operator<<(menuOut&o,classes c);
      template<typename T> inline String& operator<<(String& o,T t) {return o.operator+=(t);}

      class esp8266_WebServerOut:public esp8266BufferedOut {
        public:
          ESP8266WebServer &server;
          //using esp8266Out::esp8266Out;
          esp8266_WebServerOut(
            ESP8266WebServer &srv,
            /*const colorDef<esp8266Out::webColor> (&c)[nColors],*/
            idx_t* t,
            panelsList& p
          ):esp8266BufferedOut(t,p),server(srv) {}
          size_t write(uint8_t ch) override {response<<(char)ch;return 1;}
          // template<typename T> inline esp8266_WebServerOut& operator<<(T t) {response<<t;return *this;}
      };

      class esp8266_WebServerStreamOut:public esp8266Out {
        public:
          ESP8266WebServer &server;
          inline esp8266_WebServerStreamOut(
            ESP8266WebServer &srv,
            idx_t* t,
            panelsList& p
          ):esp8266Out(t,p),server(srv) {}
          inline size_t write(uint8_t ch) override {
            char c[2]={ch,0};
            server.sendContent(c);
            return 1;
          }
          template<typename T> inline esp8266_WebServerStreamOut& operator<<(T t) {
            server.sendContent(String()+t);
            return *this;
          }
      };

      // class esp8266_WebSocketOut:public esp8266BufferedOut {
      //   public:
      //     //client num... well we only serve a client at a time, so its safe to put t here
      //     // of course this kind of code is not thead safe...but we dont have theads anyway
      //     uint8_t num;
      //     String response;
      //     WebSocketsServer &webSocket;
      //     esp8266_WebSocketOut(WebSocketsServer &socket,idx_t* tops,panelsList& panels)
      //       :webSocket(socket),esp8266BufferedOut(tops,panels,(menuOut::styles)(minimalRedraw|expandEnums)) {}
      //     size_t write(uint8_t ch) override {response<<(char)ch;return 1;}
      // };

      //template<typename T> inline esp8266_WebServerOut& operator<<(esp8266_WebServerOut&o , T t) {return o.operator<<(t);}
      //template<typename T> inline esp8266_WiFiClientOut& operator<<(esp8266_WiFiClientOut&o , T t) {return o.operator<<(t);}
      //template<typename T> inline esp8266Out& operator<<(esp8266Out&o , T t) {return o.operator<<(t);}

    }//namespace Menu
    #include "esp8266Out.hpp"
  #endif
#endif
