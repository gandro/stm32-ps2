#include "stm32f10x.h"
#include "stm32f10x_misc.h"
#include "ps2.h"

PS2_StateTypeDef PS2_State = IDLE;
PS2_TransferStateTypeDef PS2_TransferState = START;

BitAction PS2_SendRequest = Bit_RESET;
uint8_t PS2_OutputData = 0x00;
uint8_t PS2_OutputBitNr = 0;

uint8_t PS2_InputData = 0;
uint8_t PS2_InputBitNr = 0;

uint8_t PS2_Parity = 0;

PS2_TransferStateTypeDef PS2_SendDataIRQHandler(void)
{
    BitAction PS2_DataBit = 0;
    PS2_TransferStateTypeDef PS2_NextState = PS2_TransferState;

    switch(PS2_TransferState) {
    case START:
        /* Initalize  */
        PS2_OutputBitNr = 0;
        PS2_Parity = 0;

        /* Send START Bit */
        PS2_DataBit = 0;
        PS2_NextState = DATA;
        break;
    case DATA:
        /*  next DATA Bit */
        PS2_DataBit = *(BITBAND_SRAM(&PS2_OutputData, PS2_OutputBitNr));
        PS2_OutputBitNr++;

        /* Calculate Paritiy */
        PS2_Parity ^= PS2_DataBit;
        if(PS2_OutputBitNr > 7) {
            /* Finalize paritiy bit */
            PS2_Parity = !PS2_Parity;
            PS2_NextState = PARITY;
        }
        break;
    case PARITY:
        /* Send PARITY Bit */
        PS2_DataBit = PS2_Parity;
        PS2_NextState = STOP;
        break;
    case STOP:
        /* Send STOP Bit */
        PS2_DataBit = 1;
        PS2_NextState = FINISHED;
        break;
    case ACK:
    case FINISHED:
        /* This should never happen! */
        break;
    }

    /* Write output bit */
    GPIO_WriteBit(PS2_GPIO, PS2_Pin_DATA, PS2_DataBit);


    return PS2_NextState;
}

PS2_TransferStateTypeDef PS2_ReceiveDataIRQHandler(void)
{
    PS2_TransferStateTypeDef PS2_NextState = PS2_TransferState;
    BitAction PS2_DataBit = 0;

    /* Read input */
    PS2_DataBit = GPIO_ReadInputDataBit(PS2_GPIO, PS2_Pin_DATA);

    switch(PS2_TransferState) {
    case START:
        /* Initalize */
        PS2_OutputBitNr = 0;
        PS2_Parity = 0;

        /* Check START Bit for errors */
        if(PS2_DataBit != 0) {
            /* Error */ // XXX abort here
        }

        PS2_NextState = DATA;

        break;
    case DATA:
        /*  next DATA Bit */
        *(BITBAND_SRAM(&PS2_InputData, PS2_InputBitNr)) = PS2_DataBit;
        PS2_InputBitNr++;

        /* Calculate Paritiy */
        PS2_Parity ^= PS2_DataBit;
        if(PS2_InputBitNr > 7) {
            /* Finalize paritiy bit */
            PS2_Parity = !PS2_Parity;
            PS2_NextState = PARITY;
        }
        break;
    case PARITY:
        if(PS2_DataBit != PS2_Parity) {
            /* Error */
        }
        PS2_NextState = STOP;
        break;
    case STOP:
        if(PS2_DataBit != 1) {
            /* Error */
        }
        PS2_NextState = ACK;
        break;
    case ACK:
        /* Acknoledge everything */
        GPIO_WriteBit(PS2_GPIO, PS2_Pin_DATA, Bit_RESET);
        PS2_NextState = FINISHED;
        break;
    case FINISHED:
        /* Release DATA Pin */
        GPIO_WriteBit(PS2_GPIO, PS2_Pin_DATA, Bit_SET);
        break;
    }

    return PS2_NextState;
}

void PS2_DataIRQHandler(void)
{

    if(PS2_State == IDLE || PS2_State == REQUEST) {
        /* Nothing to do */
        return;
    }

    /* Check if the communication was canceled */
    if(GPIO_ReadInputDataBit(PS2_GPIO, PS2_Pin_CLK) == RESET) {
        /* Release DATA Pin */
        GPIO_WriteBit(PS2_GPIO, PS2_Pin_DATA, Bit_SET);
        PS2_State = IDLE;
        return;
    }

    if(PS2_State == SEND) {
        PS2_TransferState = PS2_SendDataIRQHandler();
    } else if(PS2_State == RECEIVE) {
        PS2_TransferState = PS2_ReceiveDataIRQHandler();
    }
}

void PS2_CheckRequestToReceive(void)
{
    if(PS2_State == IDLE) {
        /* Idle, CLK should be set, otherwise we have a receive request */
        if(GPIO_ReadInputDataBit(PS2_GPIO, PS2_Pin_CLK) == RESET) {
            PS2_State = REQUEST;
        }
    } else if(PS2_State == REQUEST) {
        /* Check if CLK is set again, then the transfer can start */
        if(GPIO_ReadInputDataBit(PS2_GPIO, PS2_Pin_CLK) == SET) {
            // TODO check data here?
            PS2_State = RECEIVE;
            PS2_TransferState = START;
        }
    }
}

void PS2_CheckRequestToSend(void)
{
    if(PS2_State == IDLE && PS2_SendRequest == Bit_SET) {
        PS2_State = SEND;
        PS2_TransferState = START;
    }
}

void PS2_ClearFinishedSend(void)
{
    if(PS2_State == SEND && PS2_TransferState == FINISHED) {
        PS2_State = IDLE;
        PS2_SendRequest = Bit_RESET;
    }
}

void PS2_ClearFinishedReceive(void)
{
    if(PS2_State == RECEIVE && PS2_TransferState == FINISHED) {
        PS2_State = IDLE;
    }
}

void PS2_ClockIRQHandler(void)
{
    if(TIM_ReadDirection(TIM2) == UP) {
        /* Counter Direction UP, CLK Rising Edge */

        PS2_CheckRequestToReceive();

        if(PS2_State == SEND || PS2_State == RECEIVE) {
            GPIO_WriteBit(PS2_GPIO, PS2_Pin_CLK, Bit_SET);
        }

        PS2_ClearFinishedSend();
        PS2_CheckRequestToSend();
    } else {
        /* Counter Direction DOWN, CLK Falling Edge */

        if(PS2_State == SEND || PS2_State == RECEIVE) {
            GPIO_WriteBit(PS2_GPIO, PS2_Pin_CLK, Bit_RESET);
        }
        
        PS2_ClearFinishedReceive();
    }
}

