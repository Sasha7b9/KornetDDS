#pragma once
#include "defines.h"
#include "Settings/SettingsTypes.h"
#include "CommonTypes.h"


#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4514 4623)
#endif

#include "Wave.h"


/** @addtogroup Panel
 *  @{
 *  @addtogroup Interface
 *  @{
 */

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
    ��������� �������

1.  WRITE_SERVICE_COMMAND

    0                1        2
2.  SET_FORM_CHANNEL NUM_CHAN FormChannel

    0                     1        2                3...6
3.  SET_PARAMETER_CHANNEL NUM_CHAN ParameterChannel value

    0              1             2...5
12. WRITE_REGISTER NAME_REGISTER VALUE

    0                1        2...3       4...9
20  CREATE_HAND_WAVE NUM_CHAN START_POINT points

    0              1        2
22  SetRampForSine NUM_CHAN 0/1

    0                      1        2...5
23  SetDurationRampForSine NUM_CHAN value
*/

/// ��������� ������� ��� �������� � �������� ���������
struct CommandPanel
{
    enum E
    {
        None = 0,
        ENABLE_CHANNEL = 1,     ///< ��������/��������� �����
        SET_FORM_WAVE = 2,      ///< ���������� ����� �������
        SET_FREQUENCY = 3,      ///< ���������� �������
        SET_AMPLITUDE = 4,      ///< ���������� ���������
        SET_OFFSET = 5,         ///< ���������� ��������
        SET_DURATION = 6,
        SET_DUTYRATIO = 7,
        SET_PHASE = 8,
        RUN_RESET = 9,          ///< ����� ���������
        MODE_DEBUG = 10,        ///< ��������� ����������� ������ - ����� ���� ����������� ������� � FPGA
        SET_DELAY = 11,         ///< ��������
        WRITE_REGISTER = 12,    ///< ������� �������� � �������
        READ_DATA = 13,         ///< ������ ������ �� ����������
        SET_DEPTH_MODULATION = 14,
        SET_POLARITY = 15,
        SET_DURATION_RISE = 16,
        SET_DURATION_FALL = 17,
        SET_DURATION_STADY = 18,
        SET_DUTY_FACTOR = 19,
        SetModulation = 20,
        CREATE_HAND_WAVE = 21,          ///< �������� ����� ������������� �������
        SET_HAND_WAVE = 22,             ///< ������������� ����� ������������� �������
        SetRampForSine = 23,            ///< ���������� ����� ��������� ����� ��� ������
        SetDurationRampForSine = 24,    ///< ���������� ������������ ������� ��� ��������� ������ �����
        SetAmplitudeRampForSine = 25,   ///< ���������� ��������� ��� ����, ������������ �����
        Number
    } value;
    CommandPanel(E v) : value(v){};
    operator uint8() const { return (uint8)value; };
};

/// ��������� �������, ����������� �� ��������� ����������
struct CommandGenerator
{
    enum E
    {
        //   0            1...5
        COM_FREQ_MEASURE = 1,   ///< FREQ_MEASURE �������                                                        /* ��������� ��������� ����������� */
        Number
    } value;
};

struct Register
{
    enum E
    {
        Multiplexor1,
        Multiplexor2,
        OffsetA,
        OffsetB,
        FreqMeterLevel,
        FreqMeterHYS,
        FPGA_RG0_Control,           ///< ����������� �������
        FPGA_RG1_Freq,              ///< ������� ���� �������
        FPGA_RG2_Amplitude,         ///< ������� ���� ����������
        FPGA_RG3_RectA,             ///< ������� ���� ������� ������������� ����� ������ �
        FPGA_RG4_RectB,             ///< ������� ���� ������� ������������� ����� ������ B
        FPGA_RG5_PeriodImpulseA,    ///< ������� ���� ������� ������������� ��������� ������ �
        FPGA_RG6_DurationImpulseA,  ///< ������� ���� ������������ ������������ ��������� ������ �
        FPGA_RG7_PeriodImpulseB,    ///< ������� ���� ������� ������������� ��������� ������ B
        FPGA_RG8_DurationImpulseB,  ///< ������� ���� ������������ ������������ ��������� ������ B
        FPGA_RG9_FreqMeter,         ///< ������� ���������� �����������
        FPGA_RG10_Offset,           ///< ������� ��������
        Multiplexor3,
        FreqMeter_Resist,
        FreqMeter_Couple,
        FreqMeter_Filtr,
        Number
    } value;
    Register(E v) : value(v) {};
    operator uint8() const { return (uint8)value; };
    pString Name() const;
};


#define LENGTH_SPI_BUFFER   10

#define SPI_SLAVE_SYNBYTE   0x53

#define SPI_MASTER_SYNBYTE  0xAC

#define NAME_FORM(form, lang)  Form(form).Name(lang)
#define FORM_RU(form) NAME_FORM(form, Language::RU)
#define FORM_EN(form) NAME_FORM(form, Language::EN)


/** @}  @} */

#ifdef WIN32
#pragma warning(pop)
#endif
