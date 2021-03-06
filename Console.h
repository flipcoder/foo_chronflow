#pragma once
#include "stdafx.h"

class Console
{
private:
	Console(void);
public:
	static void create(void);
public:
	static void print(const wchar_t* str);
	static void println(const wchar_t* str);
	static void printf(const wchar_t* format, ...);
private:
	static HANDLE screenBuffer;
};
