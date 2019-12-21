#pragma once
#include "defines.h"


/// ����� ������
struct GeneratorWritePin
{
    enum E
    {
        AD9952_SPI3_CSA,
        AD9952_SPI3_CSB,
//        AD9952_IO_UPD,
//        AD9952_IOSYNA,
//        AD9952_IOSYNB,
//        AD9952_RES_DDS,
        Count
    } value;
    GeneratorWritePin(E v) : value(v) {};
    operator uint8() const { return static_cast<uint8>(value); };
};


struct CPU
{
    static void Init();
    /// ���������� �� PA2 ������� ����, ��� ��������� �����
    static void SetBusy();
    /// ���������� �� PA2 ������� ����, ��� ��������� ��������
    static void SetReady();
    
    static void WritePin(GeneratorWritePin pin, bool set);

private:

    static void InitGPIOS();
};
