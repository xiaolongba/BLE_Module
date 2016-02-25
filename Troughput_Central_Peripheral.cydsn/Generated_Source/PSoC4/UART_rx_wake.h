/*******************************************************************************
* File Name: UART_rx_wake.h  
* Version 2.10
*
* Description:
*  This file containts Control Register function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_UART_rx_wake_H) /* Pins UART_rx_wake_H */
#define CY_PINS_UART_rx_wake_H

#include "cytypes.h"
#include "cyfitter.h"
#include "UART_rx_wake_aliases.h"


/***************************************
*        Function Prototypes             
***************************************/    

void    UART_rx_wake_Write(uint8 value) ;
void    UART_rx_wake_SetDriveMode(uint8 mode) ;
uint8   UART_rx_wake_ReadDataReg(void) ;
uint8   UART_rx_wake_Read(void) ;
uint8   UART_rx_wake_ClearInterrupt(void) ;


/***************************************
*           API Constants        
***************************************/

/* Drive Modes */
#define UART_rx_wake_DRIVE_MODE_BITS        (3)
#define UART_rx_wake_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - UART_rx_wake_DRIVE_MODE_BITS))

#define UART_rx_wake_DM_ALG_HIZ         (0x00u)
#define UART_rx_wake_DM_DIG_HIZ         (0x01u)
#define UART_rx_wake_DM_RES_UP          (0x02u)
#define UART_rx_wake_DM_RES_DWN         (0x03u)
#define UART_rx_wake_DM_OD_LO           (0x04u)
#define UART_rx_wake_DM_OD_HI           (0x05u)
#define UART_rx_wake_DM_STRONG          (0x06u)
#define UART_rx_wake_DM_RES_UPDWN       (0x07u)

/* Digital Port Constants */
#define UART_rx_wake_MASK               UART_rx_wake__MASK
#define UART_rx_wake_SHIFT              UART_rx_wake__SHIFT
#define UART_rx_wake_WIDTH              1u


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define UART_rx_wake_PS                     (* (reg32 *) UART_rx_wake__PS)
/* Port Configuration */
#define UART_rx_wake_PC                     (* (reg32 *) UART_rx_wake__PC)
/* Data Register */
#define UART_rx_wake_DR                     (* (reg32 *) UART_rx_wake__DR)
/* Input Buffer Disable Override */
#define UART_rx_wake_INP_DIS                (* (reg32 *) UART_rx_wake__PC2)


#if defined(UART_rx_wake__INTSTAT)  /* Interrupt Registers */

    #define UART_rx_wake_INTSTAT                (* (reg32 *) UART_rx_wake__INTSTAT)

#endif /* Interrupt Registers */


/***************************************
* The following code is DEPRECATED and 
* must not be used.
***************************************/

#define UART_rx_wake_DRIVE_MODE_SHIFT       (0x00u)
#define UART_rx_wake_DRIVE_MODE_MASK        (0x07u << UART_rx_wake_DRIVE_MODE_SHIFT)


#endif /* End Pins UART_rx_wake_H */


/* [] END OF FILE */
