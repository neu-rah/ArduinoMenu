/* -*- C++ -*- */
/**************

RotaryEventIn.h

March 2020 M. Smit - github@gangkast.nl
Generic Rotary/Button input

purpose:

  Having a generic rotary event-based implementation,
  leaving rotary and button libraries up to the user.

  TODO: userland rotary/button event mapping to menu actions,
  as doubleclick/longpress are now hardcoded to back.

***/


// not sure where to put this..
template<class T> inline T operator~ (T a) { return (T)~(int)a; }
template<class T> inline T operator| (T a, T b) { return (T)((int)a | (int)b); }
template<class T> inline T operator& (T a, T b) { return (T)((int)a & (int)b); }
template<class T> inline T operator^ (T a, T b) { return (T)((int)a ^ (int)b); }
template<class T> inline T& operator|= (T& a, T b) { return (T&)((int&)a |= (int)b); }
template<class T> inline T& operator&= (T& a, T b) { return (T&)((int&)a &= (int)b); }
template<class T> inline T& operator^= (T& a, T b) { return (T&)((int&)a ^= (int)b); }


#ifndef __rotaryEventIn_h__
  #define __rotaryEventIn_h__
  #include "../menuDefs.h"

  namespace Menu {

    class RotaryEventIn:public menuIn {

      public:

        enum EventType {
          BUTTON_CLICKED            = 1 << 0,
          BUTTON_DOUBLE_CLICKED     = 1 << 1,
          BUTTON_LONG_PRESSED       = 1 << 2,

          ROTARY_CW                 = 1 << 3,
          ROTARY_CCW                = 1 << 4,
        };

        EventType config;
        EventType events;  // we could do a fifo if we miss events

        RotaryEventIn(EventType c)
          :config(c) {
          // config for future use. we could raise if
          // we are missing essential stuff
          // and we need to absorb an arg anyway...
        }

        void registerEvent(EventType e) {
          events |= e; // add it to the current events
        }

        int peek(void) override { return events; }
        int available(void) override {return peek() != 0;}

        int read() override {
          // enterCmd
          if (events & EventType::BUTTON_CLICKED) {
            events &= ~EventType::BUTTON_CLICKED; // remove from events
            return options->navCodes[enterCmd].ch;
          }
          // escCmd
          else if (events & (EventType::BUTTON_DOUBLE_CLICKED|EventType::BUTTON_LONG_PRESSED)) {
            events &= ~(EventType::BUTTON_DOUBLE_CLICKED|EventType::BUTTON_LONG_PRESSED); // remove
            return options->navCodes[escCmd].ch;
          }
          // downCmd
          else if (events & EventType::ROTARY_CW) {
            events &= ~EventType::ROTARY_CW; // remove from events
            return options->navCodes[upCmd].ch; // down sends up on menu? bug?
          }
          // upCmd
          else if (events & EventType::ROTARY_CCW) {
            events &= ~EventType::ROTARY_CCW; // remove from events
            return options->navCodes[downCmd].ch; // up sends down on menu? bug?
          }

          else
            return -1;
        }

        void flush() override {}
        size_t write(uint8_t v) override {return 0;}

    }; // class


  }//namespace Menu



#endif /* __rotaryEventIn_h__ */
