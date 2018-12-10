#include "stdafx.h"
#ifndef WIN32
#include "defines.h"
#include "log.h"
#include "String.h"
#include "Display/Painter.h"
#include "Display/Text.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <stdarg.h>
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
String::String() : buffer(nullptr)
{

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
String::String(const String &rhs) : buffer(nullptr)
{
    if (Allocate(std::strlen(rhs.CString())) + 1)
    {
        std::strcpy(buffer, rhs.CString());
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
String::String(char symbol) : buffer(nullptr)
{
    if (Allocate(2))
    {
        buffer[0] = symbol;
        buffer[1] = 0;
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void String::From(const char *format, ...)
{
#define SIZE 100
    char buf[SIZE + 1];

    va_list args;
    va_start(args, format);
    int numSymbols = std::vsprintf(buf, format, args);
    va_end(args);

    if (numSymbols < 0 || numSymbols > SIZE)
    {
        LOG_ERROR("����� ������� ���");
    }

    if (Allocate(std::strlen(buf) + 1))
    {
        std::strcpy(buffer, buf);
    }

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
String::String(const char *format, ...) : buffer(nullptr)
{
#define SIZE 100
    char buf[SIZE + 1];

    va_list args;
    va_start(args, format);
    int numSymbols = std::vsprintf(buf, format, args);
    va_end(args);

    if (numSymbols < 0 || numSymbols > SIZE)
    {
        LOG_ERROR("����� ������� ���");
    }

    if (Allocate(std::strlen(buf) + 1))
    {
        std::strcpy(buffer, buf);
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
String::~String()
{
    Free();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void String::Free()
{
    if (buffer)
    {
        std::free(buffer);
    }
    buffer = 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
char *String::CString() const
{
    return buffer;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool String::Allocate(uint size)
{
    Free();
    buffer = (char *)std::malloc(size);
    if (buffer)
    {
        return true;
    }
    else
    {
        LOG_ERROR("�� ������� ������");
    }
    return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int String::Draw(int x, int y, Color color) const
{
    return Text::DrawText(x, y, CString(), color);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void String::Release()
{
    Free();
}