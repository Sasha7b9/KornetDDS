#pragma once
#include "Command.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum
{
    A,
    B,
    NumChannels
} Channel;

typedef enum
{
    Russian,
    English
} Language;

typedef enum
{
    SizeByte_7bit,
    SizeByte_8bit
} SizeByte;

typedef enum
{
    StopBit_1,
    StopBit_2
}StopBit;


float MinValue(WaveParameter param);
float MaxValue(WaveParameter param);
