#include "Log.h"
#include "globals.h"
#include "Display/Painter.h"
#include "Display/Text.h"
#include "Generator/Generator.h"
#include "Utils/Math.h"
#include "Utils/StringUtils.h"
#include "Settings/Settings.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CURRENT_POS         (hightLightDigit)
#define CURRENT_DIGIT       (inputBuffer[CURRENT_POS])
#define DIGIT(num)          (inputBuffer[num])
#define POS_COMMA           (posComma)
#define IN_NUM_LOCK_MODE    (numLockMode)


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Wave::Form      InputWindow::Struct::form    = Wave::Form::Sine;
Wave::Parameter InputWindow::Struct::m_param = Wave::Parameter::Amplitude;
Chan            InputWindow::Struct::ch      = Chan::A;

#define SIZE_INPUT_BUFFER_IWS 17
static char m_inputBuffer[SIZE_INPUT_BUFFER_IWS];


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void InputWindow::Struct::Fill(Chan ch_, Wave::Form form_, Wave::Parameter param_)
{
    ch = ch_;
    form = form_;
    m_param = param_;
    numLockMode = false;

    memset(m_inputBuffer, 0, SIZE_INPUT_BUFFER_IWS);

	for (int i = 0; i < NUM_DIGITS; i++)
	{
		inputBuffer[i] = PARAMETER_DIG(ch, form, m_param, i);
	}
	for (int i = NUM_DIGITS - 1; i > 0; --i)
	{
		if (inputBuffer[i] == 0)
		{
			inputBuffer[i] = '0';
		}
		else
		{
			break;
		}
	}

	hightLightDigit = inputBuffer[NUM_DIGITS - 1] == '.' ? NUM_DIGITS - 2 : NUM_DIGITS - 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void InputWindow::Struct::KeyLeft()
{
	if (hightLightDigit > 0)
	{
		if (hightLightDigit == 1 && inputBuffer[0] == '.')
		{
			return;
		}
		--hightLightDigit;
		if (inputBuffer[hightLightDigit] == '.')
		{
            InputWindow::Struct::KeyLeft();
		}
	}
    else
    {
        if (!OnlyOneRigthDigit() && posComma != NUM_DIGITS - 1)
        {
            ShiftToRight();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void InputWindow::Struct::KeyRight()
{
	if (hightLightDigit < NUM_DIGITS - 1)
	{
		if (hightLightDigit == NUM_DIGITS - 2 && inputBuffer[NUM_DIGITS - 1] == '.')
		{
			return;
		}
		++hightLightDigit;
		if (inputBuffer[hightLightDigit] == '.')
		{
            InputWindow::Struct::KeyRight();
		}
	}
    else if(inputBuffer[0] == '0')
    {
        ShiftToLeft();
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void InputWindow::Struct::RegLeft()
{
    if(DecreaseDigit(hightLightDigit))
    {
        if (TUNE_FULL)
        {
            SendToGenerator();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void InputWindow::Struct::RegRight()
{
    // ��������� ��������
    for (int i = 0; i < NUM_DIGITS; i++)
    {
        prevBuffer[i] = inputBuffer[i];
    }
    prevPosComma = posComma;

    if(IncreaseDigit(hightLightDigit))
    {
        if(ValueBeforeComma() > 999)
        {
            IncreaseOrder();
        }
        if(TUNE_FULL)
        {
            SendToGenerator();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
char *InputWindow::Struct::StringValue()
{
    static char buffer[20];
    buffer[0] = '\0';

    for (int i = 0; i < NUM_DIGITS; i++)
    {
        char str[2] = {0, 0};
        str[0] = inputBuffer[i];
        strcat(buffer, str);
        if (posComma == i)
        {
            str[0] = '.';
            strcat(buffer, str);
        }
    }

    return buffer;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool InputWindow::Struct::IncreaseDigit(int num)
{
    if (num < 0 || num >= NUM_DIGITS)
    {
        return false;
    }

    Struct temp = *this;

    if (All9LeftWithThis(num))
    {
        ShiftToRight();
        ++hightLightDigit;
        IncreaseDigit(hightLightDigit);
    }
    else
    {
        DIGIT(num)++;
        if (DIGIT(num) > '9')
        {
            DIGIT(num) = '0';
            if(!IncreaseDigit(DIGIT(num - 1) == '.' ? num - 2 : num - 1))
            {
                *this = temp;
                return false;
            }
        }
    }

    if(Value() < min || Value() > max)
    {
        *this = temp;
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool InputWindow::Struct::DecreaseDigit(int num)
{
    if (num < 0 || num >= NUM_DIGITS)
    {
        return false;
    }

    if (All0LeftWithThis(num) || Only1InThis(num))
    {
        return false;
    }

    Struct temp = *this;

    DIGIT(num)--;
    if (DIGIT(num) < '0')
    {
        DIGIT(num) = '9';
        DecreaseDigit(DIGIT(num - 1) == '.' ? num - 2 : num - 1);
    }

    if(Value() < min || Value() > max)
    {
        *this = temp;
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool InputWindow::Struct::All0LeftWithThis(int num)
{
    for (int i = num; i >= 0; i--)
    {
        if (DIGIT(i) != '0')
        {
            return false;
        }
    }
    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool InputWindow::Struct::All9LeftWithThis(int num)
{
    for (int i = num; i >= 0; i--)
    {
        if (DIGIT(i) != '9' && DIGIT(i) != '.')
        {
            return false;
        }
    }
    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool InputWindow::Struct::Only1InThis(int num)
{
    if (DIGIT(num) != '1')
    {
        return false;
    }

    for (int i = 0; i < num; i++)
    {
        if (DIGIT(i) != '0')
        {
            return false;
        }
    }

    for (int i = num + 1; i < NUM_DIGITS; i++)
    {
        if (DIGIT(i) != '0')
        {
            return false;
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void InputWindow::Struct::ShiftToRight()
{
    for (int i = NUM_DIGITS - 2; i >= 0; i--)
    {
        DIGIT(i + 1) = DIGIT(i);
    }
    DIGIT(0) = '0';
    ++POS_COMMA;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void InputWindow::Struct::ShiftToLeft()
{
    for (int i = 1; i < NUM_DIGITS; i++)
    {
        DIGIT(i - 1) = DIGIT(i);
    }
    DIGIT(NUM_DIGITS - 1) = '0';
    --POS_COMMA;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
pString Order::Name() const
{
    static const char *names[Order::Number][2] =
    {
        {"�",   "n"},
        {"��",  "u"},
        {"�",   "m"},
        {"",    ""},
        {"�",   "k"},
        {"�",   "M"}
    };

    return names[value][LANG];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
const char *NameUnit(char buffer[10], Order order, Wave::Parameter parameter)
{
    static const char *names[][2] =
    {
        {"��",  "Hz"},
        {"�",   "s"},
        {"�",   "V"},
        {"�",   "V"},
        {"�",  "s"},
        {"",    ""},
        {"o",  "o"},
        {"�",  "s"}
    };

    sprintf(buffer, "%s%s", order.Name(), names[parameter][LANG]);

    return buffer;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool InputWindow::Struct::OnlyOneRigthDigit()
{
    char digitLast = DIGIT(NUM_DIGITS - 1);

    if (digitLast != '0' && All0LeftWithThis(NUM_DIGITS - 2))
    {
        return true;
    }
    return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int InputWindow::Struct::ValueBeforeComma()
{
    int lowPos = posComma;     // ������� ���� �����

    int retValue = 0;

    int pow = 1;

    for (int i = lowPos; i >= 0; i--)
    {
        retValue += (0x0f & inputBuffer[i]) * pow;
        pow *= 10;
    }

    return retValue;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
float InputWindow::Struct::ValueAfterComma()
{
    int retValue = 0;
    int pow = 1;
    for (int i = NUM_DIGITS - 1; i > posComma; i--)
    {
        char digit = inputBuffer[i];
        digit &= 0x0f;
        retValue += digit * pow;
        pow *= 10;
    }

    return 1.0f / pow * retValue;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void InputWindow::Struct::IncreaseOrder()
{
    if (order < Order::Number - 1)
    {
        order = (Order)(Order::E)((uint)order + 1);

        POS_COMMA -= 3;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
float InputWindow::Struct::Value()
{
    float value = ValueBeforeComma() + ValueAfterComma();

    if (order == Order::Nano)
    {
        return value * 1e-9f;
    }
    if (order == Order::Micro)
    {
        return value * 1e-6f;
    }
    if (order == Order::Milli)
    {
        return value * 1e-3f;
    }
    if (order == Order::Kilo)
    {
        return value * 1e3f;
    }
    if (order == Order::Mega)
    {
        return value * 1e6f;
    }

    return value;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void InputWindow::Struct::SaveValue()
{
    if (IN_NUM_LOCK_MODE)
    {
        IN_NUM_LOCK_MODE = false;

        FillFromInputBuffer();
    }

    SendToGenerator();

    ADDITION_PAGE = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void InputWindow::Struct::RestoreValue()
{
    for (int i = 0; i < NUM_DIGITS; i++)
    {
        DIGIT(i) = prevBuffer[i];
    }
    posComma = prevPosComma;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void InputWindow::Struct::PressKey(Control key)
{
    struct StrControl
    {
        Control control;
        char    symbol;
    };

    static const StrControl command[] =
    {
        {Control::B_1, '1'}, {Control::B_2, '2'}, {Control::B_3, '3'}, {Control::B_4, '4'}, {Control::B_5, '5'},
        {Control::B_6, '6'}, {Control::B_7, '7'}, {Control::B_8, '8'}, {Control::B_9, '9'}, {Control::B_0, '0'}, {Control::B_Dot, '.'},
        {Control::B_None, '.'}
    };

    if (!IN_NUM_LOCK_MODE)
    {
        IN_NUM_LOCK_MODE = true;
        m_inputBuffer[0] = 0;
    }

    if (strlen(m_inputBuffer) < SIZE_INPUT_BUFFER_IWS - 1)
    {
        int i = 0;
        while (command[i].control != Control::B_None)
        {
            if (command[i].control == key)
            {
                int length = (int)strlen(m_inputBuffer);
                m_inputBuffer[length] = command[i].symbol;
                m_inputBuffer[length + 1] = 0;
                break;
            }
            i++;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void InputWindow::Struct::DrawInputField(int x, int y)
{
    int width = 230;
    int height = 60;

    Painter::FillRegion(x, y, width, height, Color::BACK);
    Painter::DrawRectangle(x, y, width, height, Color::FILL);

    x += 8;
    y += 19;

    int i = 0;

    while (m_inputBuffer[i])
    {
        x = Text::DrawBigChar(x, y, 3, m_inputBuffer[i]);
        x += 2;
        ++i;
    }

    Painter::FillRegion(270, 30, 45, 100, Color::BACK);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void InputWindow::Struct::SendToGenerator()
{
    PARAMETER(ch, form, m_param) = *this;

    if (m_param == Wave::Parameter::Delay)
    {
        /*
        PARAMETER(Chan::B, Wave::Form(Wave::Form::Impulse), Wave::Parameter::Frequency) = 
            PARAMETER(Chan::B, Wave::Form(Wave::Form::Impulse), Wave::Parameter::Frequency);
        float frequency = PARAMETER(Chan::A, Wave::Form(Wave::Form::Impulse), Wave::Parameter::Frequency).Value();
        Generator::SetParameter(Chan::B, Wave::Parameter::Frequency, frequency);

        float value = PARAMETER(ch, form, m_param).Value();
        Generator::SetParameter(ch, m_param, value);
        */
    }
    else
    {
        float value = PARAMETER(ch, form, m_param).Value();
        Generator::SetParameter(ch, m_param, value);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void InputWindow::Struct::FillFromInputBuffer()
{
    if (m_param == Wave::Parameter::Duration || m_param == Wave::Parameter::Delay)
    {
        order = Order::Micro;
    }
    else
    {
        order = Order::One;
    }

    if (SU::FindSymbol(m_inputBuffer, '.') == -1)             // ���� ����� ����
    {
        m_inputBuffer[strlen(m_inputBuffer)] = '.';         // �� ������ � ������ ������������ ����
        m_inputBuffer[strlen(m_inputBuffer) + 1] = 0;       // � ���������� ���� ������
    }
    else
    {
        while ((int)fabs(atof(m_inputBuffer)) == 0)     // ���� ����� ����� ����� � inputBuffer == 0
        {
            // �������� ������� �� ��� ����� ������
            int pos = SU::FindSymbol(m_inputBuffer, '.');

            for (int i = pos; i < pos + 3; i++)
            {
                m_inputBuffer[i] = m_inputBuffer[i + 1];
                if(m_inputBuffer[i] == 0)
                {
                    m_inputBuffer[i] = '0';
                }
            }
            m_inputBuffer[pos + 3] = '.';
            if(m_inputBuffer[pos + 4] == 0)
            {
                m_inputBuffer[pos + 4] = '0';
            }

            order = (Order::E)((uint)order - 1);
        }
    }

    if (sign != SignValue::None)
    {
        sign = (atof(m_inputBuffer) >= 0.0) ? SignValue::Plus : SignValue::Minus;
    }

    hightLightDigit = NUM_DIGITS - 1;

    while ((int)fabs(atof(m_inputBuffer)) > 999)    // ���� ����� ����� ����� � inputBuffer > 999
    {
        // �������� ������� �� ��� ����� �����
        int pos = SU::FindSymbol(m_inputBuffer, '.');
        
        for (int i = pos; i > pos - 3; i--)         // �������� ��� ������� ����� �� ����� �� ���� ������� ������
        {
            m_inputBuffer[i] = m_inputBuffer[i - 1];
        }

        m_inputBuffer[pos - 3] = '.';               // � ������ ����� ����� �� ���� ������

        order = (Order::E)((uint)order + 1); // � ����������� ������� �� ��� �������
    }

    // � ���� ����� ����� ����� ����� ��� �� ��������� 999

    float value = (float)fabs(atof(m_inputBuffer));

    int intValue = (int)value;

    // ������� ����� ����� ����� � �����
    sprintf(inputBuffer, "%d", intValue);

    posComma = (int8)strlen(inputBuffer) - 1;

    int numDigits = NUM_DIGITS - (int)strlen(inputBuffer);      // ������� ���� ����� �������� ����� �������

    int pos = SU::FindSymbol(m_inputBuffer, '.');       // ������� ������� ����� � �������� ������. ������� ����� �� ����� ������ � iws->inputBuffer

    for (int i = 0; i < numDigits; i++)
    {
        inputBuffer[posComma + 1 + i] = m_inputBuffer[pos + 1 + i];
    }
}
