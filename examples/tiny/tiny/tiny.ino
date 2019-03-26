#include <menu/def/tinyArduino.h>
#include <menu/comp/multiLang.h>
#include <Dump.h>
using namespace Menu;


//string id's
enum LangCodes:size_t {textOk=0,textCancel,langStringsCnt};

//define multilang strings table on flash
//both the table and the strings will be on flash
// using constStr=const PROGMEM char*;
const PROGMEM char ok_en[]="Ok";
const PROGMEM char ok_pt[]="Vá";
const PROGMEM char cancel_en[]="Cancel";
const PROGMEM char cancel_pt[]="Esquece";

using MultiLang=Lang<FlashTextDef<Empty>>;
//define MultiLang::texts table
const PROGMEM MultiLang::TextsType enLang[]{ok_en,cancel_en};
const PROGMEM MultiLang::TextsType ptLang[]{ok_pt,cancel_pt};

template<LangCodes id>
using LangOp=Prompt<asTitle<MultiLang::Text<id>>>;

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
  for(int n=0;n<sizeof(ops)/sizeof(Item*);n++) {
    serialOut<<*ops[n];
    Serial<<endl;
  }
  Serial<<"change language ----"<<endl;
  MultiLang::setLangTable(ptLang);
  for(int n=0;n<sizeof(ops)/sizeof(Item*);n++) {
    serialOut<<*ops[n];
    Serial<<endl;
  }
}

void loop() {}
