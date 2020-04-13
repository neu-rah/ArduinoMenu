#include <staticMenu.h>

template<const unsigned char b>
void test() {
  Serial.print("using ");
  Serial.println(b);
}

//here using a runtime value to seek a constexpr without having to extensivelly enumerate all cases...
// however compiler will enumerate them (just less typing)
//still a forward only api, we can not return
template<unsigned char o=0>
void callWithByte(unsigned char p) {
  if(o==p) test<o>();
  else callWithByte<o+1>(p);
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  const char x=Serial.read();
  callWithByte(x);
  return 0;
}

void loop() {}
