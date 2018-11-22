#pragma once
#include "Command.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define LOG_WRITE(...)  Console::AddString(__VA_ARGS__)
#define LOG_ERROR(...)  Console::AddString(__VA_ARGS__)


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Console
{
public:
    static bool ExistString();
    /// ���������� ������������ ������
    static char *GetString();
    /// ������� ������������ ������ �� ���������
    static void DeleteString();

    static void AddString(char *format, ...);

private:
    static void AddConstString(char *buffer);
    static void AddConstString(pString buffer);
};
