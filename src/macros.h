//#include <avr/pgmspace.h>

#ifdef pgm_read_ptr_near
  //storing some values into avr flash memory (saving ram space)
  #define MEMMODE PROGMEM
  #define pgmPtrNear(addr) pgm_read_ptr(&(addr))
  #define pgmByteNear(addr) (pgm_read_byte(addr))
#else
  //use ram for non-avr devices
  #define MEMMODE
  #define pgmPtrNear(addr) (addr)
  #define pgmByteNear(addr) (*(addr))
#endif

class prompt;
class menu;
class menuOut;
template <typename T> class menuField;

#define CONCATENATE(arg1, arg2)   CONCATENATE1(arg1, arg2)
#define CONCATENATE1(arg1, arg2)  CONCATENATE2(arg1, arg2)
#define CONCATENATE2(arg1, arg2)  arg1##arg2

#define FOR_EACH_1(what, x, ...) what(x)
#define FOR_EACH_2(what, x, ...)\
  what(x)\
  FOR_EACH_1(what,  __VA_ARGS__)
#define FOR_EACH_3(what, x, ...)\
  what(x)\
  FOR_EACH_2(what, __VA_ARGS__)
#define FOR_EACH_4(what, x, ...)\
  what(x)\
  FOR_EACH_3(what,  __VA_ARGS__)
#define FOR_EACH_5(what, x, ...)\
  what(x)\
 FOR_EACH_4(what,  __VA_ARGS__)
#define FOR_EACH_6(what, x, ...)\
  what(x)\
  FOR_EACH_5(what,  __VA_ARGS__)
#define FOR_EACH_7(what, x, ...)\
  what(x)\
  FOR_EACH_6(what,  __VA_ARGS__)
#define FOR_EACH_8(what, x, ...)\
  what(x)\
  FOR_EACH_7(what,  __VA_ARGS__)
#define FOR_EACH_9(what, x, ...)\
  what(x)\
  FOR_EACH_8(what,  __VA_ARGS__)
#define FOR_EACH_10(what, x, ...)\
  what(x)\
  FOR_EACH_9(what,  __VA_ARGS__)
#define FOR_EACH_11(what, x, ...)\
  what(x)\
  FOR_EACH_10(what,  __VA_ARGS__)
#define FOR_EACH_12(what, x, ...)\
  what(x)\
  FOR_EACH_11(what,  __VA_ARGS__)
#define FOR_EACH_13(what, x, ...)\
  what(x)\
  FOR_EACH_12(what,  __VA_ARGS__)
#define FOR_EACH_14(what, x, ...)\
  what(x)\
  FOR_EACH_13(what,  __VA_ARGS__)
#define FOR_EACH_15(what, x, ...)\
  what(x)\
  FOR_EACH_14(what,  __VA_ARGS__)
#define FOR_EACH_16(what, x, ...)\
  what(x)\
  FOR_EACH_15(what,  __VA_ARGS__)

#define XFOR_EACH_1(what, params, x, ...) what(params,x)
#define XFOR_EACH_2(what, params, x, ...)\
  what(params,x)\
  XFOR_EACH_1(what, params,  __VA_ARGS__)
#define XFOR_EACH_3(what, params, x, ...)\
  what(params,x)\
  XFOR_EACH_2(what, params, __VA_ARGS__)
#define XFOR_EACH_4(what, params, x, ...)\
  what(params,x)\
  XFOR_EACH_3(what, params,  __VA_ARGS__)
#define XFOR_EACH_5(what, params, x, ...)\
  what(params,x)\
 XFOR_EACH_4(what, params,  __VA_ARGS__)
#define XFOR_EACH_6(what, params, x, ...)\
  what(params,x)\
  XFOR_EACH_5(what, params,  __VA_ARGS__)
#define XFOR_EACH_7(what, params, x, ...)\
  what(params,x)\
  XFOR_EACH_6(what, params,  __VA_ARGS__)
#define XFOR_EACH_8(what, params, x, ...)\
  what(params,x)\
  XFOR_EACH_7(what, params,  __VA_ARGS__)
#define XFOR_EACH_9(what, params, x, ...)\
  what(params,x)\
  XFOR_EACH_8(what, params,  __VA_ARGS__)
#define XFOR_EACH_10(what, params, x, ...)\
  what(params,x)\
  XFOR_EACH_9(what, params,  __VA_ARGS__)
#define XFOR_EACH_11(what, params, x, ...)\
  what(params,x)\
  XFOR_EACH_10(what, params,  __VA_ARGS__)
#define XFOR_EACH_12(what, params, x, ...)\
  what(params,x)\
  XFOR_EACH_11(what, params,  __VA_ARGS__)
#define XFOR_EACH_13(what, params, x, ...)\
  what(params,x)\
  XFOR_EACH_12(what, params,  __VA_ARGS__)
#define XFOR_EACH_14(what, params, x, ...)\
  what(params,x)\
  XFOR_EACH_13(what, params,  __VA_ARGS__)
#define XFOR_EACH_15(what, params, x, ...)\
  what(params,x)\
  XFOR_EACH_14(what, params,  __VA_ARGS__)
#define XFOR_EACH_16(what, params, x, ...)\
  what(params,x)\
  XFOR_EACH_15(what, params, __VA_ARGS__)

#define FOR_EACH_NARG(...) FOR_EACH_NARG_(__VA_ARGS__, FOR_EACH_RSEQ_N())
#define FOR_EACH_NARG_(...) FOR_EACH_ARG_N(__VA_ARGS__)
#define FOR_EACH_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, N, ...) N
#define FOR_EACH_RSEQ_N() 16,15,14,13,12,11,10,9,8, 7, 6, 5, 4, 3, 2, 1, 0

#define FOR_EACH_(N, what, x, ...) CONCATENATE(FOR_EACH_, N)(what, x, __VA_ARGS__)
#define FOR_EACH(what, x, ...) FOR_EACH_(FOR_EACH_NARG(x, __VA_ARGS__), what, x, __VA_ARGS__)

#define XFOR_EACH_(N, what, params, x, ...) CONCATENATE(XFOR_EACH_, N)(what, params, x, __VA_ARGS__)
#define XFOR_EACH(what, params , x, ...) XFOR_EACH_(FOR_EACH_NARG(x, __VA_ARGS__), what, params, x, __VA_ARGS__)

#define DECL(x) DECL_##x
#define DEF(x) DEF_##x,

#define MENU(id,text,...)\
  FOR_EACH(DECL,__VA_ARGS__)\
  const char id##_text[] MEMMODE=text;\
  prompt* const id##_data[] MEMMODE={\
    FOR_EACH(DEF,__VA_ARGS__)\
  };\
  menu id (id##_text,sizeof(id##_data)/sizeof(prompt*),id##_data);

#define SELECT(target,id,text,...)\
  const char id##_text[] MEMMODE=text;\
  XFOR_EACH(DECL_VALUE,target,__VA_ARGS__)\
  menuValue<typeof(target)>* const id##_data[] MEMMODE={\
    FOR_EACH(DEF,__VA_ARGS__)\
  };\
  menuSelect<typeof(target)> id (id##_text,sizeof(id##_data)/sizeof(prompt*),id##_data,target);

#define CHOOSE(target,id,text,...)\
  const char id##_text[] MEMMODE=text;\
  XFOR_EACH(DECL_VALUE,target,__VA_ARGS__)\
  menuValue<typeof(target)>* const id##_data[] MEMMODE={\
    FOR_EACH(DEF,__VA_ARGS__)\
  };\
  menuChoice<typeof(target)> id (id##_text,sizeof(id##_data)/sizeof(prompt*),id##_data,target);

#define TOGGLE(target,id,text,...)\
  const char id##_text[] MEMMODE=text;\
  XFOR_EACH(DECL_VALUE,target,__VA_ARGS__)\
  menuValue<typeof(target)>* const id##_data[] MEMMODE={\
    FOR_EACH(DEF,__VA_ARGS__)\
  };\
  menuToggle<typeof(target)> id (id##_text,sizeof(id##_data)/sizeof(prompt*),id##_data,target);

/*#define GET_MACRO(_1,_2,NAME,...) NAME
#define VALUE(...) GET_MACRO(__VA_ARGS__, EXPLICIT_VALUE, IMPLICIT_VALUE)(__VA_ARGS__)*/

#define OP(...) OP_(__COUNTER__,__VA_ARGS__)
#define FIELD(...) FIELD_(__COUNTER__,__VA_ARGS__)
#define VALUE(...) VALUE_(__COUNTER__,__VA_ARGS__)
#define TEXTFIELD(...) TEXTFIELD_(__COUNTER__,__VA_ARGS__)

#define DECL_OP_(cnt,text,...) \
  const char title_##cnt[] MEMMODE=text;\
  prompt op##cnt(title_##cnt,__VA_ARGS__);
#define DECL_FIELD_(cnt,target,text,units,...)\
  const char fieldLabel##cnt[] MEMMODE=text;\
  const char fieldUnit##cnt[] MEMMODE=units;\
  menuField<typeof(target)> _menuField##cnt(target,fieldLabel##cnt,fieldUnit##cnt,__VA_ARGS__);
#define DECL_TEXTFIELD_(cnt,target,...) menuTextField _menuTextField##cnt(target,__VA_ARGS__);
#define DECL_SUBMENU(id)
#define DECL_VALUE(target,...) MK_VALUE(target, _##__VA_ARGS__)
#define _VALUE_(...)  __VA_ARGS__
#define MK_VALUE(...) _MK_VALUE(__VA_ARGS__)
#define _MK_VALUE(target,cnt,text,...)\
  const char valueLabel##cnt[] MEMMODE=text;\
  menuValue<typeof(target)> choice##cnt(valueLabel##cnt,__VA_ARGS__);

#define DEF_OP_(cnt,...) &op##cnt
#define DEF_FIELD_(cnt,...) &_menuField##cnt
#define DEF_TEXTFIELD_(cnt,...) &_menuTextField##cnt
#define DEF_SUBMENU(id) &id
#define DEF_VALUE(id) &id
#define DEF_VALUE_(cnt,...) &choice##cnt
