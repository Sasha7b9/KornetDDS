#include "defines.h"
#include "log.h"
#include "Timer.h"
#include "Hardware/CPU.h"
#include "Hardware/HAL/HAL.h"
#include <stm32f4xx_hal.h>
#include <limits>


struct TimerStruct
{
    pFuncVV func;       // ������� �������
    uint    dTms;          // ������ ������������, ��
    uint    timeNextMS;    // ����� ���������� ������������. ���� == 0xffffffff, �� ������ ���������
    bool    repeat;        // ���� true, ����� �����������, ���� �� ����� ������� ������� Timer_Disable()
    uint8   notUsed0;
    uint8   notUsed1;
    uint8   notUsed2;
};



static TimerStruct timers[Timer::Type::Count];
static uint timeStartLogging = 0;
static uint timePrevPoint = 0;



#undef TIME_NEXT
#define TIME_NEXT(type) (timers[type].timeNextMS)



/// ������� �����, ������� ����������� � timeStop ��
static void StartTIM(uint timeStop);

static void StopTIM();
/// ���������� ����� ������������ ���������� �������, ���� 0, ���� �������� ���
static uint NearestTime();
/// ��������� ������� �� ������
static void TuneTIM(Timer::Type type);
/// ���������� ��� ������������ �������
static void ElapsedCallback();



bool Timer::IsRun(Type type)
{
    return TIME_NEXT(type) != UINT_MAX;
}


void Timer::Init()
{
    for(uint i = 0; i < Timer::Type::Count; i++)
    {
        timers[i].timeNextMS = UINT_MAX;
    }

    HAL_TIM2::Init();
    HAL_TIM3::Init();
}


static void ElapsedCallback()
{
    uint time = TIME_MS;

    if (NearestTime() > time)
    {
        return;
    }

    StopTIM();

    for (uint type = 0; type < Timer::Type::Count; type++)
    {
        if (TIME_NEXT(type) <= time)            // ���� ������ ����� ������������
        {
            TimerStruct *timer = &timers[type];
            timer->func();
            if (timer->repeat)
            {
                do      // ���� ����� ������, ��� ��������� ������ SysTick, ������� ����������� ������������, ����� ��������� ���������,
                {       // � ���� ������� ����������� ������, ��� timer->dTm ��, �� ��� ��� ��������
                    timer->timeNextMS += timer->dTms;
                } while (timer->timeNextMS < TIME_MS);

            }
            else
            {
                timer->timeNextMS = UINT_MAX;
            }
        }
    }

    StartTIM(NearestTime());
}


void Timer::Set(Type type, pFuncVV func, uint dTms)
{
    TimerStruct *timer = &timers[type];
    timer->func = func;
    timer->dTms = dTms;
}


void Timer::SetAndStartOnce(Type type, pFuncVV func, uint dTms)
{
    Timer::Set(type, func, dTms);
    StartOnce(type);
}


void Timer::SetAndEnable(Type type, pFuncVV func, uint dTms)
{
    Set(type, func, dTms);
    Enable(type);
}


void Timer::StartOnce(Type type)
{
    timers[type].repeat = false;
    TuneTIM(type);
}


void Timer::Enable(Type type)
{
    timers[type].repeat = true;
    TuneTIM(type);
}


static void TuneTIM(Timer::Type type)
{
    TimerStruct *timer = &timers[type];

    uint timeNearest = NearestTime();

    uint timeNext = TIME_MS + timer->dTms;
    timer->timeNextMS = timeNext;

    if(timeNext < timeNearest)      // ���� ������ ������ ��������� ������ ��������
    {
        StartTIM(timeNext);         // �� ������� ������ �� ���� �����
    }
}


void Timer::Disable(Type type)
{
    timers[type].timeNextMS = UINT_MAX;
    timers[type].repeat = false;
}


static uint NearestTime()
{
    uint time = UINT_MAX;

    for(uint type = 0; type < Timer::Type::Count; type++)
    {
        if(TIME_NEXT(type) < time)
        {
            time = TIME_NEXT(type);
        }
    }
    
    return time;
}


static void StartTIM(uint timeStopMS)
{
    StopTIM();

    if(timeStopMS == UINT_MAX)
    {
        return;
    }

    uint dT = timeStopMS - TIME_MS;

    // 10 ������������� 0.1��. �.�. ���� ��� ����� 1��, ����� �������� (100 - 1)
    HAL_TIM3::StartIT((dT * 2) - 1);
}


static void StopTIM()
{
    HAL_TIM3::StopIT();
}


void Timer::PauseOnTicks(uint numTicks)
{
    volatile uint startTicks = TIME_TICKS;
    while (TIME_TICKS - startTicks < numTicks)
    {
    };
}


void Timer::StartLogging()
{
    timeStartLogging = TIME_TICKS;
    timePrevPoint = timeStartLogging;
}


uint Timer::LogPointUS(char * name)
{
    uint interval = TIME_TICKS - timePrevPoint;
    timePrevPoint = TIME_TICKS;
    LOG_WRITE("%s %.2f us", name, interval / 120.0);
    return interval;
}


uint Timer::LogPointMS(char * name)
{
    uint interval = TIME_TICKS - timePrevPoint;
    timePrevPoint = TIME_TICKS;
    LOG_WRITE("%s %.2f ms", name, interval / 120e3);
    return interval;
}


uint Timer::TimeUS()
{
    return (TIM2->CNT / 90);
}


#ifdef __cplusplus
extern "C" {
#endif

    
    void TIM3_IRQHandler()
    {
        if ((TIM3->SR & TIM_SR_UIF) == TIM_SR_UIF)
        {
            if((TIM3->DIER & TIM_DIER_UIE) == TIM_DIER_UIE)
            {
                TIM3->SR = ~TIM_DIER_UIE;
                ElapsedCallback();
            }
        }
    }

#ifdef __cplusplus
}
#endif


#undef TIME_NEXT
