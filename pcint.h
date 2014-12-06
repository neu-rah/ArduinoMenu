/********************
Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
adapted from: http://playground.arduino.cc/Main/PcInt
with many changes to make it compatible with arduino boards (original worked on Uno)
not tested on many boards but has the code uses arduino macros for pin mappings 
it should be more compatible and also more easy to extend
some boards migh need pcintPinMap definition

Sept. 2014
  small changes to existing PCINT library, supporting an optional cargo parameter

Nov.2014 large changes
  - Use arduino standard macros for PCINT mapping instead of specific map math, broaden compatibility
  - array[a][b] is 17% faster than array[(a<<3)+b], same memory
  - reverse pin mappings for pin change check (not on arduino env. AFAIK)

**/	
#ifndef ARDUINO_PCINT_MANAGER
#define ARDUINO_PCINT_MANAGER
	#if ARDUINO < 100
	#include <WProgram.h>
	#else
	#include <Arduino.h>
	#endif
	#include "pins_arduino.h"

	// PCINT reverse map
	// because some avr's (like 2560) have a messed map we got to have this detailed pin reverse map
	// still this makes the PCINT automatization very slow, risking interrupt collision
	#if ( defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega16u4__) )
		const uint8_t PROGMEM pcintPinMap[3][8]={{8,9,10,11,12,13,-1,-1},{A0,A1,A2,A3,A4,A5,-1,-1},{0,1,2,3,4,5,6,7}};
	#elif ( defined(__AVR_ATmega2560__) )
		const uint8_t PROGMEM pcintPinMap[3][8]={{53,52,51,50,10,11,12,13},{0,15,14,-1,-1,-1,-1,-1},{A8,A9,A10,A11,A12,A13,A14,A15}};
	#else
		#error "uC PCINT REVERSE MAP IS NOT DEFINED"
	#endif
	
	#define digitalPinFromPCINTSlot(slot,bit) pgm_read_byte(pcintPinMap+(((slot)<<3)+(bit)))
	#define pcintPinMapBank(slot) ((uint8_t*)((uint8_t*)pcintPinMap+((slot)<<3)))
	#define digitalPinFromPCINTBank(bank,bit) pgm_read_byte((uint8_t*)bank+bit)

	//this handler can be used instead of any void(*)() and optionally it can have an associated void *
	//and use it to call void(*)(void* payload)
	struct mixHandler {
		union {
			void (*voidFunc)(void);
			void (*payloadFunc)(void*);
		} handler;
		void *payload;
		inline mixHandler():payload(NULL) {handler.voidFunc=NULL;}
		inline mixHandler(void (*f)(void)):payload(NULL) {handler.voidFunc=f;}
		inline mixHandler(void (*f)(void*),void *payload):payload(payload) {handler.payloadFunc=f;}
		inline void operator()() {payload?handler.payloadFunc(payload):handler.voidFunc();}
		inline bool operator==(void*ptr) {return handler.voidFunc==ptr;}
		inline bool operator!=(void*ptr) {return handler.voidFunc!=ptr;}
	};

	typedef void (*voidFuncPtr)(void);

	#define HANDLER_TYPE mixHandler

	static int PCintMode[3][8];
	static HANDLER_TYPE PCintFunc[3][8];
	static bool PCintLast[3][8];//?we can use only 3 bytes... but will use more processing power calculating masks

	/*
	 * attach an interrupt to a specific pin using pin change interrupts.
	 */
	void PCattachInterrupt(uint8_t pin, class mixHandler userFunc, uint8_t mode) {
		volatile uint8_t *pcmask=digitalPinToPCMSK(pin);
		if (!pcmask) return;//runtime checking if pin has PCINT, i would prefer a compile time check
		uint8_t bit = digitalPinToPCMSKbit(pin);
		uint8_t mask = 1<<bit;
		uint8_t pcicrBit=digitalPinToPCICRbit(pin);
		PCintMode[pcicrBit][bit] = mode;
		PCintFunc[pcicrBit][bit] = userFunc;
		//initialize last status flags
		PCintLast[pcicrBit][bit]=(*portInputRegister(digitalPinToPort(pin)))&digitalPinToBitMask(pin);
		// set the mask
		*pcmask |= mask;
		// enable the interrupt
		PCICR |= (1<<pcicrBit);
	}

	void PCdetachInterrupt(uint8_t pin) {
		volatile uint8_t *pcmask=digitalPinToPCMSK(pin);
		if (!pcmask) return;//runtime checking if pin has PCINT, i would prefer a compile time check
		// disable the mask.
		*pcmask &= ~(1<<digitalPinToPCMSKbit(pin));
		// if that's the last one, disable the interrupt.
		if (*pcmask == 0)
		  PCICR &= ~(1<<digitalPinToPCICRbit(pin));
	}

	// common code for isr handler. "port" is the PCINT number.
	// there isn't really a good way to back-map ports and masks to pins.
	// here we consider only the first change found ignoring subsequent, assuming no interrupt cascade
	static void PCint(uint8_t port) {
		const uint8_t* map=pcintPinMapBank(port);//get 8 bits pin change map
		for(int i=0;i<8;i++) {
    	uint8_t p=digitalPinFromPCINTBank(map,i);
			if (p==-1)
			  continue;//its not assigned
			//uint8_t bit = digitalPinToPCMSKbit(p);
			//uint8_t mask = (1<<bit);
			if (PCintFunc[port][i]!=NULL) {//only check active pins
				bool stat=(*portInputRegister(digitalPinToPort(p)))&digitalPinToBitMask(p);
				if (PCintLast[port][i]^stat) {//pin changed!
				  PCintLast[port][i]=stat;//register change
					if (
            (PCintMode[port][i]==CHANGE)
            || ((stat)&&(PCintMode[port][i]==RISING))
            || ((!stat)&&(PCintMode[port][i]==FALLING))
		      ) {
	 				  PCintFunc[port][i]();
						break;//if using concurrent interrupts remove this
		      }
				}
			}
		}
	}

	//AVR handle pin change... later figure it out the pin
	SIGNAL(PCINT0_vect) {
		PCint(0);
	}
	SIGNAL(PCINT1_vect) {
		PCint(1);
	}
	SIGNAL(PCINT2_vect) {
		PCint(2);
	}

#endif

