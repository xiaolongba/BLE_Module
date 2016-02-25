/*******************************************************************************
* File Name: .h
* Version 3.0
*
* Description:
*  This private file provides constants and parameter values for the
*  SCB Component in _EZI2C mode.
*  Do not use this file or its content in your project.
*
* Note:
*
********************************************************************************
* Copyright 2013-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SCB_EZI2C_PVT_UART_H)
#define CY_SCB_EZI2C_PVT_UART_H

#include "UART_EZI2C.h"


/***************************************
*      EZI2C Private Vars
***************************************/

extern volatile uint8 UART_curStatus;
extern uint8 UART_fsmState;

/* Variables intended to be used with Buffer 1: Primary slave address */
extern volatile uint8 * UART_dataBuffer1;
extern uint16 UART_bufSizeBuf1;
extern uint16 UART_protectBuf1;
extern uint16 UART_offsetBuf1;
extern uint16 UART_indexBuf1;

#if(UART_SECONDARY_ADDRESS_ENABLE_CONST)
    extern uint8 UART_addrBuf1;
    extern uint8 UART_addrBuf2;

    /* Variables intended to be used with Buffer 2: Primary slave address */
    extern volatile uint8 * UART_dataBuffer2;
    extern uint16 UART_bufSizeBuf2;
    extern uint16 UART_protectBuf2;
    extern uint16 UART_offsetBuf2;
    extern uint16 UART_indexBuf2;
#endif /* (UART_SECONDARY_ADDRESS_ENABLE_CONST) */


/***************************************
*     Private Function Prototypes
***************************************/

#if(UART_SCB_MODE_EZI2C_CONST_CFG)
    void UART_EzI2CInit(void);
#endif /* (UART_SCB_MODE_EZI2C_CONST_CFG) */

void UART_EzI2CStop(void);
#if(UART_EZI2C_WAKE_ENABLE_CONST)
    void UART_EzI2CSaveConfig(void);
    void UART_EzI2CRestoreConfig(void);
#endif /* (UART_EZI2C_WAKE_ENABLE_CONST) */

#endif /* (CY_SCB__EZI2C_PVT_UART_H) */


/* [] END OF FILE */
