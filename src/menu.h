/********************
Sept 2014 ~ Sept. 2016 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

menu library objects
definitions and enumerations

www.r-site.net
***/

#ifndef RSITE_ARDUINO_MENU_SYSTEM
	#define RSITE_ARDUINO_MENU_SYSTEM
	#include <Stream.h>
	#include "menuBase.h"

	namespace Menu {

		// Menu objects and data
		//////////////////////////////////////////////////////////////////////////
		struct promptShadowRaw {
			actionRaw a;//the hooked callback function
			systemStyles sysStyles;
			const char*text;
			const eventMask events;//registered events
		};
		class promptShadow:public action {
			public:
				systemStyles sysStyles=_noStyle;
				const char*text;
				const eventMask events=noEvent;//registered events
				promptShadow(const char* t,action a=doNothing,eventMask e=noEvent):action(a),text(t),events(e) {}
		};
		class prompt {
			public:
				status enabled=enabledStatus;//ignore enter if false
				bool dirty=true;//needs to be	redrawn
				const promptShadow* shadow;//constant read-only data (PROGMEM)
				inline prompt(const promptShadow& shadow):shadow(&shadow) {}
				inline systemStyles sysStyles() const {return (systemStyles)memWord(&shadow->sysStyles);}
				inline bool canNav() const {return sysStyles()&_canNav;}//can receive navigation focus and process keys
				inline bool isMenu() const {return sysStyles()&_menuData;}//has menu data list and can be a navNode target
				inline bool isVariant() const {return sysStyles()&_isVariant;}//a menu as an enumerated field, connected to a variable value
				virtual void printTo(idx_t i,navNode &nav,menuOut& out);//raw print to output device
				virtual bool changed(const navNode &nav,const menuOut& out) {return dirty;}
				//this is the system version of enter handler, its used by elements like toggle
				virtual result sysHandler(FUNC_PARAMS) {return proceed;}
				inline result operator()(FUNC_PARAMS) const {return (*shadow)(FUNC_VALUES);}
		};

		//--------------------------------------------------------------------------
		// can receive navigation focus and prrocess keys
		class navTarget:public prompt {
			public:
				navTarget(const promptShadow& shadow):prompt(shadow) {}
				//bool canNav() const override {return true;}
				virtual void navigate(navNode& nav,char ch,Stream& in);
		};

		//--------------------------------------------------------------------------
		struct menuNodeShadowRaw {
			actionRaw a;
			systemStyles sysStyles;
			const char*text;
			const eventMask events;//registered events
			idx_t sz;
			prompt* const* data;
			styles style;
			//int width;//field or menu width
			//int ox,oy;//coordinate origin displacement
		};
		class menuNodeShadow:public promptShadow {
			public:
				idx_t sz;
				prompt* const* data;
				styles style;
				//int width;//field or menu width
				//int ox,oy;//coordinate origin displacement
				menuNodeShadow(const char* text,idx_t sz,prompt* const* data,action a,eventMask e,styles style)
				:promptShadow(text,a,e),sz(sz),data(data),style(style) {}
		};
		class menuNode:public navTarget {
			public:
				menuNode(const menuNodeShadow& s):navTarget(s) {}
				inline prompt& operator[](idx_t i) const {
					return *(prompt*)memPtr(((prompt**)memPtr(((menuNodeShadow*)shadow)->data))[i]);
				}
				bool changed(const navNode &nav,const menuOut& out) override;
				inline idx_t sz() const {return memIdx(((menuNodeShadow*)shadow)->sz);}
				inline prompt** data() const {return (prompt**)memPtr(((menuNodeShadow*)shadow)->data);}
		};

		//--------------------------------------------------------------------------
		class menu:public menuNode {
			public:
				menu(const menuNodeShadow& shadow):menuNode(shadow) {}
		};

		//--------------------------------------------------------------------------
		template<typename T>
		struct menuFieldShadowRaw {
			actionRaw a;
			systemStyles sysStyles;
			const char*text;
			const eventMask events;//registered events
			T* value;
			const char* units;
			const T low,high,step,tune;
		};
		template<typename T>
		class menuFieldShadow:public promptShadow {
			public:
				T* value;
				const char* units;
				const T low,high,step,tune;
				menuFieldShadow(T &value,const char * text,const char *units,T low,T high,T step,T tune,action a=doNothing,eventMask e=noEvent)
					:value(&value),units(units),low(low),high(high),step(step),tune(tune),promptShadow(text,a,e) {}
		};
		template<typename T>
		class menuField:public navTarget {
			public:
				bool tunning=false;
				T reflex;
				menuField(const menuFieldShadow<T> & shadow):navTarget(shadow) {}
				void navigate(navNode& nav,char ch,Stream& in) override;
				void printTo(idx_t i,navNode &nav,menuOut& out) override;
				inline T& target() const {return *(T*)memPtr(((menuFieldShadow<T>*)shadow)->value);}
				inline T getTypeValue(const T* from) const {
					#ifdef USING_PGM
						T tmp;
						memcpy_P(&tmp, from, sizeof(T));
						return tmp;
					#else
						return *from;
					#endif
				}
				inline T low() const {return getTypeValue(&((menuFieldShadow<T>*)shadow)->low);}
				inline T high() const {return getTypeValue(&((menuFieldShadow<T>*)shadow)->high);}
				inline T step() const {return getTypeValue(&((menuFieldShadow<T>*)shadow)->step);}
				inline T tune() const {return	getTypeValue(&((menuFieldShadow<T>*)shadow)->tune);}
				bool changed(const navNode &nav,const menuOut& out) override {
					return dirty||(reflex!=target());
				}
				void clamp() {
					if (target()<low()) target()=low();
					else if (target()>high()) target()=high();
				}
		};

		//--------------------------------------------------------------------------
		template<typename T>
		struct menuValueShadowRaw {
			actionRaw a;
			systemStyles sysStyles;
			const char*text;
			const eventMask events;//registered events
			T value;
		};
		template<typename T>
		class menuValueShadow:public promptShadow {
			public:
				T value;
				inline menuValueShadow(const char * text,T value,action a=doNothing,eventMask e=noEvent)
					:promptShadow(text,a,e),value(value) {}
		};
		template<typename T>
		class menuValue:public prompt {
			public:
				menuValue(const menuValueShadow<T>& shadow):prompt(shadow) {}
				#ifdef DEBUG
				bool changed(const navNode &nav,const menuOut& out) override {
					/*Serial<<"Value changed? TODO: remove call to this and this functions!"<<endl;
					assert(false);*/
					return false;
				}
				#endif
				inline T getTypeValue(const T* from) const {
					#ifdef USING_PGM
						T tmp;
						memcpy_P(&tmp, from, sizeof(T));
						return tmp;
					#else
						return *from;
					#endif
				}
				inline T target() const {return getTypeValue(&((menuValueShadow<T>*)shadow)->value);}
		};

		//--------------------------------------------------------------------------
		template<typename T>
		struct menuVariantShadowRaw {
			actionRaw a;
			systemStyles sysStyles;
			const char*text;
			const eventMask events;//registered events
			idx_t sz;
			prompt* const* data;
			styles style;
			//int width;//field or menu width
			//int ox,oy;//coordinate origin displacement
			T* value;
		};
		template<typename T>
		class menuVariantShadow:public menuNodeShadow {
			public:
				T* value;
				menuVariantShadow(const char* text,T &target,idx_t sz,prompt* const* data,action a,eventMask e,styles style)
				:menuNodeShadow(text,sz,data,a,e,style),value(&target) {}
		};
		template<typename T>//-------------------------------------------
		class menuVariant:public menuNode {
			public:
				T reflex;
				menuVariant(const menuNodeShadow& s):menuNode(s) {}
				idx_t sync() {
					menuVariantShadow<T>& s=*(menuVariantShadow<T>*)shadow;
					for(idx_t i=0;i<sz();i++) {
						//Serial<<"checking "<<i<<" "<<target()<<"=="<<((menuValue<T>*)&operator[](i))->target()<<endl;
						if (((menuValue<T>*)&operator[](i))->target()==target()) return i;
					}
					#ifdef DEBUG
					Serial<<"value out of range "<<target()<<endl;Serial.flush();
					assert(false);
					#endif
					return -1;
				}
				idx_t sync(idx_t i) {
					menuVariantShadow<T>& s=*(menuVariantShadow<T>*)shadow;
					#ifdef DEBUG
					if (!(i>=0&&i<sz())){
						Serial<<*(prompt*)this<<" : value out of range "<<i<<endl;
					}
					assert(i>=0&&i<sz());
					#endif
					target()=reflex=((menuValue<T>*)&operator[](i))->target();
					//Serial<<"Set toggle value to :"<<s.value<<endl;
					return i;
				}
				inline T& target() const {return *(T*)memPtr(((menuVariantShadow<T>*)shadow)->value);}
				bool changed(const navNode &nav,const menuOut& out) override {
					//if (dirty) Serial<<"menuVariant::shadow "<<target()<<"/"<<reflex<<endl;
					return dirty||reflex!=target();
				}
				void navigate(navNode& nav,char ch,Stream& in) override;
				void printTo(idx_t i,navNode &nav,menuOut& out) override;
		};

		template<typename T>//-------------------------------------------
		class select:public menuVariant<T> {
			public:
				select(const menuNodeShadow& s):menuVariant<T>(s) {}
		};

		template<typename T>//-------------------------------------------
		class toggle:public menuVariant<T> {
			public:
				toggle(const menuNodeShadow& s):menuVariant<T>(s) {}
				void printTo(idx_t i,navNode &nav,menuOut& out) override;
				//bool canNav() const override {return false;}//can receive navigation focus and process keys
				result sysHandler(FUNC_PARAMS) override {
					switch(event) {
						case activateEvent: {
							menuNodeShadow& s=*(menuNodeShadow*)prompt::shadow;
							idx_t at=menuVariant<T>::sync();
							assert(at!=-1);
							at++;
							if (at>=menuNode::sz()) at=0;
							menuVariant<T>::sync(at);
							prompt::dirty=true;
							(*menuNode::operator[](at).shadow)(FUNC_VALUES);
						}
						default:
							return proceed;
					}
					//return proceed;
				}
		};

		template<typename T>//-------------------------------------------
		class choose:public menuVariant<T> {
			public:
				choose(const menuNodeShadow& s):menuVariant<T>(s) {}
				result sysHandler(FUNC_PARAMS) override;
				bool changed(const navNode &nav,const menuOut& out) override {
					//Serial<<"choose::changed "<<menuVariant<T>::reflex<<"=="<<menuVariant<T>::target()<<endl;Serial.flush();
					return menuVariant<T>::changed(nav,out)||menuNode::changed(nav,out);
					//return false;
				}
		};

		// Output
		////////////////////////////////////////////////////////////////////////////

		//navigation panels (min 1)
		//this is a thing of output devices, some might have it
		struct panel {unsigned char x,y,w,h;};

		class menuOut:public Print {
			public:
				//device size (in characters)
				idx_t maxX=80;
				idx_t maxY=24;
				idx_t lastTop=-1;
				idx_t lastSel=-1;
				idx_t top=0;//first line to draw
				bool redraw=false;
				menuNode* drawn;
				menuOut() {}
				menuOut(idx_t x,idx_t y,bool r=false):maxX(x),maxY(y),redraw(r) {}
				virtual menuOut& operator<<(prompt const &p);
				virtual menuOut& fill(
					int x1, int y1, int x2, int y2,char ch=' ',
					colorDefs color=bgColor,
					bool selected=false,
					status stat=enabledStatus,
					bool edit=false)
					{return *this;}
				virtual void clearLine(idx_t ln,colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false)=0;
				virtual void clear()=0;
				virtual void setCursor(idx_t x,idx_t y)=0;
				virtual void printMenu(navNode &nav);
				virtual void clearChanged(navNode &nav);
				virtual void setColor(colorDefs c,bool selected=false,status s=enabledStatus,bool edit=false) {}
				virtual void drawCursor(idx_t ln,bool selected,status stat,bool edit=false) {
					setColor(cursorColor, selected, stat,edit);
					write(selected?(stat==disabledStatus?options.disabledCursor:options.selectedCursor):' ');
				}
				void alert(char *msg) {
					clear();
					*this<<msg<<endl;
				}
		};

		class gfxOut:public menuOut {
			public:
				idx_t resX=1;
				idx_t resY=1;
				idx_t posX=0;
				idx_t posY=0;
				gfxOut(idx_t x,idx_t y,idx_t rx,idx_t ry,idx_t px=0,idx_t py=0,bool r=true)
					:menuOut(x,y,r),resX(rx),resY(ry),posX(px),posY(py) {}
				//void printMenu(navNode &nav) override;
		};

		//list of output devices
		//this allows parallel navigation on multiple devices
		class outputsList {
			public:
				int cnt=1;
				menuOut** outs;
				outputsList(menuOut* o[],int n):cnt(n),outs(o) {}
				menuOut& operator[](idx_t i) {
					assert(i<cnt);
					return *outs[i];
				}
				void printMenu(navNode& nav) const {
					for(int n=0;n<cnt;n++)
						outs[n]->printMenu(nav);
					for(int n=0;n<cnt;n++)
						outs[n]->clearChanged(nav);
				}
				void alert(char *msg) const {for(int n=0;n<cnt;n++) outs[n]->alert(msg);}
				void clearLine(idx_t ln,colorDefs color=bgColor,bool selected=false,status stat=enabledStatus) const {
					for(int n=0;n<cnt;n++) outs[n]->clearLine(ln,color,selected,stat);
				}
				void clearChanged(navNode &nav) const {
					for(int n=0;n<cnt;n++) outs[n]->clearChanged(nav);
				}
				void clear() {for(int n=0;n<cnt;n++) outs[n]->clear();}
				void setCursor(idx_t x,idx_t y) {
					for(int n=0;n<cnt;n++) outs[n]->setCursor(x,y);
				}
				void setColor(colorDefs c,bool selected=false,status s=enabledStatus) {
					for(int n=0;n<cnt;n++) outs[n]->setColor(c,selected,s);
				}
				void drawCursor(idx_t ln,bool selected,status stat) {
					for(int n=0;n<cnt;n++) outs[n]->drawCursor(ln,selected,stat);
				}
				result idle(idleFunc f,idleEvent e) {
					for(int n=0;n<cnt;n++) {
						menuOut& o=*outs[n];
						switch(e) {
							case idleStart:
								(*f)(o,e);
								if (!o.redraw) (*f)(o,e);
								break;
							case idling:
								if (o.redraw) (*f)(o,e);
								break;
							case idleEnd:
								(*f)(o,e);
								break;
						}
					}
				}
		};

		// Navigation
		//////////////////////////////////////////////////////////////////////////
		class navNode {
			public:
				idx_t sel=0;
				//idx_t top=0;//TODO: send this to menuOut
				menuNode* target;
				static navRoot* root;
				inline menuNodeShadow& shadow() const {return *(menuNodeShadow*)target->shadow;}
				inline idx_t sz() const {return memIdx(shadow().sz);}
				inline prompt* const * data() const {return shadow().data;}
				inline prompt& selected() const {return *(prompt*)memPtr(data()[sel]);}
				inline bool wrap() const {return memWord(shadow().style)&wrapStyle;}
				/*inline result sysHandler(eventMask event, prompt &item, Stream &in, menuOut &out) const {
					return target->sysHandler(event,*this,item,in,out);
				}*/
				inline result event(eventMask e,idx_t i);//send event to item index i
				inline result event(eventMask e) {return event(e,sel);}//send event to current item
				inline result sysEvent(eventMask e,idx_t i);//send event to item index i
				inline result sysEvent(eventMask e) {return sysEvent(e,sel);}//send event to current item
				navCmds navKeys(char ch);
				void doNavigation(char ch,Stream& in);
				inline bool changed(const menuOut& out) const {return target->changed(*this,out);}
				inline prompt& operator[](idx_t i) const {return target->operator[](i);}
		};

		class navRoot {
			public:
				outputsList &out;
				Stream& in;
				navNode* path;
				idx_t maxDepth=0;
				idx_t level=0;
				bool suspended=false;
				bool showTitle=true;
				//bool sleeping=false;//when sleeping poll will simply return
				idleFunc sleepTask=NULL;
				navTarget* navFocus=NULL;
				navRoot(menuNode& root,navNode* path,idx_t d,Stream& in,outputsList &o)
					:out(o),in(in),path(path),maxDepth(d) {
						navFocus=&root;
						path[0].target=&root;
						navNode::root=this;
						//for(int i=0;i<d;i++) path[i].root=this;
					}
				inline navNode& node() const {return path[level];}
				inline menuNode& active() const {return *node().target;}
				inline prompt& selected() const {return active()[node().sel];}
				void printMenu() const {
					if ((active().sysStyles()&_parentDraw)&&level)
						out.printMenu(path[level-1]);
					else out.printMenu(node());
				}
				//inline bool changed() const {return node().changed(out);}
				void poll();
				void enter();
				void exit();
				inline void idleOn(idleFunc task=inaction) {
					out.clear();
					sleepTask=task;
					active().dirty=true;
					out.idle(sleepTask,idleStart);
				}
				inline void idleOff() {
					out.idle(sleepTask,idleEnd);
					sleepTask=NULL;
					active().dirty=true;
					out.clear();
				}
				inline void alert(char *msg,bool modal=true) {out.alert(msg);}
		};

		////////////////////////////////////////////////////////////////////////
		template<typename T>
		void menuField<T>::printTo(idx_t i,navNode &nav,menuOut& out) {
			menuFieldShadow<T>& s=*(menuFieldShadow<T>*)shadow;
			reflex=target();
			prompt::printTo(i,nav,out);
			bool ed=this==nav.root->navFocus;
			bool sel=nav.sel==i;
			out<<(sel?(tunning?">":":"):" ");
			out.setColor(valColor,sel,enabled,ed);
			out<<reflex;
			out.setColor(unitColor,sel,enabled,ed);
			print_P(out,(const char*)memPtr(s.units));
		}

		static const char* numericChars="0123456789.";

		template<typename T>
		void menuField<T>::navigate(navNode& nav,char ch,Stream& in) {
			//Serial<<"menuField<T>::navigate"<<endl;
			menuFieldShadow<T>& s=*(menuFieldShadow<T>*)shadow;
			if (strchr(numericChars,in.peek())) {//a numeric value was entered
				target()=(T)in.parseFloat();
				//Serial<<"new parsed value:"<<target()<<endl;
				tunning=false;
				nav.root->exit();
			} else {
				//char ch=in.read();
				navCmds cmd=nav.navKeys(ch);
				switch(cmd) {
					case enterCmd:
						//Serial<<"enterCmd"<<endl;
						if (tunning||options.nav2D||!tune()) {//then exit edition
							tunning=false;
							dirty=true;
							nav.root->exit();
						} else tunning=true;
						dirty=true;
						break;
					case upCmd:
						//Serial<<"upCmd"<<endl;
						target()+=(tunning?tune():step())*(options.invertFieldKeys?-1:1);
						dirty=true;
						break;
					case downCmd:
						//Serial<<"downCmd"<<endl;
						target()-=(tunning?tune():step())*(options.invertFieldKeys?-1:1);;
						dirty=true;
						break;
					default:break;
				}
				if (ch==options.getCmdChar(enterCmd)&&!tunning) {
					nav.event(enterEvent);
				}
			}
			//Serial<<"value:"<<s.value<<endl;
			clamp();
		}

		template<typename T>
		void toggle<T>::printTo(idx_t i,navNode &nav,menuOut& out) {
			out<<*(prompt*)this;
			idx_t at=menuVariant<T>::sync(menuVariant<T>::sync());
			bool ed=this==nav.root->navFocus;
			bool sel=nav.sel==i;
			out.setColor(valColor,sel,prompt::enabled,ed);
			out<<menuNode::operator[](at);
		}

		template<typename T>
		void menuVariant<T>::printTo(idx_t i,navNode &nav,menuOut& out) {
			out<<*(prompt*)this;
			idx_t at=menuVariant<T>::sync(menuVariant<T>::sync());
			bool ed=this==nav.root->navFocus;
			bool sel=nav.sel==i;
			out<<(this==&nav.root->active()?":":" ");
			out.setColor(valColor,sel,prompt::enabled,ed);
			out<<menuNode::operator[](at);
		}

		template<typename T>
		void menuVariant<T>::navigate(navNode& nav,char ch,Stream& in) {
			nav.sel=sync();
			nav.doNavigation(ch,in);
			sync(nav.sel);
			if (ch==options.navCodes[enterCmd].ch) nav.root->exit();
		}

		template<typename T>
		result choose<T>::sysHandler(FUNC_PARAMS) {
			switch(event) {
				case enterEvent:
					nav.sel=menuVariant<T>::sync();
				default:
					return proceed;
			}
		}

	}//namespace Menu

#endif
