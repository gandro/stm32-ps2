#ifndef __PS2_H__
#define __PS2_H__

#include "stm32f10x.h"

/* 12kHZ = 24MHz/2/1000:     Prescaler = 2 Period = 1000 */
/* 1 HZ  = 24MHz/24000/1000: Prescaler = 24000, Period = 1000 */

#define PS2_CLK_PERIOD      1000    //1000
#define PS2_CLK_PRESCALER   2   //24000

/* Both pins need to be 5 Volt tolerable */

#define PS2_GPIO        GPIOC
#define PS2_GPIO_CLOCK  RCC_APB2Periph_GPIOC
#define PS2_Pin_CLK     GPIO_Pin_7 /* PC7 */
#define PS2_Pin_DATA    GPIO_Pin_6 /* PC6 */

typedef enum { IDLE, SEND, REQUEST, RECEIVE } PS2_StateTypeDef;
typedef enum { START, DATA, PARITY, STOP, ACK, FINISHED } PS2_TransferStateTypeDef;

extern PS2_StateTypeDef PS2_State;
extern PS2_TransferStateTypeDef PS2_TransferState;

extern BitAction PS2_SendRequest;
extern uint8_t PS2_OutputData;
extern uint8_t PS2_OutputBitNr;

extern uint8_t PS2_InputData;
extern uint8_t PS2_InputBitNr;

extern uint8_t PS2_Parity;

void PS2_DataIRQHandler(void);
void PS2_ClockIRQHandler(void);

#endif
