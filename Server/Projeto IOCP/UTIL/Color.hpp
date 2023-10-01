// Arquivo color.hpp

#pragma once
#ifndef _STDA_COLOR_HPP
#define _STDA_COLOR_HPP

#include <cstdint>
#include <windows.h>
#include <string>
#include <stdio.h>
#include <iostream>
namespace stdA {

	class ConsoleEx {

	public:
		ConsoleEx();
		virtual ~ConsoleEx();
		void setcolor(int color);
		void restcolor();
		void Log();
		void BarSpace();
		void TextCentralize(char*);
		void ConsoleSetTitle(std::string);
	};
}

#endif // !_STDA_COLOR_HPP
