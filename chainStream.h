//scan a chain of several input streams to provide input
template <int N>
class chainStream:public Stream {
  public:
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
    for(int n=0;n<N;n++)
      if (streams[n]->available()) {
        int key=streams[n]->read();
        while(streams[n]->peek()==key) streams[n]->read();//wait for key release
        return key;
      }
    return -1;
  }
  void flush() {
    for(int n=0;n<N;n++)
      streams[n]->flush();
  }
  size_t write(uint8_t v) {return 0;}//this is readonly, ignoring
};


