#include "defines.h"
#include "Hardware/CPU.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include <stm32f4xx_hal.h>


#ifdef __cplusplus
extern "C" {
#endif

    
    void SysTick_Handler()
    {
        HAL_IncTick();
        HAL_SYSTICK_IRQHandler();
    }
    

    void MemManage_Handler()
    {
        while (1)
        {
        }
    }

    
    void UsageFault_Handler()
    {
        while (1)
        {
        }
    }

    
    void HardFault_Handler()
    {
        while (1) //-V776
        {
        }
    }

    
    __attribute((noreturn)) void BusFault_Handler()
    {
        while (1)
        {
        }
    }

    
    void PendSV_Handler()
    {
    }

    
    void NMI_Handler()
    {
    }

    
    void SVC_Handler()
    {
    }

    
    void DebugMon_Handler()
    {
    }

    void TIM4_IRQHandler()
    {
        if((TIM4->SR & TIM_SR_UIF) == TIM_SR_UIF)
        {
            if((TIM4->DIER & TIM_DIER_UIE) == TIM_DIER_UIE)
            {
                TIM4->SR = ~TIM_DIER_UIE;
                HAL_TIM4::ElapsedCallback();
            }
        }
    }

    void TIM3_IRQHandler()
    {
        if((TIM3->SR & TIM_SR_UIF) == TIM_SR_UIF)
        {
            if((TIM3->DIER & TIM_DIER_UIE) == TIM_DIER_UIE)
            {
                TIM3->SR = ~TIM_DIER_UIE;
                Timer::ElapsedCallback();
            }
        }
    }

#ifdef __cplusplus
}
#endif