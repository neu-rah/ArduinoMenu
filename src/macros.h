/* -*- C++ -*- */
#include "baseMacros.h"

#ifdef ARDUINO_SAM_DUE
  #define MENU_USERAM
#endif
#if defined(pgm_read_ptr_near) && !defined(MENU_USERAM)
  //storing some values into avr flash memory (saving ram space)
  #define USING_PGM
  #define MEMMODE PROGMEM
  #define memPtr(src) pgm_read_ptr(&(src))
  #define memByte(addr) (pgm_read_byte(addr))
  #define memWord(addr) (pgm_read_word(addr))
  #define memIdx(src) (pgm_read_byte(&src))
  #define memStrLen strlen_P
#else
  //use ram on non-avr devices
  #define USING_RAM
  #define MEMMODE
  #define memPtr(src) (src)
  #define memByte(addr) (*(addr))
  #define memWord(addr) (*(addr))
  #define memIdx(src) (src)
  #define memStrLen strlen
#endif

#define DECL(x) DECL_##x
#define DEF(x) DEF_##x,

#define PANELS(id,...)\
  const panel _panels_##id[] MEMMODE={__VA_ARGS__};\
  menuNode* _nodes_##id[sizeof(_panels_##id)/sizeof(panel)];\
  panelsList id(_panels_##id,_nodes_##id,sizeof(_panels_##id)/sizeof(panel));

#define MENU_OUTPUTS(id,...)\
  menuOut* _outputs_##id[]={__VA_ARGS__};\
  outputsList id(_outputs_##id,sizeof(_outputs_##id)/sizeof(menuOut*));

#define MENU_INPUTS(id,...)\
  Stream* _inputs_##id[]={__VA_ARGS__};\
  chainStream<sizeof(_inputs_##id)/sizeof(Stream*)> id(_inputs_##id);

#define MENU(id,text,aFn,mask,style,...) altMENU(menu,id,text,aFn,mask,style,__VA_ARGS__)
#define altMENU(objType,id,text,aFn,mask,style,...)\
  FOR_EACH(DECL,__VA_ARGS__)\
  const char id##_text[] MEMMODE=text;\
  prompt* const id##_data[] MEMMODE={\
    FOR_EACH(DEF,__VA_ARGS__)\
  };\
  const MEMMODE menuNodeShadowRaw id##ShadowRaw={\
    (callback)aFn,\
    (systemStyles)(_menuData|_canNav),\
    id##_text,\
    mask,\
    style,\
    sizeof(id##_data)/sizeof(prompt*),\
    id##_data\
  };\
  const menuNodeShadow& id##Shadow=*(menuNodeShadow*)&id##ShadowRaw;\
  objType id(id##Shadow);

#define SELECT(...) altVARIANT(select,((systemStyles)(_menuData|_canNav|_parentDraw)),__VA_ARGS__)
#define CHOOSE(...) altVARIANT(choose,((systemStyles)(_menuData|_canNav|_isVariant)),__VA_ARGS__)
#define TOGGLE(...) altVARIANT(toggle,_menuData,__VA_ARGS__)
#define altVARIANT(objType,ss,target,id,text,action,mask,style,...)\
  const char id##_text[] MEMMODE=text;\
  XFOR_EACH(DECL_VALUE,target,__VA_ARGS__)\
  prompt* const id##_data[] MEMMODE={\
    FOR_EACH(DEF,__VA_ARGS__)\
  };\
  const MEMMODE menuVariantShadowRaw<typeof(target)> id##ShadowRaw={\
    (callback)action,\
    ss,\
    id##_text,\
    mask,\
    style,\
    sizeof(id##_data)/sizeof(prompt*),\
    id##_data,\
    &target\
  };\
  const MEMMODE menuVariantShadow<typeof(target)>& id##_Shadow=*(menuVariantShadow<typeof(target)>*)&id##ShadowRaw;\
  objType<typeof(target)> id (id##_Shadow);

// bridging macros prepending id's to arguments list
// for all elements that need separate allocation and a name to it
#define OP(...) altOP(prompt,__VA_ARGS__)
#define altOP(...) OP_(__COUNTER__,__VA_ARGS__)
#define EXIT(...) EXIT_(__COUNTER__,__VA_ARGS__)
#define FIELD(...) altFIELD(menuField,__VA_ARGS__)
#define altFIELD(...) FIELD_(__COUNTER__,__VA_ARGS__)
#define VALUE(...) VALUE_(__COUNTER__,__VA_ARGS__)

//allocating space for elements and shadows -------------------------------------
#define DECL_EXIT_(cnt,exitText)\
  const char title_##cnt[] MEMMODE=exitText;\
  const MEMMODE promptShadowRaw opShadowRaw##cnt = {\
    (callback)doExit,\
    _noStyle,\
    title_##cnt,\
    enterEvent\
  };\
  const promptShadow& opShadow##cnt=*(promptShadow*)&opShadowRaw##cnt;\
  prompt op##cnt(opShadow##cnt);
#define DECL_OP_(cnt,objType,text,aFn,mask) \
  const char title_##cnt[] MEMMODE=text;\
  const MEMMODE promptShadowRaw opShadowRaw##cnt={\
    (callback)aFn,\
    _noStyle,\
    title_##cnt,\
    mask,\
    noStyle\
  };\
  const promptShadow& opShadow##cnt=*(promptShadow*)&opShadowRaw##cnt;\
  objType op##cnt(opShadow##cnt);
#define DECL_FIELD_(cnt,objType,target,text,units,low,high,step,tune,action,mask,style)\
  const char fieldLabel##cnt[] MEMMODE=text;\
  const char fieldUnit##cnt[] MEMMODE=units;\
  const MEMMODE menuFieldShadowRaw<typeof(target)> fieldShadowRaw##cnt={\
    (callback)action,\
    _canNav,\
    fieldLabel##cnt,\
    mask,\
    style,\
    &target,\
    fieldUnit##cnt,\
    low,\
    high,\
    step,\
    tune\
  };\
  const menuFieldShadow<typeof(target)>& _fieldShadow##cnt=*(menuFieldShadow<typeof(target)>*)&fieldShadowRaw##cnt;\
  objType<typeof(target)> _menuField##cnt(_fieldShadow##cnt);
#define DECL_TEXTFIELD_(cnt,target,...)\
  menuTextField _menuTextField##cnt(target,__VA_ARGS__);
#define DECL_SUBMENU(id)
#define DECL_VALUE(target,...) MK_VALUE(target, _##__VA_ARGS__)
#define _VALUE_(...)  __VA_ARGS__
#define MK_VALUE(...) _MK_VALUE(__VA_ARGS__)
#define _MK_VALUE(target,cnt,text,value,action,mask)\
  const char valueLabel##cnt[] MEMMODE=text;\
  const MEMMODE menuValueShadowRaw<typeof(target)> choice##cnt##ShadowRaw={\
    (callback)action,\
    _noStyle,\
    valueLabel##cnt,\
    mask,\
    noStyle,\
    value\
  };\
  const menuValueShadow<typeof(target)>& choice##cnt##Shadow=\
    *(menuValueShadow<typeof(target)>*)&choice##cnt##ShadowRaw;\
  menuValue<typeof(target)> menuValue##cnt(choice##cnt##Shadow);

// when building a list of elements --------------------------------------------
#define DEF_EXIT_(cnt,...) &op##cnt
#define DEF_OP_(cnt,...) &op##cnt
#define DEF_FIELD_(cnt,...) &_menuField##cnt
#define DEF_SUBMENU(id) &id
#define DEF_VALUE(id) &id
#define DEF_VALUE_(cnt,...) &menuValue##cnt

//The navigation root ------------------------------------------------------------------
#define NAVROOT(id,menu,maxDepth,in,out)\
  navNode id##_path[maxDepth];\
  navRoot id(menu,id##_path,maxDepth-1,in,out);
