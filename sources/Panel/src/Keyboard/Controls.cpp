#include "defines.h"
#include "Controls.h"


bool Key::IsDigit() const
{
    return (value >= Key::_0 && value <= Key::_9);
}


char Key::ToChar() const
{
    if(IsDigit())
    {
        return static_cast<char>((value - Key::_0) | 0x30);
    }
    else if(value == Key::Dot)
    {
        return '.';
    }

    return '\0';
}


bool Control::IsFunctional() const
{
    return (key >= Key::F1) && (key <= Key::F4);
}


bool Control::IsRotate() const
{
    return (key == Key::RegLeft) || (key == Key::RegRight);
}


bool Control::IsRelease() const
{
    return (action == Action::Up) || (action == Action::Long);
}


pString Control::Name(Key::E key)
{
    static pCHAR names[] =
    {
        "None",
        "F1",
        "F2",
        "F3",
        "F4",
        "0",
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        "8",
        "9",
        ".",
        "-",
        "Esc",
        "Left",
        "Right",
        "On1",
        "On2",
        "RegLeft",
        "RegRight",
        "RegButton"
    };

    return names[key];
}
