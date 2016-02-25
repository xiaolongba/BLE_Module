/*******************************************************************************
* File Name: .h
* Version 3.0
*
* Description:
*  This private file provides constants and parameter values for the
*  SCB Component in I2C mode.
*  Please do not use this file or its content in your project.
*
* Note:
*
********************************************************************************
* Copyright 2013-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SCB_I2C_PVT_UART_H)
#define CY_SCB_I2C_PVT_UART_H

#include "UART_I2C.h"


/***************************************
*     Private Global Vars
***************************************/

extern volatile uint8 UART_state; /* Current state of I2C FSM */

#if(UART_I2C_SLAVE_CONST)
    extern volatile uint8 UART_slStatus;          /* Slave Status */

    /* Receive buffer variables */
    extern volatile uint8 * UART_slWrBufPtr;      /* Pointer to Receive buffer  */
    extern volatile uint32  UART_slWrBufSize;     /* Slave Receive buffer size  */
    extern volatile uint32  UART_slWrBufIndex;    /* Slave Receive buffer Index */

    /* Transmit buffer variables */
    extern volatile uint8 * UART_slRdBufPtr;      /* Pointer to Transmit buffer  */
    extern volatile uint32  UART_slRdBufSize;     /* Slave Transmit buffer size  */
    extern volatile uint32  UART_slRdBufIndex;    /* Slave Transmit buffer Index */
    extern volatile uint32  UART_slRdBufIndexTmp; /* Slave Transmit buffer Index Tmp */
    extern volatile uint8   UART_slOverFlowCount; /* Slave Transmit Overflow counter */
#endif /* (UART_I2C_SLAVE_CONST) */

#if(UART_I2C_MASTER_CONST)
    extern volatile uint16 UART_mstrStatus;      /* Master Status byte  */
    extern volatile uint8  UART_mstrControl;     /* Master Control byte */

    /* Receive buffer variables */
    extern volatile uint8 * UART_mstrRdBufPtr;   /* Pointer to Master Read buffer */
    extern volatile uint32  UART_mstrRdBufSize;  /* Master Read buffer size       */
    extern volatile uint32  UART_mstrRdBufIndex; /* Master Read buffer Index      */

    /* Transmit buffer variables */
    extern volatile uint8 * UART_mstrWrBufPtr;   /* Pointer to Master Write buffer */
    extern volatile uint32  UART_mstrWrBufSize;  /* Master Write buffer size       */
    extern volatile uint32  UART_mstrWrBufIndex; /* Master Write buffer Index      */
    extern volatile uint32  UART_mstrWrBufIndexTmp; /* Master Write buffer Index Tmp */
#endif /* (UART_I2C_MASTER_CONST) */

#if (UART_I2C_CUSTOM_ADDRESS_HANDLER_CONST)
    extern uint32 (*UART_customAddressHandler) (void);
#endif /* (UART_I2C_CUSTOM_ADDRESS_HANDLER_CONST) */

/***************************************
*     Private Function Prototypes
***************************************/

#if(UART_SCB_MODE_I2C_CONST_CFG)
    void UART_I2CInit(void);
#endif /* (UART_SCB_MODE_I2C_CONST_CFG) */

void UART_I2CStop(void);
void UART_I2CSaveConfig(void);
void UART_I2CRestoreConfig(void);

#if(UART_I2C_MASTER_CONST)
    void UART_I2CReStartGeneration(void);
#endif /* (UART_I2C_MASTER_CONST) */

#endif /* (CY_SCB_I2C_PVT_UART_H) */


/* [] END OF FILE */
