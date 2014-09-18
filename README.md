ArduinoMenu
===========

AVR generic menu/interactivity system

Easy to define menu system with sub-menus and associated function to call
Works either over serial or with LCD + encoder

example of menu definition (c++ macros)

		MENU(subMenu,"LED on pin 13",
			OP("LED On",ledOn),
			OP("LED Off",ledOff)
		);

		MENU(mainMenu,"Sistema",
			OP("Frequency",setFreq),
			OP("Dutty",setDutty),
			OP("Handler test",completeHandlerTest),
			SUBMENU(subMenu)
		);

input is read from generic streams, included simple streams for encoders and keyboards

output to menuOut devices, included derivations to support serial, GFX and lcd

multiple stream packing for input allowing mixing encoder stream with encoder keyboard (usually 1 or 2 keys)

more info at http://r-site.net?lang=en&at=//op%5B@id=%273090%27%5D
