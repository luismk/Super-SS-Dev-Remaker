// Arquivo Color.cpp

#if defined(_WIN32)
#pragma pack(1)
#endif
#include "Color.hpp"
#include <time.h>
using namespace stdA;

ConsoleEx::ConsoleEx()  {
}

ConsoleEx::~ConsoleEx() {
}

void ConsoleEx::setcolor(int color)
{
#ifdef _WIN32
	switch (color)
	{
	case 0: // White on Black
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
			FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		break;
	case 1: // Red on Black
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
		break;
	case 2: // Green on Black
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
		break;
	case 3: // Yellow on Black
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
			FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
		break;
	case 4: // Blue on Black
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);
		break;
	case 5: // Magenta on Black
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
			FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE);
		break;
	case 6: // Cyan on Black
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
			FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
		break;
	case 7: // Black on Gray
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_INTENSITY | BACKGROUND_INTENSITY);
		break;
	case 8: // Black on White
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
			BACKGROUND_INTENSITY | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN |
			BACKGROUND_BLUE);
		break;
	case 9: // Red on White
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
			BACKGROUND_INTENSITY | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN |
			BACKGROUND_BLUE | FOREGROUND_RED);
		break;
	case 10: // Green on White
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
			BACKGROUND_INTENSITY | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN |
			BACKGROUND_BLUE | FOREGROUND_GREEN);
		break;
	case 11: // Yellow on White
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
			BACKGROUND_INTENSITY | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN |
			BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
		break;
	case 12: // Blue on White
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
			BACKGROUND_INTENSITY | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN |
			BACKGROUND_BLUE | FOREGROUND_BLUE);
		break;
	case 13: // Magenta on White
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
			BACKGROUND_INTENSITY | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN |
			BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_BLUE);
		break;
	case 14: // Cyan on White
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
			BACKGROUND_INTENSITY | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN |
			BACKGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_BLUE);
		break;
	case 15: // White on White
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
			BACKGROUND_INTENSITY | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN |
			BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		break;
	default: // White on Black
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
			FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		break;
	}
#elif LINUX
#endif
}

void ConsoleEx::restcolor()
{
#ifdef _WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
		FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#elif LINUX
#endif
}


void ConsoleEx::Log()
{
	BarSpace();
	BarSpace();
	BarSpace();
	TextCentralize((char*)"CREATOR CODE: ACRISIO FILHO");
	TextCentralize((char*)"Update v6.2");
	TextCentralize((char*)"REVISION: github.com/luismk");
	BarSpace();
	BarSpace();
	BarSpace();
}

void ConsoleEx::BarSpace()
{
	setcolor(1);
	printf("************************************************************************************************************************\n");
	restcolor();
}

void ConsoleEx::TextCentralize(char* str)
{
	setcolor(2);
	//antes no barspace era 80, agora é 121 aumentei para ficar muito mais bonitinho ^^ 
	//aquilo serve pra centralizar o texto !
	int pos = (int)(121 - strlen(str)) / 2;
	for (int i = 0; i < pos; i++)
	{
		printf(" ");
	}
	printf("%s\n", str);
	restcolor();
}

void ConsoleEx::ConsoleSetTitle(std::string new_title)
{
	system(std::string("title " + new_title).c_str());
}
