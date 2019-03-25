/* -*- C++ -*- */
#pragma once

#include "../menu.h"

using Root=RootDef<Framework,Platform>;
using Item=Root::Item;
template<typename O>
using Prompt=typename Root::template Prompt<O>;
template<typename O>
using asTitle=typename Root::asTitle<O>;
using Op=Prompt<asTitle<TextDef<Root>>>;
template<const char** text>
using StaticOp=Prompt<asTitle<StaticTextDef<text,Root>>>;
using FlashOp=Prompt<asTitle<FlashTextDef<Root>>>;
using Out=AM5::SerialOut<
  decltype(Serial),
  Serial,
  Root::Fmt
>;

Out out;

//C/C++ static allocation still sucks, only works as it should for char* (FU!)
//normal prompt (can replace the text (pointer), but cant change the text directly)
Op op1("Op 1");
//static text prompt (type defines the text so you can not piunt it somewhere else)
const char*op2_text="Op 2";
StaticOp<&op2_text> op2;
//flash text prompt (same as normal text but )
const char op3_text[] PROGMEM="Op 3 from flash";
FlashOp op3(op3_text);

/////////////////////////////////////////////////
inline Out& operator<<(Out& o,Item& i) {o.out(i);return o;}
inline Out& operator<<(Out& o,const char i[]) {o.raw(i);return o;}
inline Out& operator<<(Out& o,char i) {o.raw(i);return o;}
inline Out& operator<<(Out& o,unsigned char i) {o.raw(i);return o;}
inline Out& operator<<(Out& o,int i) {o.raw(i);return o;}
inline Out& operator<<(Out& o,unsigned int i) {o.raw(i);return o;}
inline Out& operator<<(Out& o,long i) {o.raw(i);return o;}
inline Out& operator<<(Out& o,unsigned long i) {o.raw(i);return o;}
inline Out& operator<<(Out& o,double i) {o.raw(i);return o;}
//arduino specific!!!!
inline Out& operator<<(Out& o,const __FlashStringHelper *i) {o.raw(i);return o;}
inline Out& operator<<(Out& o,const String &i) {o.raw(i);return o;}
inline Out& operator<<(Out& o,const Printable&i) {o.raw(i);return o;}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial<<F("Framework:")<<Framework::Name<<endl;
  Serial<<F("Platform:")<<Platform::Name<<" "<<(sizeof(Platform::Idx)<<3)<<"bits"<<endl;
  Serial<<F("AM5 Tiny def test ---------------")<<endl;
  out<<op1;
  Serial<<endl;
  out<<op2;
  Serial<<endl;
  out<<op3;
  Serial<<endl;
  Serial<<F("test end ---------------")<<endl;
}

void loop() {}
