#pragma once
#include "defines.h"
#include "Command.h"
#include "CommonTypes.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4514)
#endif

struct BillingTime
{
    enum EBillingTime
    {
        _1ms,
        _10ms,
        _100ms,
        _1000ms,
        _10000ms
    } value;
    operator int() const { return (int)value; };
};

struct Language
{
    enum ELanguage
    {
        RU,
        EN
    } value;
    Language(int v) : value((ELanguage)v) {};
    operator uint8() const { return (uint8)value; };
};

struct SizeByte
{
    enum ESizeByte
    {
        _7bit,
        _8bit
    } value;
};

struct StopBit
{
    enum EStopBit
    {
        _1,
        _2
    } value;
};

struct Parity
{
    enum EParity
    {
        Off,
        On
    } value;
};

/// 
struct FreqInterval
{
    enum EFreqInterval
    {
        _1,
        _10
    } value;
    FreqInterval(int v) : value((EFreqInterval)v) {};
    operator uint8() const { return (uint8)value; };
};

/// �������� ������� ��� ������
struct FreqMeasure
{
    enum EFreqMeasure
    {
        Freq,
        Period
    } value;
    FreqMeasure(int v) : value((EFreqMeasure)v) {};
    operator uint8() const { return (uint8)value; };
};

/// ����� ����������� ������ � ������ ��������� ������� �������
struct FreqAvePeriod
{
    enum EFrqAvePeriod
    {
        _1,
        _10,
        _100,
        _1000,
        _10000
    } value;
    operator uint8() const { return (uint8)value; };
};

/// ����� �������
struct FreqTimeStamps
{
    enum EFreqTimeStamps
    {
        _1kHz,
        _10kHz,
        _100kHz,
        _1MHz,
        _10MHz
    } value;
    operator uint8() const { return (uint8)value; };
};

/// �������� ����� �����������
struct FreqTest
{
    enum EFreqTest
    {
        Off,
        On
    } value;
    FreqTest(int v) : value((EFreqTest)v) {};
    operator uint8() const { return (uint8)value; };
};


#ifdef WIN32
#pragma warning(pop)
#endif
