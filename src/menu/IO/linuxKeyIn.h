/* -*- C++ -*- */
#pragma once

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

  struct LinuxKeyIn {
    template<typename In=None>
    struct Part:In {
      inline LinuxKeyIn() {set_conio_terminal_mode();}//capture the keyboard
      inline ~LinuxKeyIn() {reset_terminal_mode();}//capture the keyboard
      template<typename Nav>
      inline static bool cmd(Nav& nav) {
        if (kbhit()) {
          int k=getch();
          if(isdigit(k)) {
            Idx id=0;
            while().... //TODO: implement an integer parser here!
            //well for this to be a composition we gota separate the input reader!
            // and for that every reader must be a composition top.

          } else if (k==27&&kbhit()) k=getch();
          switch(k) {
            case 91:break;
            case 66: return nav.template cmd<Cmd::Up>();
            case 65: return nav.template cmd<Cmd::Down>();
            case 13:case 67: return nav.template cmd<Cmd::Enter>();
            case 27:case 68: return nav.template cmd<Cmd::Esc>();
            case 3://handle ctrl+c within the capturewd keyboard
              reset_terminal_mode();
              exit(0);
            default:break;
          }
        }
        return false;
      }
    };
  };
};
