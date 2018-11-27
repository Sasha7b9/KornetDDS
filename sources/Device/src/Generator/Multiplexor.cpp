#include "stdafx.h"
#ifndef WIN32
#include "defines.h"
#include "FPGA.h"
#include "AD9952.h"
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
    A0_MX1 - PF0
    A1_MX1 - PF1
    A0_MX2 - PF2
    A0_MX2 - PF3
*/

#define PIN_MX1_A0  GPIO_PIN_0
#define PIN_MX2_A0  GPIO_PIN_2


Form FPGA::Multiplexor::mode[Chan::Number] = {Form::Sine, Form::Sine};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Form FPGA::Multiplexor::GetMode(Chan ch)
{
    return mode[ch];
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::Multiplexor::Init()
{
    GPIO_InitTypeDef  isGPIO =
    {
        PIN_MX1_A0 | PIN_MX2_A0,
        GPIO_MODE_OUTPUT_PP,
        GPIO_PULLUP
    };
    HAL_GPIO_Init(GPIOF, &isGPIO);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::Multiplexor::WriteRegister(Register::E reg, uint value)
{
    uint16 pinA0 = PIN_MX1_A0;

    if(reg == Register::Multiplexor2)
    {
        pinA0 = PIN_MX2_A0;
    }

    if(value & 0x1)  {   SetPin(pinA0); }
    else             { ResetPin(pinA0); }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::Multiplexor::SetMode(Chan::E ch, Form form)
{
    mode[ch] = form;

#define PIN_MX(ch) pins[ch]

    static const uint16 pins[Chan::Number] =
    {
        PIN_MX1_A0,
        PIN_MX2_A0
    };

    if(form.Is(Form::Sine))
    {
        SetPin(PIN_MX(ch));
    }
    else
    {
        ResetPin(PIN_MX(ch));
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::Multiplexor::SetPin(uint16 pin)
{
    HAL_GPIO_WritePin(GPIOF, pin, GPIO_PIN_SET);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::Multiplexor::ResetPin(uint16 pin)
{
    HAL_GPIO_WritePin(GPIOF, pin, GPIO_PIN_RESET);
}
