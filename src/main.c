#include "stm32f10x.h"
#include "stm32f10x_misc.h"
#include "stm32f10x_init.h"

#include "printf.h"
#include "ps2.h"


void __io_putchar(void* p, char ch)
{
    /* Write a character to the USART */
    USART_SendData(USART1, (uint8_t) ch);

    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}
}

int main(void)
{
    /* Initialize system */
    STM32_Configuration();
    
    /* Wire putchar for printf */
    init_printf(0, __io_putchar);

    /* Release PS2 Clock and Data */
    GPIO_WriteBit(PS2_GPIO, PS2_Pin_CLK, Bit_SET);
    GPIO_WriteBit(PS2_GPIO, PS2_Pin_DATA, Bit_SET);

    /* Send PS2 BAT Code */
    Delay(200);
    PS2_OutputData = 0xAA;
    PS2_SendRequest = Bit_SET;

    while (1) {}
}


#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{
    printf("Wrong parameters value: file %s on line %d\r\n", file, line);
    while (1) {}
}

#endif
