#include "defines.h"
#include "Generator/Generator_p.h"
#include "Generator/MathSupport.h"
#include "Generator/Parameters.h"
#include "Menu/Menu.h"
#include "Menu/Pages/Pages.h"
#include "Settings/Settings.h"
#include "Utils/Math.h"
#include <cstdio>


void Parameter::SetForm(Form *f)
{
    form = f;
}


bool Parameter::IsDouble() const
{
    return (kind == Double);
}


bool Parameter::IsComplex() const
{
    return (kind == Composite);
}


bool Parameter::IsChoice() const
{
    return (kind == Choice);
}


pString Parameter::Name() const
{
    return name[LANGUAGE];
}


void Parameter::SetParent(Parameter *p)
{
    parent = p;
}


Parameter *Parameter::GetParent()
{
    return parent;
}

bool Parameter::IsOpened() const
{
    return (parent != nullptr);
}


Form *Parameter::GetForm()
{
    return form;
}


void ParameterComposite::SetForm(Form *f)
{
    Parameter::SetForm(f);

    for (int i = 0; i < numParams; i++)
    {
        params[i]->SetForm(f);
    }
}


ParameterChoice *ParameterComposite::FindParameter(ParameterChoice::E p)
{
    for(int i = 0; i < numParams; i++)
    {
        Parameter *param = params[i];

        if(param->IsChoice())
        {
            ParameterChoice *parameter = static_cast<ParameterChoice *>(param);

            if(parameter->Type() == p)
            {
                return parameter;
            }
        }
    }

    return nullptr;
}


ParameterDouble *ParameterComposite::FindParameter(ParameterValueType::E p)
{
    for(int i = 0; i < numParams; i++)
    {
        Parameter *param = params[i];

        if(param->IsDouble())
        {
            ParameterDouble *parameter = static_cast<ParameterDouble *>(param);

            if(parameter->GetType() == p)
            {
                return parameter;
            }
        }
    }

    return nullptr;
}


pString ParameterDouble::GetMainUnits() const
{
    static const pString units[ParameterValueType::Count][2] =
    {
        {"��", "Hz"},   // Frequency
        {"�",  "s"},    // Period
        {"�",  "V"},    // Amplitude
        {"�",  "V"},    // Offset
        {"�",  "s"},    // Duration
        {"",   ""},     // DutyRatio
        {"",   ""},     // Phase
        {"�",  "s"},    // Delay
        {"�",  "s"},    // DurationRise
        {"�",  "s"},    // DurationFall
        {"�",  "s"},    // DurationStady
        {"",   ""},     // DutyFactor
        {"",   ""},     // ManipulationDuration
        {"",   ""},     // ManipulationPeriod
        {"",   ""},     // PacketPeriod
        {"",   ""},     // PacketNumber
        {"",   ""}      // Exit
    };

    return units[GetType()][LANGUAGE];
}


pString ParameterDouble::GetUnits(Order::E order) const
{
    if (order == Order::Count)
    {
        order = value.GetOrder();
    }

    static char units[10];

    std::strcpy(units, Order::Suffix(order));
    std::strcat(units, GetMainUnits());

    return units;
}


bool ParameterDouble::SetAndLoadValue(double val)
{
    if(!InRange(val))
    {
        return false;
    }

    value.FromDouble(val);

    PGenerator::SetParameter(this);

    return true;
}


bool ParameterDouble::SetAndLoadValue(DoubleValue val)
{
    if (!InRange(val))
    {
        return false;
    }

    value = val;

    PGenerator::SetParameter(this);

    return true;
}


pString ParameterComposite::ToString() const
{
    if(type == Manipulation)
    {
        static pCHAR valuesRU[2] =
        {
            " ����", " ���"
        };

        static pCHAR valuesEN[2] =
        {
            " Off", " On"
        };

        ParameterChoice *enabled = const_cast<ParameterComposite *>(this)->FindParameter(ParameterChoice::ManipulationEnabled);

        return LANG_IS_RU ? valuesRU[enabled->GetChoice()] : valuesEN[enabled->GetChoice()];
    }

    return "";
}


pString ParameterChoice::ToString() const
{
    return names[choice];
}


void ParameterChoice::NextChoice()
{
    Math::CircleIncrease(&choice, 0, NumChoices() - 1);

    Chan::E ch = form->GetWave()->GetChannel();

    if(type == ModeStart)
    {
        PGenerator::LoadStartMode(ch, choice);
    }
    else
    {
        PGenerator::TuneChannel(ch);
    }
}


bool ParameterChoice::SetAndLoadChoice(int ch)
{
    if(ch < 0 || ch >= NumChoices())
    {
        return false;
    }

    choice = ch;

    PGenerator::SetParameter(this);

    return true;
}


int ParameterChoice::GetChoice() const
{
    return 0;
}


bool ParameterChoice::DrawChoice(int, int) const
{
    return true;
}


ParameterManipulation::ParameterManipulation(Parameter **parameters) : ParameterComposite(ParameterComposite::Manipulation, "�����������", "Manipulation", parameters)
{

}


bool ParameterDouble::InRange(double val) const
{
    return (val >= min.ToDouble()) && (val <= max.ToDouble());
}


bool ParameterDouble::InRange(DoubleValue val) const
{
    return (val >= min && val <= max);
}


int ParameterChoice::NumChoices() const
{
    return 1;
}


ParameterDouble::ParameterDouble(ParameterValueType::E t, const char *nameRU, const char *nameEN, const DoubleValue &_min, const DoubleValue &_max, const DoubleValue &_value) :
    Parameter(Parameter::Double, nameRU, nameEN), tuner(this), type(t), min(_min), max(_max), value(_value)
{
}


static Order::E CalculateOrder(const ParameterDouble *param)
{
    DoubleValue value = param->GetValue();

    Order::E order = value.GetOrder();

    if (param->IsVoltage() && (value.Abs() == 0))
    {
        order = Order::Milli;
    }

    return order;
}


pString ParameterDouble::ToString() const
{
    static char buffer[30];

    std::strcpy(buffer, MathDoubleValue::GetIndicatedValue(this));
    std::strcat(buffer, " ");
    std::strcat(buffer, GetUnits(CalculateOrder(this)));

    return buffer;
}


pString ParameterDouble::ToString(DoubleValue val) const
{
    static char buffer[30];
    
    std::strcpy(buffer, val.ToString(val.GetOrder(), IsSigned()));
    std::strcat(buffer, " ");
    std::strcat(buffer, GetUnits());

    return buffer;
}


pString Order::Suffix(Order::E order)
{
    static const pString suf[Count][2] =
    {
        {"�", "M"},
        {"�",  "k"},
        {"",   ""},
        {"�",  "m"},
        {"��", "u"},
        {"�",  "n"}
    };

    return suf[order][LANGUAGE];
}


int Order::GetPow10(Order::E order)
{
    static const int pows[Count] = { 6, 3, 0, -3, -6, -9 };

    return pows[order];
}


void ParameterChoice::OnPressButtonTune()
{
    NextChoice();
}


void ParameterDouble::OnPressButtonTune()
{
    PageTuneParameter::SetParameter(this);
    Menu::SetAdditionPage(PageTuneParameter::self);
}


void ParameterComposite::OnPressButtonTune()
{
    form->OpenComplexParameter();
}


void Parameter::OnPressButtonTune()
{
    switch (kind)
    {
    case Double:    reinterpret_cast<ParameterDouble *>(this)->OnPressButtonTune();   break;
    case Composite: reinterpret_cast<ParameterComposite *>(this)->OnPressButtonTune(); break;
    case Choice:    reinterpret_cast<ParameterChoice *>(this)->OnPressButtonTune();  break;

    case Exit:
        break;
    }
}
