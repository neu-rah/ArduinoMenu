/* -*- C++ -*- */
#pragma once

//POSIX PC Keyboard

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>

namespace Menu {
  struct termios orig_termios;

  void reset_terminal_mode() {tcsetattr(0, TCSANOW, &orig_termios);}

  void set_conio_terminal_mode() {
      struct termios new_termios;
      tcgetattr(0, &orig_termios);
      memcpy(&new_termios, &orig_termios, sizeof(new_termios));
      atexit(reset_terminal_mode);
      cfmakeraw(&new_termios);
      tcsetattr(0, TCSANOW, &new_termios);
  }

  int kbhit() {
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
  }

  int getch() {
    unsigned char c;
    int r=read(0, &c, sizeof(c));
    return r<0?r:c;
  }

  //POSIX system key reader
  struct LinuxKeyIn {
    template<typename In>
    struct Part:In {
      static constexpr bool isReader=true;
      inline Part() {set_conio_terminal_mode();}//capture the keyboard
      inline ~Part() {reset_terminal_mode();}//capture the keyboard
      template<typename Nav>
      inline bool parseKey(Nav& nav) {
        if(!kbhit()) return In::parseKey(nav);
        bool ext=false;
        int k=getch();
        trace(MDO<<"LinuxKeyIn::parseKey "<<k<<endl);
        if (k==3) {
          //preemptive handle Ctrl+C
          reset_terminal_mode();
          exit(0);
        }
        if (k==27&&kbhit()) {
          ext=true;
          k=getch();
          trace(MDO<<"ext key:"<<k<<endl);
        }
        return In::parseCmd(nav,k,ext);
      }
    };
  };

  //PC keyboard arrows
  struct PCArrows {
    template<typename In>
    struct Part:In {
      static constexpr bool isParser=true;
      template<typename Nav>
      bool parseCmd(Nav& nav,Key k,bool e=false) {
        if (e&&k==91) return false;//wait for ext code
        switch(k) {
          case 66: return nav.template cmd<Cmd::Up>();
          case 65: return nav.template cmd<Cmd::Down>();
          case 13:case 67: return nav.template cmd<Cmd::Enter>();
          case 27:case 68: return nav.template cmd<Cmd::Esc>();
          default:break;
        }
        _trace(MDO<<"PCArrows passing key"<<endl);
        return In::parseCmd(nav,k,e);
      }
    };
  };

};
