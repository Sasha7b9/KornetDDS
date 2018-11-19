#include "stdafx.h"
#ifndef WIN32
#include "defines.h"
#include "GeneratorPanel.h"
#include "Interface/InterfacePanel.h"
#include "Log.h"
#include "Menu/MenuItems.h"
#include "Hardware/CPU.h"
#include "Hardware/Timer.h"
#include "FrequencyMeter/FrequencyMeter.h"
#include "Settings/Settings.h"
#include "Display/Console.h"
#include "Signals.h"
#include "Command.h"
#include "structs.h"
#include "Signals.h"
#include "Display/Console.h"
#include "Utils/Math.h"
#include "Utils/Debug.h"
#include <cmath>
#include <string.h>
#include <cstdlib>
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Generator::EnableChannel(Chan ch, bool enable)
{
    Interface::Send(Buffer(Command::EnableChannel, ch, enable ? 1u : 0u));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Generator::LoadStartMode(Chan ch, int mode)
{
    Interface::Send(Buffer(Command::SetStartMode, ch, (uint8)mode));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Generator::LoadRegister(Register reg, uint64 value)
{
    INIT_BIT_SET_64(bitSet, value);

    uint8 buffer[10] = {Command::WriteRegister, (uint8)reg,   bitSet.byte0, bitSet.byte1, bitSet.byte2, bitSet.byte3,
                                                                    bitSet.byte4, bitSet.byte5, bitSet.byte6, bitSet.byte7};
    Interface::Send(buffer, 10);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Generator::SetDebugMode(bool enable)
{
    Interface::Send(Buffer(Command::ModeDebug, enable ? 1u : 0u));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Generator::Reset()
{
    Interface::Send(Buffer(Command::RunReset));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Generator::SetFormWave(Wave *w)
{
    Chan ch = w->GetChannel();
    uint8 form = (uint8)FORM(ch)->value;

    if(FORM(ch)->IsDDS())
    {
        SetFormDDS(FORM(ch));
    }
    else
    {
        Interface::Send(Buffer(Command::SetFormWave, ch, form));
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Generator::SetFormDDS(Form *form)
{
    Chan ch = form->GetWave()->GetChannel();

    switch (form->value)
    {
        uint8 buffer[FPGA_NUM_POINTS * 2];
        float data[FPGA_NUM_POINTS];

        case Form::RampPlus:
            {
                float step = 2.0f / FPGA_NUM_POINTS;

                for(int i = 0; i < FPGA_NUM_POINTS; i++)
                {
                    data[i] = -1.0f + step * i;
                }

                TransformDataToCode(data, buffer);

                LoadPointsToDDS(ch, buffer);
            }
            break;
        case Form::RampMinus:
            {
                float step = 2.0f / FPGA_NUM_POINTS;

                for (int i = 0; i < FPGA_NUM_POINTS; i++)
                {
                    data[i] = 1.0f - step * i;
                }

                TransformDataToCode(data, buffer);

                LoadPointsToDDS(ch, buffer);
            }
            break;
        case Form::Triangle:
            {
                float step = 2.0f / (FPGA_NUM_POINTS / 2);

                for (int i = 0; i < FPGA_NUM_POINTS / 2; i++)
                {
                    data[i] = - 1.0f + step * i;
                }

                for(int i = FPGA_NUM_POINTS / 2; i < FPGA_NUM_POINTS; i++)
                {
                    data[i] = 1.0f - step * (i - FPGA_NUM_POINTS / 2);
                }

                TransformDataToCode(data, buffer);

                LoadPointsToDDS(ch, buffer);
            }
            break;
        case Form::Meander:
        case Form::Sine:
        case Form::Impulse:
        case Form::PacketImpuls:
        case Form::DDS:
        case Form::Number:
            break;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Generator::TransformDataToCode(float d[FPGA_NUM_POINTS], uint8 code[FPGA_NUM_POINTS * 2])
{
    int max = 0x1fff;

    for (int i = 0; i < FPGA_NUM_POINTS; i++)
    {
        uint16 c = (uint16)(std::fabs(d[i]) * max);

        if (Sign(d[i]) == -1)
        {
            SetBit(c, 13);
        }

        code[i] = (uint8)c;
        code[i + FPGA_NUM_POINTS] = (uint8)(c >> 8);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Generator::LoadPointsToDDS(Chan ch, uint8 points[FPGA_NUM_POINTS * 2])
{
    Interface::Send(Buffer(Command::LoadFormDDS, ch));

    Interface::Send(points, FPGA_NUM_POINTS * 2);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Generator::SetParameter(ParameterChoice *param)
{
    static const struct StructCommand
    {
        Command command;
        StructCommand(Command::E c) : command(c) {};
    }
    commands[ParameterChoice::Number] =
    {
        Command::SetPolarity,
        Command::SetStartMode,
        Command::SetManipulationMode,
        Command::SetManipulation
    };

    Interface::Send(Buffer((uint8)commands[param->value].command, (uint8)param->GetForm()->GetWave()->GetChannel(), (uint8)param->GetChoice()));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Generator::SetParameter(ParameterValue *param)
{
    static const struct StructCommand
    {
        Command command;
        StructCommand(Command::E c) : command(c) {};
    }
    commands[ParameterValue::Number] =
    {
        Command::SetFrequency,
        Command::SetPeriod,
        Command::SetAmplitude,
        Command::SetOffset,
        Command::SetDuration,
        Command::SetDutyRatio,
        Command::SetPhase,
        Command::SetDelay,
        Command::SetDurationRise,
        Command::SetDurationFall,
        Command::SetDurationStady,
        Command::SetDutyFactor,
        Command::SetManipulationDuration,
        Command::SetManipulationPeriod,
        Command::SetPacketPeriod,
        Command::SetPacketNumber,
        Command::RequestData
    };

    uint8 buffer[6] = {(uint8)commands[param->value].command, (uint8)param->GetForm()->GetWave()->GetChannel()};

    float value = param->GetValue();

    if(param->Is(ParameterValue::Offset))
    {
        value -= 5.0f;
    }

    memcpy(&buffer[2], &value, 4);
    Interface::Send(buffer, 6);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Generator::TuneChannel(Chan ch)
{
    EnableChannel(ch, CHANNEL_ENABLED(ch));

    FORM(ch)->TuneGenerator(ch);
}
