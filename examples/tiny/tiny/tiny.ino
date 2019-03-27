#include <menu/def/tinyArduino.h>
#include <menu/comp/multiLang.h>
#include <Dump.h>
using namespace Menu;

using FlashText=FlashTextDef<Empty>;

//string id's
enum LangCodes:size_t {textOk=0,textCancel,langStringsCnt};

//define multilang strings table on flash
//both the table and the strings will be on flash
// using constStr=const PROGMEM char*;
const PROGMEM char ok_en[]="Ok";
const PROGMEM char ok_pt[]="Vá";
const PROGMEM char cancel_en[]="Cancel";
const PROGMEM char cancel_pt[]="Esquece";

using MultiLang=Lang<FlashText>;
// define MultiLang::texts table
const PROGMEM FlashText enLang[]{ok_en,cancel_en};
const PROGMEM FlashText ptLang[]{ok_pt,cancel_pt};

MultiLang langs(enLang);

template<LangCodes id>
using LangOp=Prompt<MultiLang::Text<langs,id>>;

//normal option
SerialOut serialOut;
Op op1("Op 1");

//option using flash text
const char op2_text[] PROGMEM="Op 2";
FlashOp op2(op2_text);

LangOp<textOk> op3;
LangOp<textCancel> op4;

//they can fit on same array
//and will preserve the composed behavior
Item* ops[]{&op1,&op2,&op3,&op4};

void setup() {
  Serial.begin(115200);
  while(!Serial);
  serialOut<<"AM5 tiny example ----"<<endl;
  for(auto o: ops) serialOut<<*o<<endl;
  Serial<<"change language ----"<<endl;
  langs.setLangTable(ptLang);
  for(auto o: ops) serialOut<<*o<<endl;
  serialOut<<"----"<<endl;
}

void loop() {}
