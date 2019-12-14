#include "defines.h"
#include "FrequencyMeter/FrequencyMeter.h"
#include "Generator/GeneratorPanel.h"
#include "Generator/Signals.h"
#include "Menu/Pages/Include/PageSignals.h"
#include "Settings/CalibrationSettings.h"


/// ���������� ������ ��� ������ ���������� ��������
static void SetParameter(Chan::E ch, KoeffCal::E koeff)
{
    if (koeff == KoeffCal::AD9952_ZERO)
    {
        Generator::SetOffset(ch, 0.0F);
    }
    else if (koeff == KoeffCal::AD9952_NEG)
    {
        Generator::SetOffset(ch, -5.0F);
    }
    else if (koeff == KoeffCal::AD9952_POS)
    {
        Generator::SetOffset(ch, +5.0F);
    }
    else if (koeff == KoeffCal::AD9952_AMPL)
    {
        Generator::SetAmplitude(ch, 10.0F);
    }
    else if (koeff == KoeffCal::FREQ_LEVEL_TRIG)
    {
        FrequencyMeter::LoadLevel();
    }
    else if (koeff == KoeffCal::DDS_MAX)
    {
        Generator::SetAmplitude(ch, 10.0F);
    }
    else if (koeff == KoeffCal::DDS_MIN)
    {
        Generator::SetAmplitude(ch, -10.0F);
    }
    else if (koeff == KoeffCal::DDS_OFFSET)
    {
        Generator::SetOffset(ch, 0.0F);
    }
    else
    {
        // ����� ������
    }
}


void PageSignals::PageCalibration::OnPress_OffsetAD9952(Chan::E ch, bool enter, KoeffCal::E koeff)
{
    if (enter)
    {
        Generator::SetAmplitude(ch, 0.0F);
        SetParameter(ch, koeff);
    }
}


void PageSignals::PageCalibration::OnPress_AmplitudeAD9952(Chan::E ch, bool enter, KoeffCal::E koeff)
{
    if (enter)
    {
        Generator::SetOffset(ch, 0.0F);
        SetParameter(ch, koeff);
    }
}


void PageSignals::PageCalibration::WriteKoeffCal(Chan::E ch, KoeffCal::E koeff)
{
    Message::Calibrate(static_cast<uint8>(ch), static_cast<uint8>(koeff)).Transmit();

    setCal.Save();

    SetParameter(ch, koeff);
}


void PageSignals::PageCalibration::OnPress_DDS(Chan::E ch, bool enter, KoeffCal::E koeff)
{
#define MAX_VALUE   (((uint64)(8191) << 14) + 8191)
#define WORK_VALUE  (((uint64)(16383) << 14) + 8191)

    static const Register::E registers[Chan::Count] =
    {
        Register::FPGA_RG3_RectA, Register::FPGA_RG4_RectB
    };

    if (enter)
    {
        for (int8 i = 0; i < WAVE(ch).NumberOfForms(); i++)
        {
            Form *form = WAVE(ch).GetForm(i);
            if (form->Is(Form::Meander))
            {
                WAVE(ch).SetIndexForm(i);
                Generator::TuneChannel(ch);
                break;
            }
        }

        if (koeff == KoeffCal::DDS_OFFSET)
        {
            Generator::SetAmplitude(ch, 0.0F);
        }
        else if(koeff == KoeffCal::DDS_MAX || koeff == KoeffCal::DDS_MIN)
        {
            // 1. �������� � RG::3 (RG::4) ��������, ��������������� ���������

            Generator::LoadRegister(registers[ch], MAX_VALUE);
        }
        else
        {
            // ����� ������
        }
    }
    else
    {
        if (koeff == KoeffCal::DDS_MAX || koeff == KoeffCal::DDS_MIN)
        {
            // 1. �������� � RG::3 (RG::4) ������� ��������

            Generator::LoadRegister(registers[ch], WORK_VALUE);
        }
    }
}
