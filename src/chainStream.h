/***********
Sept. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extendable: Yes

scan a chain of several input streams to provide input
***/

#ifndef RSITE_CHAINSTREAM_DEF_H
#define RSITE_CHAINSTREAM_DEF_H

template <int N>
class chainStream:public Stream {
  public:
  static int on;
  Stream** streams;
  chainStream<N>(Stream** chain):streams(chain) {}
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
        on=-1;
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
#endif
