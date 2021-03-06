#include "defines.h"
#include "common/Interface_p.h"
#include "Display/Display.h"
#include "Hardware/CPU.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Menu/Menu.h"
#include "SCPI/SCPI.h"
#include "Settings/CalibrationSettings.h"
#include "Settings/Settings.h"
#include "Tests/Tests.h"
#include "Utils/StringUtils.h"


void init()
{
    CPU::Init();

    Timer::Init();

    Display::Init();

    HAL_TIM::Delay(1);

    setCal.Load();

    set.LoadToDevice();

    Menu::Init();

    Locale::FindSeparator();
}



void update()
{
    static bool first = true;

    static uint timeStart = 0;

    if (first)
    {
        first = false;

        timeStart = TIME_MS;
    }

    if (TIME_MS - timeStart < 1000)
    {
        Display::DrawScreenSplash();
    }
    else if (TIME_MS - timeStart < 3000)
    {
        TestsHardware::Run();
    }
    else
    {
        CPU::Update();

        Menu::Update();

        Display::Update();

        PInterface::Update();

        SCPI::Update();
    }
}

