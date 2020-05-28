#include "defines.h"
#include "common/Common.h"
#include "Display/Painter.h"
#include "Display/Symbols.h"
#include "Display/Text.h"
#include "Display/Font/Font.h"
#include "Display/WaveGraphics.h"
#include "Generator/Parameters.h"
#include "Generator/Tuner.h"
#include "Generator/Wave.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Menu/Pages/Pages.h"
#include "Settings/Settings.h"
#include "Utils/Math.h"
#include "Utils/Stack.h"


// ����� ��� �������� �������� ������ � ������ ����������������� �����
class EnterBuffer
{
public:
    EnterBuffer() : stack(30), param(nullptr) { }
    void Prepare(ParameterValue *parameter);
    void Push(const Control &control);
    int Size() const;
    char At(const int i) const;
private:
    Stack<char> stack;
    ParameterValue *param;
};


class Cursor
{
public:
    Cursor() : timeInit(0U) { }
    void Init();
    void Draw(int x, int y);
private:
    uint timeInit;
};


static EnterBuffer enterBuffer;     // ����� ����� ������� ������� ������ � ������ �����
static Cursor cursor;               // �������� ������ ��� ������ ����������������� �����


void Cursor::Init()
{
    timeInit = TIME_MS;
}


void Cursor::Draw(int x, int y)
{
    uint time = TIME_MS - timeInit;

    if ((time % 1000) < 500)
    {
        Primitives::Rectangle(10, 3).Fill(x, y);
    }
}


void EnterBuffer::Prepare(ParameterValue *parameter)
{
    stack.Clear();
    param = parameter;
}


void EnterBuffer::Push(const Control &control)
{
    stack.Push(Key(control.key).ToChar());
}


int EnterBuffer::Size() const
{
    return stack.Size();
}


char EnterBuffer::At(const int i) const
{
    return stack[i];
}


Indicator::Indicator(TunerDisplay *_display) : indexHighlight(0), display(_display)
{
}


int Indicator::Draw(int x, int y)
{
    static const int dx = 12;

    int pos = 0;

    while (digits[pos] != '\0')
    {
        Color color = CalculateColor(pos);

        x += AdditionShiftForDigit(pos);

        Char(digits[pos]).Draw(x, y, color);

        if (pos == IndexHighlightReal())
        {
            HighlightSymbol(x, y);
        }

        x += dx;

        if (CommaInPosition(pos))
        {
            x -= 7;
        }

        pos++;
    }

    return x;
}


Color Indicator::CalculateColor(int pos)
{
    if (!digits[pos].IsNumber())
    {
        return Color::FILL;
    }

    if (digits[pos] == '0')
    {
        if (AllNumbersOfLeftIsZero(pos))
        {
            return Color::GRAY_25;
        }
        if (AllNumberOfRightIsZero(pos) && (PositionComma() < pos))
        {
            return Color::GRAY_25;
        }
    }
    return Color::FILL;
}


bool Indicator::AllNumbersOfLeftIsZero(int pos)
{
    for (int i = 0; i < pos; i++)
    {
        if (digits[i].IsNumber() && digits[i] != '0')
        {
            return false;
        }
    }

    return true;
}


bool Indicator::AllNumberOfRightIsZero(int pos)
{
    for (int i = pos + 1; (i < MAX_NUM_DIGITS) && !digits[i].IsEmpty(); i++)
    {
        if (digits[i].IsNumber() && digits[i] != '0')
        {
            return false;
        }
    }

    return true;
}


int Indicator::AdditionShiftForDigit(int pos)
{
    static const int d = 5;

    int posComma = PositionComma();

    for (int i = 1; i < 5; i++)
    {
        if (pos == (posComma - i * 3))
        {
            return d;
        }
        if (pos == (posComma + i * 3 + 1))
        {
            return d;
        }
    }

    return 0;
}


void Indicator::HighlightSymbol(int x, int y)
{
    Font::StoreAndSet(TypeFont::_7);

    x += 2;

    Char(Ideograph::_7::FillDown).Draw(x, y - 7, Color::WHITE);

    Char(Ideograph::_7::FillUp).Draw(x, y + 19);

    Font::Restore();
}


int Indicator::IndexHighlightReal()
{
    return (indexHighlight < PositionComma()) ? indexHighlight : (indexHighlight + 1);
}


bool Indicator::OnControlKey(const Control control) //-V801
{
    if (control.IsRotate() || control.IsCursors())
    {
        if (control.Is(Key::Left))
        {
            HighlightToLeft();
        }
        else if (control.Is(Key::Right))
        {
            HighlightToRight();
        }
        else if (control.Is(Key::RotateLeft))
        {
            DecreaseInPosition(IndexHighlightReal());
        }
        else if (control.Is(Key::RotateRight))
        {
            IncreaseInPosition(IndexHighlightReal());
        }

        return true;
    }

    return false;
}


bool Indicator::IsSigned()
{
    return !digits[0].IsNumber();
}


void Indicator::IncreaseInPosition(int pos)
{
    ParameterValue *param = display->GetTuner()->GetParameter();

    DoubleValue value = param->GetValue();

    if (!ChangeSign(&value, pos))
    {
        DoubleValue step = StepPosition(pos);

        value.Add(step);

        LIMITATION_ABOVE(value, param->GetMax());
    }

    display->Init(value);
}


void Indicator::DecreaseInPosition(int pos)
{
    ParameterValue *param = display->GetTuner()->GetParameter();

    DoubleValue value = param->GetValue();

    if (!ChangeSign(&value, pos))
    {
        DoubleValue step = StepPosition(pos);

        value.Sub(step);

        LIMITATION_BELOW(value, param->GetMin());
    }

    display->Init(value);
}


bool Indicator::ChangeSign(DoubleValue *value, int pos)
{
    if (IsSigned() && (pos == 0))
    {
        if (value->Abs() != 0)
        {
            value->SetSign(-value->Sign());
        }

        return true;
    }

    return false;
}


void Indicator::HighlightToLeft()
{
    do
    {
        Math::CircleDecrease(&indexHighlight, 0, NumberHighligthingDigits());
    } while (CommaInPosition(IndexHighlightReal()));

}


void Indicator::HighlightToRight()
{
    do
    {
        Math::CircleIncrease(&indexHighlight, 0, NumberHighligthingDigits());
    } while (CommaInPosition(IndexHighlightReal()));
}


int Indicator::NumberHighligthingDigits()
{
    int result = 0;

    for (int i = 0; !digits[i].IsEmpty(); i++)
    {
        if (digits[i].IsNumber() || digits[i].IsSigned())
        {
            result++;
        }
    }

    return result;
}


bool Indicator::CommaInPosition(int pos)
{
    return (digits[pos] == Digit::COMMA);
}


int Indicator::PositionComma()
{
    for (int i = 0; i < MAX_NUM_DIGITS; i++)
    {
        if (digits[i] == Digit::COMMA)
        {
            return i;
        }
    }

    return -1;
}


bool Indicator::FirstSignedDigitInPosition(int pos)
{
    for (int i = 0; digits[i] != '\0'; i++)
    {
        if (digits[i].IsNumber())
        {
            return (i == pos);
        }
    }

    return false;
}


int Indicator::FindPositionLeftDigit(int pos)
{
    if (FirstSignedDigitInPosition(pos))
    {
        return -1;
    }

    do 
    {
        pos--;
    } while (!digits[pos].IsNumber());

    return pos;
}


char *Indicator::GetStringDigits() const
{
    static char result[MAX_NUM_DIGITS];

    int i = -1;

    do 
    {
        i++;
        result[i] = digits[i];
    } while (result[i] != '\0');

    return result;
}


DoubleValue Indicator::StepPosition(int pos)
{
    int posAboutComma = pos - PositionComma();          // ������� ������� ������������ �����

    if (posAboutComma < 0)
    {
        posAboutComma++;
    }

    DoubleValue step("1.0");

    Order::E order = display->GetTuner()->GetParameter()->GetValue().GetOrder();

    step.MulPow10(Order::GetPow10(order) - posAboutComma);
     
    return step;
}


void Indicator::InitHighlight()
{
    indexHighlight = PositionComma() - 1;
}


TunerDisplay::TunerDisplay(Tuner *_tuner) : tuner(_tuner), indicator(this), mode(Correction)
{
}


void TunerDisplay::Draw()
{
    Chan ch = tuner->GetParameter()->GetForm()->GetWave()->GetChannel();

    int x = WaveGraphics::X();
    int y = WaveGraphics::Y(ch.GetInverse());

    Primitives::Rectangle(WaveGraphics::Width(), WaveGraphics::Height()).DrawFilled(x, y, Color::BLUE_10, Color::WHITE);

    Font::StoreAndSet(TypeFont::_GOSTB20);

    DrawTitle(x, y, WaveGraphics::Width());

    if (mode == Entering)
    {
        DrawEnteringMode(x, y + 50, WaveGraphics::Width());
    }
    else
    {
        x = DrawValue(x, y + 50);

        DrawUnits(x, y + 50);
    }

    Font::Restore();
}


void TunerDisplay::DrawTitle(int x, int y, int width)
{
    String(tuner->GetParameter()->Name(LANGUAGE)).DrawInCenterRect(x, y, width, 30, Color::WHITE);
}


void TunerDisplay::DrawUnits(int x, int y)
{
    Font::SetUpperCase(false);

    String(tuner->GetParameter()->GetUnits(LANGUAGE)).Draw(x + 7, y, Color::WHITE);

    Font::SetUpperCase(true);
}


int TunerDisplay::DrawValue(int x, int y)
{
    return indicator.Draw(x + 20, y);
}


bool TunerDisplay::OnControlKey(const Control &control)
{
    if (control.IsEntering())
    {
        return OnEnteringKey(control);
    }

    return indicator.OnControlKey(control);
}


bool TunerDisplay::OnEnteringKey(const Control &control)
{
    if (control.IsEntering())
    {
        if (mode == Correction)
        {
            SetModeEntering();
        }

        enterBuffer.Push(control);
        cursor.Init();

        return true;
    }

    return false;
}


void TunerDisplay::DrawEnteringMode(int x, int y, int)
{
    for (int i = 0; i < enterBuffer.Size(); i++)
    {
        Char(enterBuffer.At(i)).Draw(x, y);

        x += (enterBuffer.At(i) == '.') ? 10 : 14;
    }

    cursor.Draw(x, y + Font::GetHeight());
}


void TunerDisplay::SetModeEntering()
{
    PageTuneParameter::SetModeEntering();
    mode = Entering;
    enterBuffer.Prepare(tuner->GetParameter());
}


void TunerDisplay::Init()
{
    MathParameterValue::SetParameterValue(tuner->GetParameter());

    for (int i = 0; i < Indicator::MAX_NUM_DIGITS; i++)
    {
        indicator.digits[i].Set('\0');
    }

    int before = MathParameterValue::GetNumberDigitsBeforeComma();

    indicator.digits[before].Set(Digit::COMMA);

    FillDigitsIntegerPart();

    FillDigitsFractPart();
}


void TunerDisplay::FillDigitsIntegerPart()
{
    int before = MathParameterValue::GetNumberDigitsBeforeComma();
    ParameterValue *param = tuner->GetParameter();
    DoubleValue value = param->GetValue();

    int pos = before - 1;                               // ������ � ���� ������� ����� ��������� ���������� �����

    if (param->IsSigned())
    {
        indicator.digits[0].Set((value < DoubleValue("0.0")) ? '-' : '+');
    }

    for (int i = 0; i < before; i++)
    {
        indicator.digits[pos].Set(MathFloatValue::GetChar(value, i));
        pos--;

        if (param->IsSigned() && (pos == 0))
        {
            break;
        }
    }
}


void TunerDisplay::FillDigitsFractPart()
{
    int before = MathParameterValue::GetNumberDigitsBeforeComma();
    int after = MathParameterValue::GetNumberDigitsAfterComma();
    DoubleValue value = tuner->GetParameter()->GetValue();

    int pos = before + 1;                                   // ������ � ��� ������� ����� ���������� �������� ����� �������

    for (int i = 0; i < after; i++)
    {
        indicator.digits[pos].Set(MathFloatValue::GetChar(value, -i - 1));
        pos++;
    }
}


void TunerDisplay::Init(DoubleValue value)
{
    tuner->GetParameter()->SetAndLoadValue(value);
    Init();
}


Tuner::Tuner(ParameterValue *_param) : param(_param), display(this)
{
}


void Tuner::Init()
{
    display.Init();

    display.indicator.InitHighlight();
}


void Tuner::Draw()
{
    display.Draw();
}


bool Tuner::OnControlKey(const Control control) //-V801
{
    if (control.IsCursors() || control.IsRotate() || control.IsEntering())
    {
        return display.OnControlKey(control);
    }

    return false;
}
