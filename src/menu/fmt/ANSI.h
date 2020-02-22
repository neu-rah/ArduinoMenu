/* -*- C++ -*- */
#pragma once
/**
* @author Rui Azevedo
* @date 10 May 2019
* @copyright 2020 Rui Azevedo
* @brief ArduinoMenu ANSI terminal output
*/

namespace Menu {
	#define ESCAPE 0x1B
	#define BRACE '['
	#define BLACK 0
	#define RED 1
	#define	GREEN 2
	#define YELLOW 3
	#define BLUE 4
	#define MAGENTA 5
	#define CYAN 6
	#define WHITE 7
	#define BOLD_ON 1
	#define BOLD_OFF 22
	#define UNDERLINE_ON 4
	#define UNDERLINE_OFF 24
	#define ITALICS_ON 3
	#define ITALICS_OFF 23
	#define STRIKETHROUGH_ON 9
	#define STRIKETHROUGH_OFF 29
	#define INVERSE_ON 7
	#define INVERSE_OFF 27
	#define RESET 1
	#define DEFAULT_FOREGROUND 39
	#define DEFAULT_BACKGROUND 49

	struct preamble {
		template<typename Out>
		Out& operator()(Out& out) {
			out.raw((char)ESCAPE);
			out.raw(BRACE);
			return out;
		}
	};
	template<typename Out>
	inline Out& operator<<(Out &o,preamble cmd) { return cmd(o); }

	struct preambleAndNumberAndValue {
		int x;char v;
		preambleAndNumberAndValue(int x, char v):x(x),v(v) {}
		template<typename Out>
		inline Out& operator()(Out& out) {
			out<<preamble();
			out.raw((Idx)x);
			out.raw(v);
			return out;
		}
	};
	template<typename Out>
	inline Out& operator<<(Out &o, preambleAndNumberAndValue cmd) { return cmd(o); }

	struct cursorOp {
		char v;
		cursorOp(char v):v(v) {}
		template<typename Out>
		inline Out& operator()(Out& out) {
			out<<preamble();
			out.raw('?');
			out.raw((Idx)25);
			out.raw(v);
			return out;
		}
	};
	template<typename Out>
	inline Out& operator<<(Out &o, cursorOp cmd) { return cmd(o); }

	struct setAttribute {
		int a;
		setAttribute(int a):a(a) {}
		template<typename Out>
		inline Out& operator()(Out& out) const {return out<<preambleAndNumberAndValue(a, 'm');}
	};
	template<typename Out>
	inline Out& operator<<(Out &o, const setAttribute cmd) { return cmd(o); }

	struct home {
		template<typename Out>
		inline Out& operator()(Out& out) const {return out<<preamble()<<'H';}
	};
	template<typename Out>
	inline Out& operator<<(Out &o, const home cmd) { return cmd(o); }

	struct xy {
		int x,y;
		inline xy(int x, int y):x(x),y(y) {}
		template<typename Out>
		inline Out& operator()(Out& out) const {
			out<<preamble();
			out.raw((Idx)y);
			out.raw(';');
			out.raw((Idx)x);
			out.raw('H');
			return out;
		}
	};
	template<typename Out>
	inline Out& operator<<(Out &o, const xy cmd) { return cmd(o); }

	struct up {
		int x;
		inline up(int x=1):x(x){}
		template<typename Out>
		inline Out& operator()(Out& out) const {return out<<preambleAndNumberAndValue(x,'A');}
	};
	template<typename Out>
	inline Out& operator<<(Out &o, const up cmd) { return cmd(o); }

	struct down {
		int x;
		inline down(int x=1):x(x) {}
		template<typename Out>
		inline Out& operator()(Out& out) const {return out<<preambleAndNumberAndValue(x,'B');}
	};
	template<typename Out>
	inline Out& operator<<(Out &o, const down cmd) { return cmd(o); }

	struct forward {
		int x;
		inline forward(int x=1):x(x) {}
		template<typename Out>
		inline Out& operator()(Out& out) const {return out<<preambleAndNumberAndValue(x,'C');}
	};

	struct backward {
		int x;
		inline backward(int x=1):x(x) {}
		template<typename Out>
		inline Out& operator()(Out& out) const {return out<<preambleAndNumberAndValue(x,'D');}
	};
	template<typename Out>
	inline Out& operator<<(Out &o, const backward cmd) { return cmd(o); }

	struct eraseLine {
		template<typename Out>
		inline Out& operator()(Out& out) const {return out<<preamble()<<'2'<<'K';}
	};
	template<typename Out>
	inline Out& operator<<(Out &o, const eraseLine cmd) { return cmd(o); }

	struct eraseScreen {
		template<typename Out>
		inline Out& operator()(Out& out) const {return out<<preamble()<<'1'<<'J';}
	};
	template<typename Out>
	inline Out& operator<<(Out &o, const eraseScreen cmd) { return cmd(o); }

	struct setBackgroundColor {
		int color;
		inline setBackgroundColor(int color):color(color) {}
		template<typename Out>
		inline Out& operator()(Out& out) const {return out<<setAttribute(color + 40);}
	};
	template<typename Out>
	inline Out& operator<<(Out &o, const setBackgroundColor cmd) { return cmd(o); }

	struct setForegroundColor {
		int color;
		inline setForegroundColor(int color):color(color) {}
		template<typename Out>
		inline Out& operator()(Out& out) const {return out<<setAttribute(color + 30);}
	};
	template<typename Out>
	inline Out& operator<<(Out &o, const setForegroundColor cmd) { return cmd(o); }

	struct ansiFill {
		int x1, y1, x2, y2;
		char ch;
		inline ansiFill(int x1, int y1, int x2, int y2,char ch=' '):x1(x1),y1(y1),x2(x2),y2(y2),ch(ch) {}
		template<typename Out>
		inline Out& operator()(Out& out) const {
			for (int y = y1; y <= y2; y++) {
				xy(x1,y)(out);
				for (int x = x1; x <= x2; x++) out.raw(ch);
			}
			return out;
		}
	};

	template<char c>
	struct attr {
		template<typename Out>
		inline Out& operator()(Out& out) const {return out<<setAttribute(c);}
	};
	template<typename Out,char c>
	inline Out& operator<<(Out &o, const attr<c> cmd) { return cmd(o); }

	struct hideCursor {
		template<typename Out>
		inline Out& operator()(Out& out) const {return out<<cursorOp('h');}
	};

	struct showCursor {
		template<typename Out>
		inline Out& operator()(Out& out) const {return out<<cursorOp('l');}
	};

	typedef attr<BOLD_ON> boldOn;
	typedef attr<BOLD_OFF> boldOff;
	typedef attr<ITALICS_ON> italicsOn;
	typedef attr<ITALICS_OFF> italicsOff;
	typedef attr<UNDERLINE_ON> underlineOn;
	typedef attr<UNDERLINE_OFF> underlineOff;
	typedef attr<STRIKETHROUGH_ON> strikethroughOn;
	typedef attr<STRIKETHROUGH_OFF> strikethroughOff;
	typedef attr<INVERSE_ON> inverseOn;
	typedef attr<INVERSE_OFF> inverseOff;
	typedef attr<RESET> reset;
	typedef attr<DEFAULT_BACKGROUND> defaultBackground;
	typedef attr<DEFAULT_FOREGROUND> defaultForeground;

	struct AnsiFmt {
		template<typename O>
		struct Part:O {
		  using This=AnsiFmt::Part<O>;
			template<bool toPrint=true>
			inline void clrLine(Idx nr) {
				if(toPrint) ansiFill(O::orgX(),nr,O::orgX()+O::width(),nr)(*this);
				setCursor(O::orgX(),nr);
			}
			inline void setCursor(int x,int y) {xy(x,y)(*this);}
		  template<bool io,bool toPrint=true>
		  inline void fmtPanel(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
				if (io) {
					setCursor(O::orgX(),O::orgY());
					setBackgroundColor(BLUE)(*this);
					setForegroundColor(WHITE)(*this);
					boldOn()(*this);
					if (toPrint)
						ansiFill(O::orgX(),O::orgY(),O::orgX()+O::width(),O::orgY()+O::height())(*this);
					O::template fmtPanel<io,toPrint>(n,s,e,m);
				} else {
					O::template fmtPanel<io,toPrint>(n,s,e,m);
					boldOff()(*this);
					defaultForeground()(*this);
					defaultBackground()(*this);
				}
				setCursor(O::orgX(),O::orgY());
		  }
		  template<bool io,bool toPrint=true>
		  inline void fmtMenu(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
				if (io) {
					hideCursor()(*this);
					setBackgroundColor(BLUE)(*this);
					setForegroundColor(WHITE)(*this);
					boldOn()(*this);
				} else {
					O::template fmtMenu<io,toPrint>(n,s,e,m);
					boldOff()(*this);
					defaultForeground()(*this);
					defaultBackground()(*this);
					// showCursor()(*this);
					setCursor(0,0);
				}
				// setCursor(O::orgX(),O::orgY());
		  }
		};
	};
};
