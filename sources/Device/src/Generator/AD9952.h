#pragma once
#include "defines.h"
#include "Hardware/CPU.h"
#include "GeneratorSettingsTypes.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class AD9952
{
public:

    static void Init();

    static void SetFrequency(Chan ch, float frequency);

    static void SetAmplitude(Chan ch, float amplitude);

    /// ������� ��� ��������� "�����"
    class Ramp
    {
    public:
        /// ����������/�������� ������������� ��������� �������� "����"
        static void SetEnabled(Chan ch, bool enable);
        /// ���������� ������������
        static void SetDuration(Chan ch, float duration);
        /// ���������� ������
        static void SetAmplitude(Chan ch, float amplitude);
        /// True ��������, ��� ����������� ��������� "�����"
        static bool enabled[Chan::Number];
        /// ������������ ����
        static float duration[Chan::Number];

        static float amplitude[Chan::Number];
    };

private:
    struct Register
    {
        enum E
        {
            CFR1,
            CFR2,
            ASF,
            ARR,
            FTW0,
            POW
        } value;
        Register(E v) : value(v) { };
        operator uint8() const { return (uint8)value;  };
    };

    static void WriteToHardware(Chan ch, Register reg, uint value);
    static GeneratorWritePin ChipSelect(Chan ch);
    static void Reset();
    static void WriteRegister(Chan ch, Register reg);
    static void WriteCFR1(Chan ch);
    static void WriteCFR2(Chan ch);
    static void WriteARR(Chan ch);
    static void WriteASF(Chan ch);
    static void WriteFTW0(Chan ch);
};
