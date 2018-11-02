/* -*- C++ -*- */
/********************
Sept. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
Oct. 2018 - modified for pin interrupts   Kevin O'Rourke

quadrature encoder driver (PCINT) 
quadrature encoder stream (fake, not using buffers)

*/

#ifndef RSITE_ARDUINO_MENU_ENCODER
  #define RSITE_ARDUINO_MENU_ENCODER

  #include <pcint.h> //https://github.com/neu-rah/PCINT
  #include "../menuDefs.h"
	
  namespace Menu {

    template<uint8_t pinUP,uint8_t pinDOWN,uint8_t pinSEL, uint8_t pinLEFT, uint8_t pinRIGHT>
    class encoderIn {
    public:
      volatile int pos = 0;
      volatile bool menu_running = false;
      volatile bool menu_interrupt = false;
      volatile unsigned long last_interrupt_time = 0;
      void begin() {
      	pinMode(pinUP, INPUT_PULLUP);
	pinMode(pinDOWN, INPUT_PULLUP);
	pinMode(pinSEL, INPUT_PULLUP);
	pinMode(pinLEFT, INPUT_PULLUP);
	pinMode(pinRIGHT, INPUT_PULLUP);
	PCattachInterrupt<pinUP>(mixHandler((void(*)(void*))encoderInUpdateA,this), LOW);
	PCattachInterrupt<pinDOWN>(mixHandler((void(*)(void*))encoderInUpdateB,this), LOW);
	PCattachInterrupt<pinSEL>(mixHandler((void(*)(void*))encoderInUpdateC,this), LOW);
	PCattachInterrupt<pinLEFT>(mixHandler((void(*)(void*))encoderInUpdateD,this), LOW);
	PCattachInterrupt<pinRIGHT>(mixHandler((void(*)(void*))encoderInUpdateE,this), LOW);
      }
      static void encoderInUpdateA(class encoderIn<pinUP,pinDOWN,pinSEL,pinLEFT,pinRIGHT> *e);
      static void encoderInUpdateB(class encoderIn<pinUP,pinDOWN,pinSEL,pinLEFT,pinRIGHT> *e);
      static void encoderInUpdateC(class encoderIn<pinUP,pinDOWN,pinSEL,pinLEFT,pinRIGHT> *e);
      static void encoderInUpdateD(class encoderIn<pinUP,pinDOWN,pinSEL,pinLEFT,pinRIGHT> *e);
      static void encoderInUpdateE(class encoderIn<pinUP,pinDOWN,pinSEL,pinLEFT,pinRIGHT> *e);
    };	
	
    //PCint handlers
	
    template<uint8_t pinUP,uint8_t pinDOWN,uint8_t pinSEL, uint8_t pinLEFT, uint8_t pinRIGHT>
    void encoderIn<pinUP,pinDOWN,pinSEL,pinLEFT,pinRIGHT>::encoderInUpdateA(class encoderIn<pinUP,pinDOWN,pinSEL,pinLEFT,pinRIGHT> *e) {
       unsigned long interrupt_time = millis();
       if (interrupt_time - e->last_interrupt_time > 20UL){
		  if(e->menu_running){ 
			e->menu_interrupt = true;
			e->pos++;
			}
	    }
		e->last_interrupt_time = interrupt_time;
    }
	
    template<uint8_t pinUP,uint8_t pinDOWN,uint8_t pinSEL, uint8_t pinLEFT, uint8_t pinRIGHT>
    void encoderIn<pinUP,pinDOWN,pinSEL,pinLEFT,pinRIGHT>::encoderInUpdateB(class encoderIn<pinUP,pinDOWN,pinSEL,pinLEFT,pinRIGHT> *e) {
       unsigned long interrupt_time = millis();
       if (interrupt_time - e->last_interrupt_time > 20UL){
		 if(e->menu_running){
			e->menu_interrupt = true; 
			e->pos--;
			}
	    }
		e->last_interrupt_time = interrupt_time;
    }
	
    template<uint8_t pinUP,uint8_t pinDOWN,uint8_t pinSEL, uint8_t pinLEFT, uint8_t pinRIGHT>
    void encoderIn<pinUP,pinDOWN,pinSEL,pinLEFT,pinRIGHT>::encoderInUpdateC(class encoderIn<pinUP,pinDOWN,pinSEL,pinLEFT,pinRIGHT> *e) {
       unsigned long interrupt_time = millis();  
	   if (interrupt_time - e->last_interrupt_time > 20UL){
			if(e->menu_running){
				e->pos+=2;
			}else if(!e->menu_running) {
				e->menu_running = true;//lock menu if false
				}
			e->menu_interrupt = true;				
	    }
		e->last_interrupt_time = interrupt_time;
    }
	
    template<uint8_t pinUP,uint8_t pinDOWN,uint8_t pinSEL, uint8_t pinLEFT, uint8_t pinRIGHT>
    void encoderIn<pinUP,pinDOWN,pinSEL,pinLEFT,pinRIGHT>::encoderInUpdateD(class encoderIn<pinUP,pinDOWN,pinSEL,pinLEFT,pinRIGHT> *e) {
       unsigned long interrupt_time = millis();
       if (interrupt_time - e->last_interrupt_time > 20UL){
		 if(e->menu_running){ 
			e->menu_interrupt = true;
			e->pos+=3;
			}
	    }
		e->last_interrupt_time = interrupt_time;  
    }
	
    template<uint8_t pinUP,uint8_t pinDOWN,uint8_t pinSEL, uint8_t pinLEFT, uint8_t pinRIGHT>
    void encoderIn<pinUP,pinDOWN,pinSEL,pinLEFT,pinRIGHT>::encoderInUpdateE(class encoderIn<pinUP,pinDOWN,pinSEL,pinLEFT,pinRIGHT> *e) {
       unsigned long interrupt_time = millis();
       if (interrupt_time - e->last_interrupt_time > 20UL){
		  if(e->menu_running){
			e->menu_interrupt = true;
		    e->pos-=3;
		    }
	    }
		e->last_interrupt_time = interrupt_time;  
    }

    //emulate a stream based on encoderIn movement returning +/- for every step
    //buffer not needer because we have an accumulator
    template<uint8_t pinUP,uint8_t pinDOWN,uint8_t pinSEL, uint8_t pinLEFT, uint8_t pinRIGHT>
    class encoderInStream:public menuIn {
    public:
      encoderIn<pinUP,pinDOWN,pinSEL,pinLEFT,pinRIGHT> &enc;//associated hardware encoderIn
      encoderInStream(encoderIn<pinUP,pinDOWN,pinSEL,pinLEFT,pinRIGHT> &enc):enc(enc) {}
      int oldPos = 0;
      int available(void) {return abs(enc.pos-oldPos);}
      int peek(void) override {
        int d = enc.pos-oldPos;
        if (d==-1)return options->navCodes[downCmd].ch;
        if (d==1) return options->navCodes[upCmd].ch;
	if (d>=2) {
		if (d>=3){
			return options->navCodes[leftCmd].ch;
		}else{
			return options->navCodes[enterCmd].ch;
			}
        }
	if (d<=-3){
		return options->navCodes[rightCmd].ch;
		}		
        return -1;
	}
      int read() override {	
        int d=enc.pos-oldPos;
        if (d==-1) {
          oldPos-=1;
          return options->navCodes[downCmd].ch;
        }
        if (d==1) {
          oldPos+=1;
          return options->navCodes[upCmd].ch;
        }		
	if (d>=2) {
		if (d>=3){
			oldPos+=3;
			return options->navCodes[leftCmd].ch;
		}else{
			oldPos+=2;
			return options->navCodes[enterCmd].ch;
			}
        }
	if (d<=-3){
		oldPos-=3;
		return options->navCodes[rightCmd].ch;
		}
        return -1;
	}
      void flush() {
		  oldPos = enc.pos;
		}
      size_t write(uint8_t v) {
		oldPos = v;
		return 1;
		}
    };
  }//namespace Menu
#endif
//void doNav(navCmd cmd);//fly by wire mode
