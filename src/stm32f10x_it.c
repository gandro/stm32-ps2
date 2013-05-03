#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "stm32f10x_misc.h"
#include "ps2.h"

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
    while (1)
    {}
}

void MemManage_Handler(void)
{
    while (1)
    {}
}

void BusFault_Handler(void)
{
    while (1)
    {}
}

void UsageFault_Handler(void)
{
    while (1)
    {}
}

void DebugMon_Handler(void)
{
}

void SVC_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
    TimingDelay_Decrement();
}

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET) {
        /* CC1 is the CLK Timer Channel */
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);

        PS2_ClockIRQHandler();
    } else if (TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET) {
        /* CC2 is the DATA Timer Channel */
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);

        PS2_DataIRQHandler();
    }
}

void EXTI0_IRQHandler(void)
{
    EXTI_ClearITPendingBit(EXTI_Line0);
    if(PS2_SendRequest == Bit_RESET) {
        PS2_OutputData = 0x15; // Makecode Q
        PS2_SendRequest = Bit_SET;
    }
}
