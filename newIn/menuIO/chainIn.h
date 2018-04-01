/* -*- C++ -*- */
/***********
Sept. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

scan a chain of several input chain to provide input

***/

#ifndef RSITE_CHAINSTREAM_DEF_H
  #define RSITE_CHAINSTREAM_DEF_H

  #include "../menu.h"

  namespace Menu {

    template <int N>
    class chainIn:public streamIn {
      public:
        // static int on;
        menuIn** chain;
        chainIn<N>(menuIn** chain):chain(chain) {}
        void setFieldMode(bool mode) override {
          menuIn::setFieldMode(mode);
          for(int n=0;n<N;n++) chain[n]->setFieldMode(mode);
        }
        int available(void) override {
          int cnt=0;
          for(int n=0;n<N;n++)
            cnt+=chain[n]->available();
          return cnt;
        }
        navCmds peek(void) override {
          for(int n=0;n<N;n++)
            if (chain[n]->peek()!=noCmd) return chain[n]->peek();
          return noCmd;
        }
        navCmds getCmd() override {
          // Serial<<"chainIn::getCmd ";
          for(int n=0;n<N;n++) {
            // Serial<<n;
            if (chain[n]->peek()!=noCmd) {
              auto r=chain[n]->getCmd();
              Serial<<"chain read:"<<r<<endl;
              return r;
            }
          }
          // Serial<<endl;
          return noCmd;
        }
    };

    // template<int N>
    // int chainIn<N>::on=-1;

  }//namespace Menu

#endif
