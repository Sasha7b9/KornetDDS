#pragma once
#include "GeneratorSettingsTypes.h"
#include "Command.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Multiplexor
{
friend class Interface;
public:

    /// �������������� �������������
    static void Init();
    /// �������������� ������������� � ������������ � ������ ���������������� ������� �� ������ ������
    static void SetMode(Chan ch, Form form);

    static Form GetMode(Chan ch);

private:
    static void SetPin(uint16 pin);
    static void ResetPin(uint16 pin);

    static void WriteRegister(Register reg, uint value);

    static Form mode[Chan::Number];
};
