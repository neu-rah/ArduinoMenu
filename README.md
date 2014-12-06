ArduinoMenu
===========
AVR generic menu/interactivity system

Easy to define menu system with sub-menus and associated function to call
Works either over serial or with LCD + encoder

-------------------------------------------------------------------------------
V2.0 

main changes:
	- non-blocking menu main cycle
	- Menufields as menu prompts with associated value
		values can be:
			numeric withing range
			list of values toggled on click (for small lists)
			list of values selected as submenu (for longer lists)
	- PCINT now supports Mega/2560 and possibly others

notes:
	- encoder now needs begin() to be called on setup

-------------------------------------------------------------------------------
example of menu definition (c++ macros)

	//a submenu
	MENU(ledMenu,"LED on pin 13",
		OP("LED On",ledOn),
		OP("LED Off",ledOff)
	);

	//field value toggle on click
	TOGGLE(targetVar,trigModes,"Mode: ",
		VALUE("None",trigPoint::None),
		VALUE("On rise",trigPoint::onRise),
		VALUE("On fall",trigPoint::onFall),
		VALUE("Both",trigPoint::isIn)
	);

	//field value, click to browse, click to choose
	CHOOSE(adc_prescale,sample_clock,"Sample clock",
		VALUE("/128",avrADC::clk_128,setADCClk),
		VALUE("/64",avrADC::clk_64,setADCClk),
		VALUE("/32",avrADC::clk_32,setADCClk),
		VALUE("/16",avrADC::clk_16,setADCClk),
		VALUE("/8",avrADC::clk_8,setADCClk),
		VALUE("/4",avrADC::clk_4,setADCClk),
		VALUE("/2",avrADC::clk_2,setADCClk)
	);

	//the main menu...
	//Fields are numeric and show selected value. click to start change, click to fine tune, click to end
	MENU(mainMenu,"Main menu",
		FIELD(frequency,"Freq","Hz",0,16000000,100,1,updateFreq),
		FIELD(dutty,"Duty","%",0,100,1,0,updateDutty),
		OP("Handler test",completeHandlerTest),
		SUBMENU(trigModes),
		SUBMENU(ledMenu)
	);

-------------------------------------------------------------------------------
syntax:

  OP(name,function)
		name string to be shown as menu option prompt
		function to be called on click
	
	FIELD(variable,name,units,min,max,step,tune,function)
		Holding and changing numeric values
		where:
			variable: holding the value (must be numeric or support comparison oprators)
			name: to use as prompt
			units: to be shown after value
			min,max: defining numeric value range
			step: increment/decrement when adjusting value
			tune: value to increment/decrement when fine tunning the value
			function: called on every value change
	
	VALUE(text,value)
		holding possible FIELD values
		where:
			text: to be used as prompt
			value: to be passed when selected
		
	TOGGLE(variable,id,name,
		VALUE(...),
		...,
		VALUE(...)
	)
		Holding a value and a list of possible values to toggle on click
		this is ideal for On/Off Yes/No and other small list of values
		where:
			variable: holding the value
			id: of this element to be used with SUBMENU
			name: to be used as prompt
			
	CHOOSE(variable,id,name,
		VALUE(...),
		...,
		VALUE(...)
	)
		Holding a value and a list of possible values to select as a submenu
		this is ideal for longer lists of values
		where:
			variable: holding the value
			id: of this element to be used with SUBMENU
			name: to be used as prompt

	SUBMENU(id)
		link in a submenu as option of the current one
		where:
			id: the submenu id

	MENU(id,name,
		...
		OP(...),
		FIELD(...),
		SUBMENU(...),
		...
	)
	define menu structure
	where:
		id: this menu id
		
-------------------------------------------------------------------------------
Notes:

	input is read from generic streams, included simple streams for encoders and keyboards
	- provided encoder driver uses internal pull-ups and reverse logic

	output to menuOut devices, included derivations to support serial, GFX and lcd

	multiple stream packing for input to mix encoder stream with encoder keyboard (usually 1 or 2 keys)

-------------------------------------------------------------------------------
more info at http://r-site.net?lang=en&at=//op%5B@id=%273090%27%5D


