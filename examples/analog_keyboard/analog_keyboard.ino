 // this example will just show how to use analog keyboard with the menu
 // depend on https://github.com/neu-rah/ArduinoMenu/issues/17
 // the used keyboard is: https://www.dfrobot.com/wiki/index.php/Arduino_LCD_KeyPad_Shield_(SKU:_DFR0009)
 // you have to change your analog data according to your keyboadr
 
#include <menu.h>//menu macros and objects
#include <genericKeyboard.h>
#include <chainStream.h>
#include <LiquidCrystal.h>
#include <menuLCD.h>





#define btnRIGHT  menu::enterCode
#define btnUP     menu::downCode
#define btnDOWN   menu::upCode
#define btnLEFT   menu::escCode
#define btnENTER  menu::enterCode
#define btnNONE   -1

genericKeyboard mykb(read_keyboard);
//alternative to previous but now we can input from Serial too...
//Stream* in2[]={&mykb,&Serial};
//chainStream<2> allIn(in2);
Stream* in2[] = { &mykb };
chainStream<1> allIn(in2);

//set the pin and the lcd
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
menuLCD menu_lcd(lcd, 16, 2);//menu output device

//this is a simple menu just to test the keyboard check othe eample to extend it.

MENU(mainMenu, "Main menu",
	OP("LED On", menu::nothing),
	OP("LED Off", menu::nothing),
	OP("Empty 1", menu::nothing),
	OP("Empty 2", menu::nothing),
	OP("Empty 3", menu::nothing),
	OP("Empty 4", menu::nothing),
	OP("Empty 5...", menu::nothing)
);



int old_button = 0;

int read_keyboard() {
	int button, button2, pressed_button;
	button = getButton();
	if (button != old_button) {
		delay(50);        // debounce
		button2 = getButton();

		if (button == button2) {
			old_button = button;
			pressed_button = button;
			if (button != 0) {
				//Serial.println(button); 
				if (button == 1) return btnLEFT;
				if (button == 2) return btnUP;
				if (button == 3) return btnDOWN;
				if (button == 4) return btnRIGHT;
				if (button == 5) return btnENTER;
			}
		}
	}
	else {
		return btnNONE;
	}
}

int getButton() {
	int i, z, button;
	int sum = 0;

	for (i = 0; i < 4; i++) {
		sum += analogRead(0);
	}
	z = sum / 4;
  // change this numbers according to your keyboard.
	if (z > 1000) button = 0;
	else if (z >= 0 && z < 50) button = 1; //LEFT                    
	else if (z > 120 && z < 155) button = 2; //UP                
	else if (z > 292 && z < 335) button = 3; //DOWN                
	else if (z > 460 && z < 515) button = 4; //RIGHT           
	else if (z > 650 && z < 745) button = 5; //ENTER
	else button = 0;
	//{ 0, 128, 302, 474, 718 }
	return button;
}


void setup() {

}

void loop() {

	mainMenu.poll(menu_lcd, allIn);
	delay(50);

}
