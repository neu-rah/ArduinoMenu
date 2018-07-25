/* -*- C++ -*- */
/***********
Sept. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

scan a chain of several input streams to provide input

***/

#ifndef RSITE_CHAINSTREAM_DEF_H
  #define RSITE_CHAINSTREAM_DEF_H

  #include "../menuDefs.h"

  namespace Menu {

    template <int N>
    class chainStream:public menuIn {
      public:
        static int on;
        menuIn** streams;
        chainStream<N>(menuIn** chain):streams(chain) {}
        void setFieldMode(bool mode) override {
          menuIn::setFieldMode(mode);
          for(int n=0;n<N;n++) streams[n]->setFieldMode(mode);
        }
        int available(void) {
          int cnt=0;
          for(int n=0;n<N;n++)
            cnt+=streams[n]->available();
          return cnt;
        }
        int peek(void) {
          for(int n=0;n<N;n++)
            if (streams[n]->available()) return streams[n]->peek();
          return -1;
        }
        int read() {
          for(int n=0;n<N;n++) {
            int key=streams[n]->available()?streams[n]->read():-1;
            if (key!=on) {
              on=-1;//key;
              return key;
            }
              //streams[n]->read();
              //if (key==-1) return -1;
              //while(streams[n]->peek()==key) streams[n]->read();//wait for key release
              //return key;
          }
          return -1;
        }
        void flush() {
          for(int n=0;n<N;n++)
            streams[n]->flush();
        }
        size_t write(uint8_t v) {return 0;}//this is readonly, ignoring
    };

    template<int N>
    int chainStream<N>::on=-1;

  }//namespace Menu

#endif
