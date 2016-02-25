/*******************************************************************************
* File Name: UART_uart_tx_i2c_scl_spi_miso.h  
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

#if !defined(CY_PINS_UART_uart_tx_i2c_scl_spi_miso_H) /* Pins UART_uart_tx_i2c_scl_spi_miso_H */
#define CY_PINS_UART_uart_tx_i2c_scl_spi_miso_H

#include "cytypes.h"
#include "cyfitter.h"
#include "UART_uart_tx_i2c_scl_spi_miso_aliases.h"


/***************************************
*        Function Prototypes             
***************************************/    

void    UART_uart_tx_i2c_scl_spi_miso_Write(uint8 value) ;
void    UART_uart_tx_i2c_scl_spi_miso_SetDriveMode(uint8 mode) ;
uint8   UART_uart_tx_i2c_scl_spi_miso_ReadDataReg(void) ;
uint8   UART_uart_tx_i2c_scl_spi_miso_Read(void) ;
uint8   UART_uart_tx_i2c_scl_spi_miso_ClearInterrupt(void) ;


/***************************************
*           API Constants        
***************************************/

/* Drive Modes */
#define UART_uart_tx_i2c_scl_spi_miso_DRIVE_MODE_BITS        (3)
#define UART_uart_tx_i2c_scl_spi_miso_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - UART_uart_tx_i2c_scl_spi_miso_DRIVE_MODE_BITS))

#define UART_uart_tx_i2c_scl_spi_miso_DM_ALG_HIZ         (0x00u)
#define UART_uart_tx_i2c_scl_spi_miso_DM_DIG_HIZ         (0x01u)
#define UART_uart_tx_i2c_scl_spi_miso_DM_RES_UP          (0x02u)
#define UART_uart_tx_i2c_scl_spi_miso_DM_RES_DWN         (0x03u)
#define UART_uart_tx_i2c_scl_spi_miso_DM_OD_LO           (0x04u)
#define UART_uart_tx_i2c_scl_spi_miso_DM_OD_HI           (0x05u)
#define UART_uart_tx_i2c_scl_spi_miso_DM_STRONG          (0x06u)
#define UART_uart_tx_i2c_scl_spi_miso_DM_RES_UPDWN       (0x07u)

/* Digital Port Constants */
#define UART_uart_tx_i2c_scl_spi_miso_MASK               UART_uart_tx_i2c_scl_spi_miso__MASK
#define UART_uart_tx_i2c_scl_spi_miso_SHIFT              UART_uart_tx_i2c_scl_spi_miso__SHIFT
#define UART_uart_tx_i2c_scl_spi_miso_WIDTH              1u


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define UART_uart_tx_i2c_scl_spi_miso_PS                     (* (reg32 *) UART_uart_tx_i2c_scl_spi_miso__PS)
/* Port Configuration */
#define UART_uart_tx_i2c_scl_spi_miso_PC                     (* (reg32 *) UART_uart_tx_i2c_scl_spi_miso__PC)
/* Data Register */
#define UART_uart_tx_i2c_scl_spi_miso_DR                     (* (reg32 *) UART_uart_tx_i2c_scl_spi_miso__DR)
/* Input Buffer Disable Override */
#define UART_uart_tx_i2c_scl_spi_miso_INP_DIS                (* (reg32 *) UART_uart_tx_i2c_scl_spi_miso__PC2)


#if defined(UART_uart_tx_i2c_scl_spi_miso__INTSTAT)  /* Interrupt Registers */

    #define UART_uart_tx_i2c_scl_spi_miso_INTSTAT                (* (reg32 *) UART_uart_tx_i2c_scl_spi_miso__INTSTAT)

#endif /* Interrupt Registers */


/***************************************
* The following code is DEPRECATED and 
* must not be used.
***************************************/

#define UART_uart_tx_i2c_scl_spi_miso_DRIVE_MODE_SHIFT       (0x00u)
#define UART_uart_tx_i2c_scl_spi_miso_DRIVE_MODE_MASK        (0x07u << UART_uart_tx_i2c_scl_spi_miso_DRIVE_MODE_SHIFT)


#endif /* End Pins UART_uart_tx_i2c_scl_spi_miso_H */


/* [] END OF FILE */
