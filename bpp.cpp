#include <windows.h>
#include <cstdio>
#include <string>
#include <cstring>
#include <ctime>
#include <vector>
#include <math.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "mathparser/arithmetic.cpp"

HANDLE CON_OUT;
HANDLE CON_IN;
CONSOLE_SCREEN_BUFFER_INFO BufferInfo;
CHAR_INFO* Buffer;
COORD BufferSize;
COORD Cursor;

class Buffer {
public:
    COORD BufferSize, BufferCursor;
    CHAR_INFO* Characters;
    CONSOLE_SCREEN_BUFFER_INFO BufferInfo;
    int defaultColor = 7;

    void Init() {
        GetConsoleScreenBufferInfo(CON_OUT, &BufferInfo);
        BufferSize.X = (BufferInfo.srWindow.Right - BufferInfo.srWindow.Left + 1);
        BufferSize.Y = (BufferInfo.srWindow.Bottom - BufferInfo.srWindow.Top + 1);
        Characters = new CHAR_INFO[BufferSize.Y * BufferSize.X];
        Cursor.X = Cursor.Y = 0;
    }

    void Read() {
        ReadConsoleOutputW(CON_OUT, Characters, BufferSize, Cursor, &BufferInfo.srWindow);
    }

    void Write() {
        WriteConsoleOutputW(CON_OUT, Characters, BufferSize, Cursor, &BufferInfo.srWindow);
    }

    void SetDefaultColor(int iColor) {
        defaultColor = iColor;
    }

    void PutChar(wchar_t cChar, int posX, int posY, int color) {
        Characters[posY*BufferSize.X+posX].Char.UnicodeChar = cChar;
        Characters[posY*BufferSize.X+posX].Attributes = color == -1 ? defaultColor : color;
    }

    void PutText(std::wstring text, int posX, int posY, int color = -1) {
        for(int i = 0; i < text.length(); i++) PutChar(text[i], posX + i, posY, color);
    }

    void PutLine(std::wstring cChar, int x1, int y1, int x2, int y2, int color = -1) {
        int d, dx, dy, ai, bi, xi, yi;
        int x = x1, y = y1;
        if(x1 < x2) {
            xi = 1;
            dx = x2 - x1;
        } else {
            xi = -1;
            dx = x1 - x2;
        }
        if(y1 < y2) {
            yi = 1;
            dy = y2 - y1;
        } else {
            yi = -1;
            dy = y1 - y2;
        }
        PutChar(cChar[0], x, y, color);
        if(dx > dy) {
            ai = (dy - dx) * 2;
            bi = dy * 2;
            d = bi - dx;
            while(x != x2) {
                if(d >= 0) {
                    x += xi;
                    y += yi;
                    d += ai;
                } else {
                    d += bi;
                    x += xi;
                }
                PutChar(cChar[0], x, y, color);
            }
        } else {
            ai = (dx - dy) * 2;
            bi = dx * 2;
            d = bi - dy;
            while(y != y2) {
                if(d >= 0) {
                    x += xi;
                    y += yi;
                    d += ai;
                } else {
                    d += bi;
                    y += yi;
                }
                PutChar(cChar[0], x, y, color);
            }
        }
    }
}

const wchar_t* strrep(const wchar_t* str, int count) {
	if(count == 0) return L"";
	const size_t size = wcslen(str) * count + 1;
	wchar_t* ret = new wchar_t[size];
	wcscpy(ret, str);
	for(int i = 1; i < count; i++) {
		wcscat(ret, str);
	}
	return ret;
}

const wchar_t* strtow(const char* str) {
	const size_t size = strlen(str) + 1;
	wchar_t* wstr = new wchar_t[size];
	mbstowcs(wstr, str, size);
	return wstr;
}

std::vector<std::wstring> StringSplit(std::wstring input, wchar_t delim, int c = 0) {
    std::vector<std::wstring> out;
    int i = 0;
    size_t oldpos = 0;
    size_t pos = 0;
    while(true) {
        if(c < 1 || i < c) {
            pos = input.find(delim, oldpos);
            if(pos != std::wstring::npos) {
                out.push_back(input.substr(oldpos, pos - oldpos));
            } else {
                out.push_back(input.substr(oldpos));
                break;
            }
        } else {
            out.push_back(input.substr(oldpos));
            break;
        }
        oldpos = pos + 1;
        i++;
    }
    return out;
}

std::wstring StringJoin(const std::vector<std::wstring> &input, const wchar_t* str, int begin = 0, int end = 0) {
	std::wstringstream out;
	//std::copy(strings.begin() + begin, strings.end() + end,
		//std::ostream_iterator<std::string>(out, delim));
	return out.str();
}

double Math(std::wstring input) {
	std::string expression(input.begin(), input.end());
	ArithmeticExpression expr(expression);
	expr.parse(expression);
	return expr.value();
}

void _Console_Char(wchar_t input, int pX, int pY, int color = 7) {
	Buffer[pY*BufferSize.X+pX].Char.UnicodeChar = input;
	Buffer[pY*BufferSize.X+pX].Attributes = color;
}

void _Console_Text(std::wstring input, int pX, int pY, int color = 7) {
	for(int x = 0; x < input.length(); x++) {
		_Console_Char(input[x], pX + x, pY, color);
	}
}

void _Console_Line(std::wstring text, int x1, int y1, int x2, int y2, int color = 7) {
	int d, dx, dy, ai, bi, xi, yi;
	int x = x1, y = y1;
	if(x1 < x2) {
		xi = 1;
		dx = x2 - x1;
	} else {
		xi = -1;
		dx = x1 - x2;
	}
	if(y1 < y2) {
		yi = 1;
		dy = y2 - y1;
	} else {
		yi = -1;
		dy = y1 - y2;
	}
	_Console_Text(text, x, y, color);
	if(dx > dy) {
		ai = (dy - dx) * 2;
		bi = dy * 2;
		d = bi - dx;
		while(x != x2) {
			if(d >= 0) {
				x += xi;
				y += yi;
				d += ai;
			} else {
				d += bi;
				x += xi;
			}
			_Console_Text(text, x, y, color);
		}
	} else {
		ai = (dx - dy) * 2;
		bi = dx * 2;
		d = bi - dy;
		while(y != y2) {
			if(d >= 0) {
				x += xi;
				y += yi;
				d += ai;
			} else {
				d += bi;
				y += yi;
			}
			_Console_Text(text, x, y, color);
		}
	}
}

void drawCircle(std::wstring input, int xc, int yc, int x, int y, bool filled, int color) {
	if(filled) {
	_Console_Line(input, xc-x, yc+y, xc+x, yc+y, color);
	_Console_Line(input, xc-x, yc-y, xc+x, yc-y, color);
	_Console_Line(input, xc-y, yc+x, xc+y, yc+x, color);
	_Console_Line(input, xc-y, yc-x, xc+y, yc-x, color);
	} else {
	_Console_Text(input, xc+x, yc+y, color);
	_Console_Text(input, xc-x, yc+y, color);
	_Console_Text(input, xc+x, yc-y, color);
	_Console_Text(input, xc-x, yc-y, color);
	_Console_Text(input, xc+y, yc+x, color);
	_Console_Text(input, xc-y, yc+x, color);
	_Console_Text(input, xc+y, yc-x, color);
	_Console_Text(input, xc-y, yc-x, color);
	}
}

void drawElipsa(std::wstring input, int rx, int ry, int xc, int yc, bool filled, int color) {
	double dx, dy, d1, d2, x, y;
	x = 0;
	y = ry;
	d1 = (ry * ry) - (rx * rx * ry) + (0.25 * rx * rx);
	dx = 2 * ry * ry * x;
	dy = 2 * rx * rx * y;
	while (dx < dy) {
		if(filled) {
			_Console_Line(input, -x + xc, y + yc, x + xc, y + yc);
			_Console_Line(input, -x + xc, -y + yc, x + xc, -y + yc);
		} else {
		_Console_Text(input, x + xc, y + yc, color);
			_Console_Text(input, -x + xc, y + yc, color);
			_Console_Text(input, x + xc, -y + yc, color);
			_Console_Text(input, -x + xc, -y + yc, color);
		}
		if (d1 < 0) {
			x++;
			dx = dx + (2 * ry * ry);
			d1 = d1 + dx + (ry * ry);
		} else {
			x++;
			y--;
			dx = dx + (2 * ry * ry);
			dy = dy - (2 * rx * rx);
			d1 = d1 + dx - dy + (ry * ry);
		}
	}
	d2 = ((ry * ry) * ((x + 0.5) * (x + 0.5))) + ((rx * rx) * ((y - 1) * (y - 1))) - (rx * rx * ry * ry);
	while (y >= 0) {
		if(filled) {
			_Console_Line(input, -x + xc, y + yc, x + xc, y + yc);
			_Console_Line(input, -x + xc, -y + yc, x + xc, -y + yc);
		} else {
		_Console_Text(input, x + xc, y + yc, color);
			_Console_Text(input, -x + xc, y + yc, color);
			_Console_Text(input, x + xc, -y + yc, color);
			_Console_Text(input, -x + xc, -y + yc, color);
		}
		if (d2 > 0) {
			y--;
			dy = dy - (2 * rx * rx);
			d2 = d2 + (rx * rx) - dy;
		} else {
			y--;
			x++;
			dx = dx + (2 * ry * ry);
			dy = dy - (2 * rx * rx);
			d2 = d2 + dx - dy + (rx * rx);
		}
	}
}

void _Console_Ellipse(std::wstring input, int sx, int sy, int ex, int ey, bool filled, int color) {
	ex = sx + ex - 1;
	ey = sy + ey - 1;
	int xradius = (ex - sx) / 2;
	int yradius = (ey - sy) / 2;
	int midx = sx + xradius;
	int midy = sy + yradius;
	drawElipsa(input, xradius, yradius, midx, midy, filled, color);
}

void _Console_Circle(std::wstring input, int xc, int yc, int r, bool filled = 0, int color = 7) {
	int x = 0, y = r;
	int d = 3 - 2 * r;
	drawCircle(input, xc, yc, x, y, filled, color);
	while (y >= x)
	{

		x++;
		if (d > 0)
		{
			y--;
			d = d + 4 * (x - y) + 10;
		}
		else
			d = d + 4 * x + 6;
		drawCircle(input, xc, yc, x, y, filled, color);
	}
}

int random(int min, int max) {
	return min + rand() % ((max + 1) - min);
}

int initCOLOR = -1;
int fillCOLOR = -1;

int _Console_Fill(std::wstring chr, int pX, int pY, int color = 7) {
	if(initCOLOR == -1) initCOLOR = Buffer[pY*BufferSize.X + pX].Attributes;
	if(fillCOLOR == -1) fillCOLOR = color;

	if(pX >= BufferSize.X || pY >= BufferSize.Y) return 0;
	if(pX < 0 || pY < 0) return 0;

	if(Buffer[pY*BufferSize.X + pX].Attributes == initCOLOR) {
		Buffer[pY*BufferSize.X + pX].Char.UnicodeChar = chr[0];
		Buffer[pY*BufferSize.X + pX].Attributes = fillCOLOR;
		_Console_Fill(chr, pX - 1, pY, color);
		_Console_Fill(chr, pX + 1, pY, color);
		_Console_Fill(chr, pX, pY - 1, color);
		_Console_Fill(chr, pX, pY + 1, color);
	}
	return 1;
}

struct ELEMENT {
	int type; // 1/2/3/4 - text/button/input/checkbox
	int pX; // pozycja X
	int pY; // pozycja Y
	std::wstring data; // text/button/input/checkbox - tekst/tekst/dane/zaznaczony
	int length;
	int limit;
};

void _Console_Rect(std::wstring chr, int pX, int pY, int sX, int sY, bool filled, int color) {
	_Console_Line(chr, pX, pY, pX + sX - 1, pY, color);
	_Console_Line(chr, pX, pY + sY - 1, pX + sX - 1, pY+ sY - 1, color);
	for(int y = 1; y < sY - 1; y++) {
		if(filled) {
			_Console_Line(chr, pX, pY + y, pX + sX - 1, pY + y, color);
		} else {
			_Console_Text(chr, pX, pY + y, color);
			_Console_Text(chr, pX + sX - 1, pY + y, color);
		}
	}
}

void _Console_Capture(const wchar_t* output, int sX, int sY, int eX, int eY) {
	FILE* plik = _wfopen(output, L"wb");
	if(plik != NULL) {
		int w = eX - sX + 1;
		int h = eY - sY + 1;
		const size_t size = w * h * (sizeof(wchar_t) + 1) + 4;
		uint8_t* out = new uint8_t[size];
		*(int*)out = w;
		int i = 4;
		for(int y = sY; y <= eY; y++) {
			for(int x = sX; x <= eX; x++) {
				*(wchar_t*)(out + i) = Buffer[y*BufferSize.X+x].Char.UnicodeChar;
				i = i + sizeof(wchar_t);
				out[i++] = Buffer[y*BufferSize.X+x].Attributes;
			}
		}
		fwrite(out, sizeof(uint8_t), size, plik);
		fclose(plik);
	}
}

void _Console_Sprite(const wchar_t* input, int pX, int pY) {
	FILE* plik = _wfopen(input, L"rb");
	if(plik != NULL) {
		fseek(plik, 0, SEEK_END);
		long size = ftell(plik);
		fseek(plik, 0, SEEK_SET);
		uint8_t* data = (uint8_t*)malloc(size);
		fread(data, 1, size, plik);
		fclose(plik);
		int w;
		memcpy(&w, data, 4);
		int h = (size - 4) / (sizeof(wchar_t) + 1) / w;
		int i;
		for(int y = pY; y < h + pY; y++) {
			for(int x = pX; x < w + pX; x++) {
				i = ((y - pY) * w + x - pX) * (sizeof(wchar_t) + 1) + 4;
				memcpy(&Buffer[y*BufferSize.X+x].Char.UnicodeChar, &data[i], sizeof(wchar_t));
				Buffer[y*BufferSize.X+x].Attributes = data[i + sizeof(wchar_t)];
			}
		}
	}
}

struct CON_INP {
	int Button, X, Y, Key;
	wchar_t UnicodeChar;
	char AsciiChar;
};

CON_INP _Console_Input(int timeout = -1) {
	CON_INP ret;
	DWORD cNumRead, fdwMode, fdwSaveOldMode, iEvents;
	INPUT_RECORD irInBuf;
	GetConsoleMode(CON_IN, &fdwSaveOldMode);
	fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
	SetConsoleMode(CON_IN, fdwMode);
	clock_t tStart = 0, tStop = 0;
	if(timeout != -1) {
		tStart = clock();
	}
	ret.Button = ret.X = ret.Y = ret.Key = -1;
	while(timeout == -1 || ((float)(tStop - tStart))/CLOCKS_PER_SEC*1000 < timeout) {
		tStop = clock();
		GetNumberOfConsoleInputEvents(CON_IN, &iEvents);
		if(iEvents) {
			ReadConsoleInput(CON_IN, &irInBuf, 1, &cNumRead);
			if(cNumRead) {
				if(irInBuf.EventType == MOUSE_EVENT) {
					ret.Button = irInBuf.Event.MouseEvent.dwButtonState;
  				  ret.X = irInBuf.Event.MouseEvent.dwMousePosition.X;
  				  ret.Y = irInBuf.Event.MouseEvent.dwMousePosition.Y;
  				  break;
				} else if(irInBuf.EventType == KEY_EVENT && irInBuf.Event.KeyEvent.bKeyDown) {
					ret.Key = irInBuf.Event.KeyEvent.wVirtualKeyCode;
					ret.UnicodeChar = irInBuf.Event.KeyEvent.uChar.UnicodeChar;
					ret.AsciiChar = irInBuf.Event.KeyEvent.uChar.AsciiChar;
					break;
				}
			}
		}
	}
	SetConsoleMode(CON_IN, fdwSaveOldMode);
	return ret;
}

void Sleep(int timeout) {
	clock_t tStart = clock(), tStop = 0;
	while(((float)(tStop -tStart))/CLOCKS_PER_SEC*1000 < timeout) {
		tStop = clock();
	}
}

struct BOX {
	int pX, pY, sX, sY;
};

BOX box;
bool bTitle;
std::vector<ELEMENT> ELEMENTY;

void _Console_Box_Draw_Elements() {
    //elementy
    for(int count = 0; count < ELEMENTY.size(); count++)
	{
		switch(ELEMENTY[count].type)
		{
			case 1:
			    //label rysowanie
				_Console_Text(ELEMENTY[count].data, box.pX + ELEMENTY[count].pX, box.pY + ELEMENTY[count].pY, strtol("f0", NULL, 16));
				break;
			case 2:
			    //button rysowanie
				_Console_Text(ELEMENTY[count].data, box.pX + ELEMENTY[count].pX, box.pY + ELEMENTY[count].pY, strtol("cf", NULL, 16));
				break;
			case 3:
			    //input rysowanie
                _Console_Line(L" ", box.pX + ELEMENTY[count].pX, box.pY + ELEMENTY[count].pY, box.pX + ELEMENTY[count].pX + ELEMENTY[count].length - 1, box.pY + ELEMENTY[count].pY, strtol("80", NULL, 16));
                _Console_Text(ELEMENTY[count].data.length() > ELEMENTY[count].length ? ELEMENTY[count].data.substr(ELEMENTY[count].data.length() - ELEMENTY[count].length) : ELEMENTY[count].data, box.pX + ELEMENTY[count].pX, box.pY + ELEMENTY[count].pY, strtol("80", NULL, 16));
				break;
			case 4:
			    //checkbox rysowanie
				if(!_wcsicmp(ELEMENTY[count].data.c_str(), L"true")) {
					_Console_Text(L"x", box.pX + ELEMENTY[count].pX, box.pY + ELEMENTY[count].pY, strtol("80", NULL, 16));
				} else {
					_Console_Text(L" ", box.pX + ELEMENTY[count].pX, box.pY + ELEMENTY[count].pY, strtol("80", NULL, 16));
				}
				break;
		}
	}
}

int main() {
	int argc;
	bool noUpdate = false;
	wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	srand(time(NULL) * GetCurrentProcessId());
	CON_OUT = GetStdHandle(STD_OUTPUT_HANDLE);
	CON_IN = GetStdHandle(STD_INPUT_HANDLE);
	GetConsoleScreenBufferInfo(CON_OUT, &BufferInfo);
	BufferSize.X = (BufferInfo.srWindow.Right - BufferInfo.srWindow.Left + 1);
	BufferSize.Y = (BufferInfo.srWindow.Bottom - BufferInfo.srWindow.Top + 1);
	Buffer = new CHAR_INFO[BufferSize.Y * BufferSize.X];
	Cursor.X = Cursor.Y = 0;
	ReadConsoleOutputW(CON_OUT, Buffer, BufferSize, Cursor, &BufferInfo.srWindow);
	if(argc >= 2) {
		if(!_wcsicmp(argv[1], L"sleep") && argc >= 3) {
		Sleep(_wtoi(argv[2]));
		//bpp capture file [startX startY endX endY]
		} else if(!_wcsicmp(argv[1], L"capture") && argc >= 7) {
			_Console_Capture(argv[2], _wtoi(argv[3]), _wtoi(argv[4]), _wtoi(argv[5]), _wtoi(argv[6]));
		} else if(!_wcsicmp(argv[1], L"capture") && argc >= 3) {
			_Console_Capture(argv[2], 0, 0, BufferSize.X - 1, BufferSize.Y - 1);
		//bpp sprite file [posX posY]
		} else if(!_wcsicmp(argv[1], L"sprite") && argc >= 5) {
			_Console_Sprite(argv[2], _wtoi(argv[3]), _wtoi(argv[4]));
		} else if(!_wcsicmp(argv[1], L"sprite") && argc >= 3) {
			_Console_Sprite(argv[2], 0, 0);
		//bpp text string posX posY [color]
		} else if(!_wcsicmp(argv[1], L"text") && argc >= 5) {
			_Console_Text(argv[2], _wtoi(argv[3]), _wtoi(argv[4]), argc >= 6 ? wcstol(argv[5], NULL, 16) : 7);
		//bpo fill char posX posY [color] [mode]
		} else if(!_wcsicmp(argv[1], L"fill") && argc >= 5) {
			_Console_Fill(argv[2], _wtoi(argv[3]), _wtoi(argv[4]), argc >= 6 ? wcstol(argv[5], NULL, 16) : 7);
		//bpp line char startX startY endX endY [color]
		} else if(!_wcsicmp(argv[1], L"line") && argc >= 7) {
			_Console_Line(argv[2], _wtoi(argv[3]), _wtoi(argv[4]), _wtoi(argv[5]), _wtoi(argv[6]), argc >=8 ? wcstol(argv[7], NULL, 16) : 7);
		//bpp circle char posX posY radius [filled] [color]
		} else if(!_wcsicmp(argv[1], L"circle") && argc >= 6) {
			_Console_Circle(argv[2], _wtoi(argv[3]), _wtoi(argv[4]), _wtoi(argv[5]), argc >= 7 ? _wtoi(argv[6]) : false, argc >= 8 ? wcstol(argv[7], NULL, 16) : 7);
		//bpp ellipse char posX posY sizeX sizeY [filled] [color]
		} else if(!_wcsicmp(argv[1], L"ellipse") && argc >= 7) {
			_Console_Ellipse(argv[2], _wtoi(argv[3]), _wtoi(argv[4]), _wtoi(argv[5]), _wtoi(argv[6]), argc >= 8 ? _wtoi(argv[7]) : false, argc >= 9 ? wcstol(argv[8], NULL, 16) : 7);
		//bpp rect char posX posY sizeX sizeY [filled] [color]
		} else if(!_wcsicmp(argv[1], L"rect") && argc >= 7) {
			_Console_Rect(argv[2], _wtoi(argv[3]), _wtoi(argv[4]), _wtoi(argv[5]), _wtoi(argv[6]), argc >= 8 ? _wtoi(argv[7]) : false, argc >= 9 ? wcstol(argv[8], NULL, 16) : 7);
		//bpp random min max
		} else if(!_wcsicmp(argv[1], L"random") && argc >= 4) {
			return random(_wtoi(argv[2]), _wtoi(argv[3]));
		//bpp gotoxy posX posY
		} else if(!_wcsicmp(argv[1], L"gotoxy") && argc >= 4) {
			COORD pos;
			pos.X = _wtoi(argv[2]);
			pos.Y = _wtoi(argv[3]);
			SetConsoleCursorPosition(CON_OUT, pos);
		//bpp calc expression
		} else if(!_wcsicmp(argv[1], L"calc") && argc >= 3) {
			printf("%f", Math(argv[2]));
			noUpdate = true;
		//bpp cursor hide/show/size(int)
		} else if(!_wcsicmp(argv[1], L"cursor") && argc >= 3) {
			CONSOLE_CURSOR_INFO cursor;
			GetConsoleCursorInfo(CON_OUT, &cursor);
			if(!_wcsicmp(argv[2], L"show")) {
				cursor.bVisible = true;
			} else if(!_wcsicmp(argv[2], L"hide")) {
				cursor.bVisible = false;
			} else {
				cursor.dwSize = _wtoi(argv[2]);
			}
			SetConsoleCursorInfo(CON_OUT, &cursor);
		//bpp input [timeout]
		} else if(!_wcsicmp(argv[1], L"input")) {
			CON_INP inp = _Console_Input(argc >= 3 ? _wtoi(argv[2]) : -1);
			printf("%i %i %i %i", inp.Button, inp.X, inp.Y, inp.Key);
			noUpdate = true;
		//bpp box posX posY sizeX sizeY elements
		} else if(!_wcsicmp(argv[1], L"box") && argc >= 7) {
		    CONSOLE_CURSOR_INFO old_cursor, cursor;
			GetConsoleCursorInfo(CON_OUT, &old_cursor);
			GetConsoleCursorInfo(CON_OUT, &cursor);
            cursor.bVisible = false;
            SetConsoleCursorInfo(CON_OUT, &cursor);
			box.pX = _wtoi(argv[2]);
			box.pY = _wtoi(argv[3]);
			box.sX = _wtoi(argv[4]);
			box.sY = _wtoi(argv[5]);
			bTitle = false;
			bool noShadow = false;
			CONSOLE_SCREEN_BUFFER_INFO oldBufferInfo;
			GetConsoleScreenBufferInfo(CON_OUT, &oldBufferInfo);
			COORD cursorPos;
			std::wstring chrset = L"`1234567890-=~!@#$%^&*()_+qwertyuiop[]QWERTYUIOP{}asdfghjkl;'\\ASDFGHJKL:\"zxcvbnm,./ZXCVBNM<>? ";
			int boxButtons = 0;
			std::vector<std::wstring> eleme;
			for(int i = 6; i < argc; i++) {
				eleme = StringSplit(argv[i], L' ', 1);
				std::transform(eleme[0].begin(), eleme[0].end(), eleme[0].begin(), std::towlower);
				if(eleme.size() == 2) {
                    //checkbox posX posY [true/false]
					if(!eleme[0].compare(L"checkbox")) {
						eleme = StringSplit(eleme[1], L' ');
						if(eleme.size() >= 2) {
							ELEMENT elem;
							elem.type = 4;
							elem.pX = _wtoi(eleme[0].c_str());
							elem.pY = _wtoi(eleme[1].c_str());
							elem.length = 1;
							if(eleme.size() >= 3 && (!eleme[2].compare(L"true") || !eleme[2].compare(L"false"))) {
                                elem.data = eleme[2];
                            } else {
                                elem.data = L"false";
                            }
							ELEMENTY.push_back(elem);
						}
                        //button posX posY text
					} else if(!eleme[0].compare(L"button")) {
						eleme = StringSplit(eleme[1], L' ', 2);
						if(eleme.size() == 3) {
                            if(!boxButtons) boxButtons = 1;
                            ELEMENT elem;
							elem.type = 2;
							elem.pX = _wtoi(eleme[0].c_str());
							elem.pY = _wtoi(eleme[1].c_str());
							elem.data = eleme[2].substr(0, box.sX - elem.pX);
							elem.length = elem.data.length();
							ELEMENTY.push_back(elem);
						}
						//input posX posY limit
					} else if(!eleme[0].compare(L"input")) {
						eleme = StringSplit(eleme[1], L' ');
						if(eleme.size() >= 3) {
							ELEMENT elem;
							elem.type = 3;
							elem.pX = _wtoi(eleme[0].c_str());
							elem.pY = _wtoi(eleme[1].c_str());
							elem.length = _wtoi(eleme[2].c_str());
							if(elem.length > box.sX) elem.length -= elem.length - box.sX;
							elem.data = L"";
							elem.limit = eleme.size() >= 4 ? _wtoi(eleme[3].c_str()) : -1;
							ELEMENTY.push_back(elem);
						}
						//label posX posY text
					} else if(!eleme[0].compare(L"label")) {
						eleme = StringSplit(eleme[1], L' ', 2);
						if(eleme.size() == 3) {
							ELEMENT elem;
							elem.type = 1;
							elem.pX = _wtoi(eleme[0].c_str());
							elem.pY = _wtoi(eleme[1].c_str());
							elem.data = eleme[2].substr(0, box.sX - elem.pX);
							elem.length = elem.data.length();
							ELEMENTY.push_back(elem);
						}
						//title text
					} else if(!eleme[0].compare(L"title") && !bTitle) {
						bTitle = true;
						ELEMENT elem;
						elem.type = 0;
						elem.pX = 0;
						elem.pY = 0;
						elem.data = eleme[1].length() > box.sX ? eleme[1].substr(0, box.sX) : eleme[1];
						elem.length = elem.data.length();
						ELEMENTY.push_back(elem);
					}
				} else {
                    if(!eleme[0].compare(L"noshadow"))
                        noShadow = true;
				}
			}

			// sprawdzanie czy istnieje belka i rysowanie ewentualne
			for(int count = 0; count < ELEMENTY.size(); count++) {
				if(ELEMENTY[count].type == 0) {
					_Console_Line(L" ", box.pX, box.pY, box.pX + box.sX - 1, box.pY, strtol("90", NULL, 16));
					_Console_Text(ELEMENTY[count].data, (box.sX - ELEMENTY[count].length) / 2 + box.pX, box.pY, strtol("9f", NULL, 16));
					box.pY++;
					break;
				}
			}

			// podstawa boxa
			_Console_Rect(L" ", box.pX, box.pY, box.sX, box.sY, 1, strtol("f0", NULL, 16));

			//cien
			if(!noShadow) {
                _Console_Rect(L"\x2580", box.pX + 1, box.pY + box.sY, box.sX, 1, 1, 7);
                _Console_Rect(L" ", box.pX + box.sX, box.pY, 1, box.sY, 1, strtol("70", NULL, 16));
                if(bTitle) {
                    _Console_Text(L"\x2580", box.pX + box.sX, box.pY - 1, strtol("70", NULL, 16));
                } else {
                    _Console_Text(L"\x2580", box.pX + box.sX, box.pY, strtol("70", NULL, 16));
                }
			}

            _Console_Box_Draw_Elements();
            WriteConsoleOutputW(CON_OUT, Buffer, BufferSize, Cursor, &BufferInfo.srWindow);
			CON_INP event;
			//na którym elemencie jest myszka, ktory input wybrany
			int elem_focus = -1, input_focus = -1;
			int found_next;
			while(boxButtons) {
                // czekamy na input mysz/klawiatura
                event = _Console_Input();
                //rysowanie elementów
                _Console_Box_Draw_Elements();
                if(event.Key != -1) {
                    //Key event
                    if(input_focus != -1) {
                            // jakis input zosta³ wybrany myszka
                        switch(event.Key) {
                            /*case 13:
                                found_next = -1;
                                for(int iinp = input_focus + 1; iinp < ELEMENTY.size(); iinp++) {
                                    if(ELEMENTY[iinp].type == 3) {
                                        found_next = iinp;
                                        break;
                                    }
                                }
                                if(found_next == -1) {
                                    return 0;
                                } else {
                                    if(ELEMENTY[found_next].type == 3) {
                                        input_focus = elem_focus = found_next;
                                    } else {
                                        input_focus = -1;
                                    }
                                }
                                break;*/
                            case 8: //backspace
                                ELEMENTY[input_focus].data = ELEMENTY[input_focus].data.substr(0, ELEMENTY[input_focus].data.length() - 1);
                                break;
                            default:
                                if(ELEMENTY[input_focus].data.length() < ELEMENTY[input_focus].limit) {
                                    if(chrset.find(event.UnicodeChar) != std::string::npos) {
                                        ELEMENTY[input_focus].data += event.UnicodeChar;
                                    }
                                }
                                break;
                        }
                        cursorPos.X = box.pX + ELEMENTY[input_focus].pX + (ELEMENTY[input_focus].data.length() > ELEMENTY[input_focus].length ? ELEMENTY[input_focus].length : ELEMENTY[input_focus].data.length());
                        cursorPos.Y = box.pY + ELEMENTY[input_focus].pY;
                        SetConsoleCursorPosition(CON_OUT, cursorPos);
                        //przerysowanie inputa
                        _Console_Line(L" ", box.pX + ELEMENTY[input_focus].pX, box.pY + ELEMENTY[input_focus].pY, box.pX + ELEMENTY[input_focus].pX + ELEMENTY[input_focus].length - 1, box.pY + ELEMENTY[input_focus].pY, strtol("80", NULL, 16));
                        _Console_Text(ELEMENTY[input_focus].data.length() > ELEMENTY[input_focus].length ? ELEMENTY[input_focus].data.substr(ELEMENTY[input_focus].data.length() - ELEMENTY[input_focus].length) : ELEMENTY[input_focus].data, box.pX + ELEMENTY[input_focus].pX, box.pY + ELEMENTY[input_focus].pY, strtol("80", NULL, 16));
                    }
                } else {
                    //Mouse event
                    elem_focus = -1;
                    for(int ielem = 0; ielem <= ELEMENTY.size(); ielem++) {
                        if(event.Y == box.pY + ELEMENTY[ielem].pY && event.X >= ELEMENTY[ielem].pX + box.pX && event.X <= ELEMENTY[ielem].pX + box.pX + ELEMENTY[ielem].length - 1) {
                            //Focus na elemencie
                            elem_focus = ielem;
                        }
                    }
                }
                //nad ktoryms elementem jest myszka
                if(elem_focus != -1) {
                    switch(ELEMENTY[elem_focus].type) {
                        case 2:
                            //rysowanie ciemniejszego guzika
                            _Console_Text(ELEMENTY[elem_focus].data, box.pX + ELEMENTY[elem_focus].pX, box.pY + ELEMENTY[elem_focus].pY, strtol("47", NULL, 16));
                            if(event.Button == 1) {
                                FILE* plik = _wfopen(L"return.bat", L"w");
                                if(plik != NULL) {
                                    std::wstring buff;
                                    int inputs = 0, checkboxes = 0;
                                    buff += L"set \"button=" + ELEMENTY[elem_focus].data + L"\"\n";
                                    for(int iret = 0; iret <= ELEMENTY.size(); iret++) {
                                        if(ELEMENTY[iret].type == 3) {
                                            buff += L"set \"input" + std::to_wstring(inputs) + L'=' + ELEMENTY[iret].data + L"\"\n";
                                            inputs++;
                                        } else if(ELEMENTY[iret].type == 4) {
                                            buff += L"set \"checkbox" + std::to_wstring(checkboxes) + L'=' + ELEMENTY[iret].data + L"\"\n";
                                            checkboxes++;
                                        }
                                    }
                                    fputws(buff.c_str(), plik);
                                }
                                fclose(plik);
                                SetConsoleCursorInfo(CON_OUT, &old_cursor);
                                cursorPos.X = oldBufferInfo.dwCursorPosition.X;
                                cursorPos.Y = oldBufferInfo.dwCursorPosition.Y;
                                SetConsoleCursorPosition(CON_OUT, cursorPos);
                                return 0;
                            }
                        case 3:
                            if(event.Button == 1) {
                                input_focus = elem_focus; // ustawienie wybranego inputa jak klikniesz guzik
                                cursor.bVisible = true;
                                SetConsoleCursorInfo(CON_OUT, &cursor);
                                cursorPos.X = box.pX + ELEMENTY[input_focus].pX + (ELEMENTY[input_focus].data.length() > ELEMENTY[input_focus].length ? ELEMENTY[input_focus].length : ELEMENTY[input_focus].data.length());
                                cursorPos.Y = box.pY + ELEMENTY[input_focus].pY;
                                SetConsoleCursorPosition(CON_OUT, cursorPos);
                            }
                            break;
                        case 4:
                            if(event.Button == 1) {
                                ELEMENTY[elem_focus].data = ELEMENTY[elem_focus].data == L"true" ? L"false" : L"true";
                            }
                            break;
                    }
                } else {
                    if(event.Button == 1) {
                        elem_focus = input_focus = -1;
                        cursor.bVisible = false;
                        SetConsoleCursorInfo(CON_OUT, &cursor);
                    }
                }
                WriteConsoleOutputW(CON_OUT, Buffer, BufferSize, Cursor, &BufferInfo.srWindow);
			}
		}
	}
	if(!noUpdate) WriteConsoleOutputW(CON_OUT, Buffer, BufferSize, Cursor, &BufferInfo.srWindow);
	return 0;
}
