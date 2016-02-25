/*******************************************************************************
* File Name: CTS.h  
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

#if !defined(CY_PINS_CTS_H) /* Pins CTS_H */
#define CY_PINS_CTS_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CTS_aliases.h"


/***************************************
*        Function Prototypes             
***************************************/    

void    CTS_Write(uint8 value) ;
void    CTS_SetDriveMode(uint8 mode) ;
uint8   CTS_ReadDataReg(void) ;
uint8   CTS_Read(void) ;
uint8   CTS_ClearInterrupt(void) ;


/***************************************
*           API Constants        
***************************************/

/* Drive Modes */
#define CTS_DRIVE_MODE_BITS        (3)
#define CTS_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - CTS_DRIVE_MODE_BITS))

#define CTS_DM_ALG_HIZ         (0x00u)
#define CTS_DM_DIG_HIZ         (0x01u)
#define CTS_DM_RES_UP          (0x02u)
#define CTS_DM_RES_DWN         (0x03u)
#define CTS_DM_OD_LO           (0x04u)
#define CTS_DM_OD_HI           (0x05u)
#define CTS_DM_STRONG          (0x06u)
#define CTS_DM_RES_UPDWN       (0x07u)

/* Digital Port Constants */
#define CTS_MASK               CTS__MASK
#define CTS_SHIFT              CTS__SHIFT
#define CTS_WIDTH              1u


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define CTS_PS                     (* (reg32 *) CTS__PS)
/* Port Configuration */
#define CTS_PC                     (* (reg32 *) CTS__PC)
/* Data Register */
#define CTS_DR                     (* (reg32 *) CTS__DR)
/* Input Buffer Disable Override */
#define CTS_INP_DIS                (* (reg32 *) CTS__PC2)


#if defined(CTS__INTSTAT)  /* Interrupt Registers */

    #define CTS_INTSTAT                (* (reg32 *) CTS__INTSTAT)

#endif /* Interrupt Registers */


/***************************************
* The following code is DEPRECATED and 
* must not be used.
***************************************/

#define CTS_DRIVE_MODE_SHIFT       (0x00u)
#define CTS_DRIVE_MODE_MASK        (0x07u << CTS_DRIVE_MODE_SHIFT)


#endif /* End Pins CTS_H */


/* [] END OF FILE */
