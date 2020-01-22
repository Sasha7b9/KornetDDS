#include "defines.h"
#include "Display/Font/Font.h"
#include "Generator/Parameters.h"
#include "Generator/ParameterPainter.h"
#include "Generator/Signals.h"
#include "Generator/Wave.h"
#include <cstring>


Parameter *ParameterPainter::parameter = nullptr;


pString ParameterPainter::DigitsWithSign()
{
    if(parameter->IsValue())
    {
        return DigitsWithSignValue();
    }

    return "";
}


pString ParameterPainter::Units(Language::E lang)
{
    if(parameter->IsValue())
    {
        return UnitsValue(lang);
    }

    return "";
}


pString ParameterPainter::DigitsWithSignValue()
{
    ParameterValue *value = static_cast<ParameterValue *>(parameter);

    if(value->Type() == ParameterValue::Offset)
    {
        return DigitsWithSignOffset();
    }

    return "";
}


pString ParameterPainter::UnitsValue(Language::E lang)
{
    ParameterValue *value = static_cast<ParameterValue *>(parameter);

    if(value->Type() == ParameterValue::Offset || value->Type() == ParameterValue::Amplitude)
    {
        return (lang == Language::RU) ? "�" : "V";
    }

    return "";
}


pString ParameterPainter::DigitsWithSignOffset()
{
    //ParameterValue *value = static_cast<ParameterValue *>(parameter);

    FloatValue value = static_cast<ParameterValue *>(parameter)->value;

    const int NUM_SYMBOLS = 1 + 1 + 1 + 3 + 1;

    static char buffer[NUM_SYMBOLS];

    buffer[0] = (value.Sign() < 0) ? '-' : '+';

    buffer[NUM_SYMBOLS - 1] = '\0';

    buffer[1] = MathFloatValue::GetChar(value, 0);

    buffer[2] = ',';

    buffer[3] = MathFloatValue::GetChar(value, -1);

    buffer[4] = MathFloatValue::GetChar(value, -2);

    buffer[5] = MathFloatValue::GetChar(value, -3);

    return buffer;
}


pString MathFloatValue::GetStringValue(const FloatValue &value, bool sign, int numDigits, Order::E *order, int posFirst)
{
    static char buffer[20];

    buffer[numDigits + (sign ? 2 : 1)] = '\0';

    int position = (posFirst == 255) ? GetPositionFirstDigit(value) : posFirst;

    int posComma = PositionComma(position, order);

    if(sign)
    {
        buffer[0] = value.Sign() == 1 ? '+' : '-';
    }

#define POS(i) (sign ? (i + 1) : i)

    for(int i = 0; i <= numDigits; i++)
    {
        if(i == posComma)
        {
            buffer[POS(i)] = '.';
        }
        else
        {
            buffer[POS(i)] = GetChar(value, position);
            position--;
        }
    }

    return buffer;

}


int MathFloatValue::GetPositionFirstDigit(const FloatValue &_value)
{
    FloatValue value = _value;
    value.SetSign(1);

    int result = 0;

    if(value.Integer() > 0)
    {
        int whole = value.Integer();        // ����� ����� �����

        while(whole > 9)
        {
            whole /= 10;
            result++;
        }
    }
    else
    {
        int fract = value.FractNano();

        if(fract == 0)
        {
            return 0;
        }

        do
        {
            result--;
            fract *= 10;
        } while(fract < (1000 * 1000 * 1000));
    }

    return result;
}


int MathFloatValue::GetDigit(const FloatValue &_value, int position)
{
    FloatValue value = _value;
    value.SetSign(1);

    if(position < 0)
    {
        int divider = 100 * 1000 * 1000;       /// �� ��� ����� ����� ������ ���������� ����������

        int fract = value.FractNano();

        while(position < -1)
        {
            fract %= divider;
            divider /= 10;
            position++;
        }

        return fract / divider;
    }
    else
    {
        int whole = value.Integer();

        while(position > 0)
        {
            whole /= 10;
            position--;
        }

        return (whole % 10);
    }
}


char MathFloatValue::GetChar(const FloatValue &value, int postition)
{
    return static_cast<char>(GetDigit(value, postition) | 0x30);
}


int MathFloatValue::PositionComma(int posFirstDigit, Order::E *order)
{
    if(posFirstDigit > 5)
    {
        *order = Order::Mega;
    }
    else if(posFirstDigit > 2)
    {
        *order = Order::Kilo;
    }
    else if(posFirstDigit > -1)
    {
        *order = Order::One;
    }
    else if(posFirstDigit > -4)
    {
        *order = Order::Milli;
    }
    else if(posFirstDigit > -7)
    {
        *order = Order::Micro;
    }
    else
    {
        *order = Order::Nano;
    }

    int result = posFirstDigit - 5;

    while(result < 1)
    {
        result += 3;
    }

    return result;
}


void ParameterPainterSupporting::SetParameter(Parameter *param)
{
    parameter = param;

    ParameterPainter::SetPatameter(parameter);
    buffer[0] = '\0';
    std::strcpy(buffer, ParameterPainter::DigitsWithSign());
    std::strcat(buffer, ParameterPainter::Units());

    int length = X(NumSymbols() + 1);

    x0 = Width() / 2 - length / 2 + X0();
}


uint ParameterPainterSupporting::NumSymbols() const
{
    return std::strlen(buffer);
}


int ParameterPainterSupporting::X(uint pos) const
{
    int delta = 2;

    int result = 0;

    for(uint i = 0; i < pos; i++)
    {
        result += Font::GetLengthSymbol(buffer[i]) + delta;
    }

    if(pos >= PositionFirstUnit())
    {
        result += 5;
    }

    return x0 + result;
}


char ParameterPainterSupporting::Symbol(uint pos) const
{
    return buffer[pos];
}


uint ParameterPainterSupporting::PositionFirstUnit() const
{
    for(uint i = 0; i < NumSymbols(); i++)
    {
        if(static_cast<uint8>(Symbol(i)) >= static_cast<uint8>('A'))
        {
            return i;
        }
    }

    return static_cast<uint>(-1);
}


int ParameterPainterSupporting::X0() const
{
    return Wave::Graphics::X();
}


int ParameterPainterSupporting::Width() const
{
    return Wave::Graphics::Width() - 1;
}


int ParameterPainterSupporting::Y0() const
{
    return Wave::Graphics::Y(Chan::GetInverse(CURRENT_CHANNEL)) + 1;
}
