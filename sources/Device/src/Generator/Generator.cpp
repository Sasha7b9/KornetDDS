#include "AD5697.h"
#include "FPGA.h"
#include "Generator.h"
#include "Multiplexor.h"
#include "GeneratorSettingsTypes.h"
#include "Hardware/CPU.h"
#include "FreqMeter/FreqMeter.h"
#include "CommonTypes.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool waveIsSine = true;          // ����� ��� ����, ����� ������ ������� � ���������� ����� - ���� ��� AD9952


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Generator::Init()
{
    EnableChannel(Chan::A, false);
    EnableChannel(Chan::B, false);
    AD9952::Init();
    AD5697::Init();
    FPGA::Init();
    Multiplexor::Init();
    FreqMeter::Init();
    
    for(int i = 0; i < 2; i++)
    {
        Chan ch = (Chan::E)i;
        EnableChannel(ch, true);
        Generator::SetFormWave(ch, Form::Sine);
        Generator::SetFrequency(ch, 1e6f);
        Generator::SetOffset(ch, 0.0f);
        Generator::SetAmplitude(ch, 10.0f);
    }

    
    for(int i = 0; i < 2; i++)
    {
        /*
        Generator::SetFormWave(Chan::A, Form::PacketImpuls);
        FPGA::PacketImpulse::SetPeriodImpulse(2e-4  f);
        FPGA::PacketImpulse::SetDurationImpulse(1e-4f);
        FPGA::PacketImpulse::SetNumberImpules(3);

        FPGA::PacketImpulse::SetPeriodPacket(1e-3f);
        */

        
        /*
        Generator::SetFormWave(Chan::A, Form::Impulse);
        FPGA::SetPeriodImpulse(Chan::A, 1.0001e-3f);
        FPGA::SetDurationImpulse(Chan::A, 2e-4f);
        
    
        Generator::SetFormWave(Chan::B, Form::Impulse);
        FPGA::SetPeriodImpulse(Chan::B, 1e-3f);
        FPGA::SetDurationImpulse(Chan::B, 1e-4f);
        */
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Generator::EnableChannel(Chan ch, bool enable)
{
    if(!FPGA::Start())
    {
        static const GeneratorWritePin pin[Chan::Number] = { GeneratorWritePin::Pin_P3_OutA, GeneratorWritePin::Pin_P4_OutB };

        CPU::WritePin(pin[ch], !enable);
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Generator::SetFormWave(Chan ch, Form form)
{
    if(ch.value < Chan::Number && form.value < Form::Number)
    {
        waveIsSine = form.Is(Form::Sine);

        if (!waveIsSine)
        {
            FPGA::SetWaveForm(ch, form);
        }

        Multiplexor::SetMode(ch, form);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Generator::SetParameter(Chan ch, CommandPanel command, float value)
{
    static const struct StructFunc
    {
        typedef void (*pFuncChF)(Chan, float);
        pFuncChF func;
        StructFunc(pFuncChF f) : func(f) {};
    }
    func[CommandPanel::Number] =
    {
        EmptyFunc,
        EmptyFunc,
        EmptyFunc,
        SetFrequency,
        SetAmplitude,
        SetOffset,
        SetDuration,
        SetDutyRatio,
        SetPhase,
        EmptyFunc,
        EmptyFunc,
        SetDelay,
        EmptyFunc,
        EmptyFunc,
        EmptyFunc,
        EmptyFunc,
        EmptyFunc,
        EmptyFunc,
        EmptyFunc,
        EmptyFunc,
        SetPacketPeriod,
        SetPacketNumber,
        EmptyFunc
    };

    func[command].func(ch, value);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Generator::SetFrequency(Chan ch, float frequency)
{
    if (waveIsSine)
    {
        AD9952::SetFrequency(ch, frequency);
    }
    else
    {
        FPGA::SetFrequency(ch, frequency);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Generator::SetAmplitude(Chan ch, float amplitude)
{
    if (waveIsSine)
    {
        AD9952::SetAmplitude(ch, amplitude);
    }
    else
    {
        FPGA::SetAmplitude(ch, amplitude);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Generator::SetOffset(Chan ch, float offset)
{
    if(waveIsSine)
    {
        AD5697::SetOffset(ch, offset);
    }
    else
    {
        FPGA::SetOffset(ch, offset);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Generator::SetPhase(Chan ch, float phase)
{
    AD9952::SetPhase(ch, phase);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Generator::SetPacketNumber(Chan, float)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Generator::SetPacketPeriod(Chan, float)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Generator::SetDutyRatio(Chan, float)
{

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Generator::SetDuration(Chan ch, float value)
{
    FPGA::SetDurationImpulse(ch, value);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Generator::SetDelay(Chan, float)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Generator::EmptyFunc(Chan, float)
{
}
