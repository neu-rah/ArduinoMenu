#include <menu/def/tinyArduino.h>
#include <menu/comp/multiLang.h>
#include <Dump.h>
using namespace Menu;


//available languages
enum Langs {en,pt,langsCnt};

//string id's
enum LangCodes:size_t {textOk,textCancel,langStringsCnt};

//define multilang strings table on flash
//both the table and the strings will be on flash
using constStr=const PROGMEM char*;
using MultiLang=Lang<Langs,langStringsCnt,const constStr>;
const PROGMEM char ok_en[]="Ok";
const PROGMEM char ok_pt[]="Vá";
const PROGMEM char cancel_en[]="Cancel";
const PROGMEM char cancel_pt[]="Esquece";
//define MultiLang::texts table
namespace menu {
  template<>
  const PROGMEM constStr MultiLang::texts[][2] {
    {ok_en,ok_pt},//textOk
    {cancel_en,cancel_pt}//textCancel
  };
}

template<LangCodes id>
using LangOp=Prompt<asTitle<MultiLang::Text<id,FlashTextDef<Empty>>>>;

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

Prompt<FlashTextDef<Empty>> l00(MultiLang::texts[0][0]);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial<<"RAM:"<<endl;
  // dumpRam(Serial, MultiLang::texts[0][0],64);
  // Serial<<"PGM:"<<endl;
  dumpPgm(Serial, MultiLang::texts[0][0],16);
  dumpPgm(Serial, MultiLang::texts[1][0],16);
  dumpPgm(Serial, MultiLang::texts[0][1],16);
  dumpPgm(Serial, MultiLang::texts[1][1],16);
  serialOut<<l00;
  Serial<<endl;
  // Serial<<"texts are at "<<hex((long)pgm_read_ptr(MultiLang::texts))<<endl;
  // for(int n=0;n<sizeof(ops)/sizeof(Item*);n++) {
  //   serialOut<<*ops[n];
  //   Serial<<endl;
  // }
  // Serial<<"change language ----"<<endl;
  // MultiLang::sel=pt;
  // for(int n=0;n<sizeof(ops)/sizeof(Item*);n++) {
  //   serialOut<<*ops[n];
  //   Serial<<endl;
  // }
}

void loop() {}
