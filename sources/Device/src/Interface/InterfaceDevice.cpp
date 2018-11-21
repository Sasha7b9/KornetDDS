#include "stdafx.h"
#ifndef WIN32
#include "defines.h"
#include "log.h"
#include "Message.h"
#include "Transceiver.h"
#include "InterfaceDevice.h"
#include "FDrive/FDriveDevice.h"
#include "Utils/Console.h"
#include "Generator/GeneratorDevice.h"
#include "Generator/FPGA.h"
#include "Hardware/CPU.h"
#include "Hardware/Timer.h"
#include "Hardware/Modules/SPI.h"
#include "FreqMeter/FreqMeter.h"
#include "Settings/Settings.h"
#include "Utils/Array.h"
#include "Utils/Debug.h"
#include "Utils/StringUtils.h"
#include "Command.h"
#include "structs.h"
#include <string.h>
#include <cstdlib>
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ��������� �������� ��������, ��� ��� ������� �������� � ������ ��� ���������� ������������ ��������
volatile static uint freqForSend = MAX_UINT;
uint  Interface::timeLastReceive = 0;

static const struct FuncInterface
{
    typedef void(*pFuncInterfaceVpM)(Message *);             // ���������� ������� ��������� �� �������� ������
    pFuncInterfaceVpM func;
    FuncInterface(pFuncInterfaceVpM f) : func(f) {};
}
commands[Command::Number] =
{
/* RequestData             */ Interface::SendData,
/* EnableChannel           */ Interface::EnableChannel,
/* SetFormWave             */ Interface::SetFormWave,
/* SetFrequency            */ Interface::ParameterValue,
/* SetAmplitude            */ Interface::ParameterValue,
/* SetOffset               */ Interface::ParameterValue,
/* SetDuration             */ Interface::ParameterValue,
/* SetDutyRatio            */ Interface::ParameterValue,
/* SetPhase                */ Interface::ParameterValue,
/* RunReset                */ Interface::RunReset,
/* ModeDebug               */ Interface::ModeDebug,
/* SetDelay                */ Interface::ParameterValue,
/* WriteRegister           */ Interface::WriteRegister,
/* SetDurationRise         */ Interface::Empty,
/* SetDurationFall         */ Interface::Empty,
/* SetDurationStady        */ Interface::Empty,
/* SetDutyFactor           */ Interface::Empty,
/* SetManipulation         */ Interface::SetManipulation,
/* SetManipulationDuration */ Interface::ParameterValue,
/* SetManipulationPeriod   */ Interface::ParameterValue,
/* SetPacketPeriod         */ Interface::ParameterValue,
/* SetPacketNumber         */ Interface::ParameterValue,
/* SetStartMode            */ Interface::SetStartMode,
/* SetPeriod               */ Interface::ParameterValue,
/* SetPolarity             */ Interface::SetPolarity,
/* SetManipulationMode     */ Interface::SetManipulationMode,
/* LoadFromDDS             */ Interface::LoadFormDDS,
/* FreqMeasure             */ Interface::Empty,
/* Log                     */ Interface::Empty,
/* FDrive_NumDirsAndFiles  */ Interface::Empty,
/* FDrive_Mount            */ Interface::Empty,
/* FDrive_RequestDir       */ Interface::Empty,
/* FDrive_RequestFile      */ Interface::Empty,
/* Test                    */ Interface::Test,
/* SetKoeffCalibration     */ Interface::SetKoeffCalibration
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Interface::Update()
{
#define TIMEOUT 200

    CPU::SetReady();

    uint size = 0;

    if (SPI1_::Receive(&size, 4))                                                               // ����� ������ ������������ ���������
    {
        Message first;              // ���� ��������� ������ ���������
        Message second;             // ���� ��������� ������ ���������

        if (first.AllocateMemory(size))
        {
            if (SPI1_::Receive(first.Data(), first.Size(), TIMEOUT))                            // ��������� ������
            {
                if (SPI1_::Transmit(&size, 4, TIMEOUT))                                         // ������� ��� ������
                {
                    if (SPI1_::Transmit(first.Data(), first.Size(), TIMEOUT))                   // � ������
                    {
                        if (SPI1_::Receive(&size, 4))
                        {
                            if (second.AllocateMemory(size))                                    // ������ ��� ��������� ����� ��������� � ���� �����
                            {
                                if (SPI1_::Receive(second.Data(), second.Size(), TIMEOUT))      // ��� � ������
                                {
                                    size = second.Size();

                                    if (SPI1_::Transmit(&size, 4, TIMEOUT))
                                    {
                                        if (SPI1_::Transmit(second.Data(), second.Size(), TIMEOUT))
                                        {
                                            if (second.IsEquals(&first))                        // ���������, ������� �� ��� �������� ���������
                                            {
                                                commands[first.Data()[0]].func(&first);         // �, ���� �������, ������� ��������� �� ���������
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    CPU::SetBusy();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Interface::SendData(Message *)
{
    CPU::SetBusy();

    Message message;

    if (CreateMessageForSend(&message))
    {
        Timer::PauseOnTime(2);

        CPU::SetReady();

        Transceiver::Transmit(&message);

        CPU::SetBusy();
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Interface::CreateMessageForSend(Message *message)
{
    if (freqForSend != MAX_UINT)
    {
        message->AllocateMemory(5);
        message->Put8(Command::FreqMeasure);
        message->Put32(freqForSend);

        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Interface::EnableChannel(Message *message)
{
    message->TakeByte();

    Chan ch = (Chan::E)message->TakeByte();

    bool enable = (message->TakeByte() == 1);
      
    Generator::EnableChannel(ch, enable);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Interface::SetPolarity(Message *message)
{
    message->TakeByte();

    Chan ch = (Chan::E)message->TakeByte();

    FPGA::SetPolarity(ch, message->TakeByte());
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Interface::LoadFormDDS(Message *message)
{
    message->TakeByte();

    Chan ch = (Chan::E)message->TakeByte();

    std::memcpy(FPGA::DataDDS(ch), message->Data() + 2, FPGA_NUM_POINTS * 2);

    Generator::SetFormWave(ch, Form::DDS);
}

//---------------------------------------------------------------------------------------------------------------------
void Interface::Test(Message *)
{
    std::srand(TIME_MS);
    
    Array array(400);
    
    for(uint i = 0; i < array.Size(); i++)
    {
        array.Data()[i] = (uint8)std::rand();
    }
    
    uint size = 1 + 4 + 4 + array.Size();
    
    // �������� ���������� ����
    SPI1_::Transmit(&size, 2);
    
    // � ������ �������� ���� �����
    
    uint8 buffer[9] = {Command::Test};
    
    BitSet32 bsSize(array.Size());
    bsSize.WriteToBuffer(buffer + 1);
    
    BitSet32 bsCRC(array.CRC32());
    bsCRC.WriteToBuffer(buffer + 5);
    
    SPI1_::Transmit(buffer, 9);
    
    SPI1_::Transmit(array.Data(), array.Size());
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Interface::SetKoeffCalibration(Message *msg)
{
    msg->TakeByte();

    Chan ch = (Chan::E)msg->TakeByte();

    static int16 * const values[] =
    {
        &CAL_AD9952_OFFSET_NEG(Chan::A),
        &CAL_AD9952_OFFSET_ZERO(Chan::A),
        &CAL_AD9952_AMPLITUDE(Chan::A),
        &CAL_DDS_MAX(Chan::A),
        &CAL_DDS_MIN(Chan::A)
    };

    int16 *koeff = values[msg->TakeByte()];

    koeff[ch] = (int16)msg->TakeByte();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Interface::SetManipulation(Message *msg)
{
    msg->TakeByte();

    Chan ch = (Chan::E)msg->TakeByte();

    AD9952::Manipulation::SetEnabled(ch, msg->TakeByte() != 0);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Interface::SetManipulationMode(Message *msg)
{
    msg->TakeByte();

    Chan ch = (Chan::E)msg->TakeByte();

    AD9952::Manipulation::SetType(ch, msg->TakeByte());
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Interface::SetStartMode(Message *msg)
{
    msg->TakeByte();

    Chan ch = (Chan::E)msg->TakeByte();

    StartMode mode = (StartMode)msg->TakeByte();

    FPGA::SetStartMode(ch, mode);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Interface::SetFormWave(Message *msg)
{
    msg->TakeByte();

    Chan ch = (Chan::E)msg->TakeByte();

    Form form = (Form::E)msg->TakeByte();

    Generator::SetFormWave(ch, form);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Interface::WriteRegister(Message *msg)
{
    msg->TakeByte();

    Register reg = (Register::E)msg->TakeByte();

    uint64 value = msg->TakeDoubleWord();

    switch (reg.value)
    {
    case Register::OffsetA:
        AD5697::SetOffset(Chan::A, (float)value);
        break;

    case Register::OffsetB:
        AD5697::SetOffset(Chan::B, (float)value);
        break;

    case Register::FreqMeterLevel:
        AD5697::SetFreqLevel((float)value);
        break;

    case Register::FreqMeterHYS:
        AD5697::SetFreqHysteresys((float)value);
        break;

    case Register::FPGA_RG0_Control:
        FPGA::WriteRegister(FPGA::RG::_0_Control, value);
        break;

    case Register::FPGA_RG1_Freq:
        FPGA::WriteRegister(FPGA::RG::_1_Freq, value);
        break;

    case Register::FPGA_RG2_Amplitude:
        FPGA::WriteRegister(FPGA::RG::_2_Amplitude, value);
        break;

    case Register::FPGA_RG3_RectA:
        FPGA::WriteRegister(FPGA::RG::_3_RectA, value);
        break;

    case Register::FPGA_RG4_RectB:
        FPGA::WriteRegister(FPGA::RG::_4_RectB, value);
        break;

    case Register::FPGA_RG5_PeriodImpulseA:
        FPGA::WriteRegister(FPGA::RG::_5_PeriodImpulseA, value);
        break;

    case Register::FPGA_RG6_DurationImpulseA:
        FPGA::WriteRegister(FPGA::RG::_6_DurationImpulseA, value);
        break;

    case Register::FPGA_RG7_PeriodImpulseB:
        FPGA::WriteRegister(FPGA::RG::_7_PeriodImpulseB, value);
        break;

    case Register::FPGA_RG8_DurationImpulseB:
        FPGA::WriteRegister(FPGA::RG::_8_DurationImpulseB, value);
        break;

    case Register::FPGA_RG9_FreqMeter:
        FPGA::WriteRegister(FPGA::RG::_9_FreqMeter, value);
        freqForSend = MAX_UINT;
        break;

    case Register::FPGA_RG10_Offset:
        FPGA::WriteRegister(FPGA::RG::_10_Offset, value);
        break;

    case Register::FreqMeter_Resist:
        FreqMeter::SetResist((FreqResist::E)value);
        break;

    case Register::FreqMeter_Couple:
        FreqMeter::SetCouple((FreqCouple::E)value);
        break;

    case Register::FreqMeter_Filtr:
        FreqMeter::SetFiltr((FreqFiltr::E)value);
        break;

    case Register::Number:
    case Register::Multiplexor1:
    case Register::Multiplexor2:
    default:
        break;
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Interface::ParameterValue(Message *msg)
{
    Command command = (Command::E)msg->TakeByte();

    Chan ch = (Chan::E)msg->TakeByte();
    
    Generator::SetParameter(ch, command, msg->TakeFloat());
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Interface::RunReset(Message *)
{
#ifndef WIN32

#define MAIN_PROGRAM_START_ADDRESS  (uint)0x8000000
    typedef void(*pFunction)();
    __disable_irq();
    pFunction JumpToApplication = (pFunction)(*(__IO uint *)(MAIN_PROGRAM_START_ADDRESS + 4));
    __enable_irq();
    JumpToApplication();

#endif
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Interface::ModeDebug(Message *)
{
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Interface::Empty(Message *)
{
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Interface::SendFrequency(uint value)
{
    freqForSend = value;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Interface::Send(void *buffer, uint size)
{
    SPI1_::Transmit(&size, 2);

    SPI1_::Transmit(buffer, size);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Interface::Data::Data(uint size, uint8 command) : data(0), size(0)
{
    Init(size);

    if (size)
    {
        data[0] = command;
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Interface::Data::~Data()
{
    Release();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Interface::Data::Init(uint _size)
{
    if (data)
    {
        delete data;
    }

    size = _size;

    if (size)
    {
        data = (uint8 *)std::malloc(size);

        return data != 0;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Interface::Data::Release()
{
    if (data)
    {
        delete data;
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Interface::Data::IsEmpty() const
{
    return data == 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8 *Interface::Data::GetData()
{
    return data;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint Interface::Data::GetSize() const
{
    return size;
}
