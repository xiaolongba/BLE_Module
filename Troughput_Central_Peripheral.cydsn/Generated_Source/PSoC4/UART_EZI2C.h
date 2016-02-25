/*******************************************************************************
* File Name: UART_EZI2C.h
* Version 3.0
*
* Description:
*  This file provides constants and parameter values for the SCB Component in
*  the EZI2C mode.
*
* Note:
*
********************************************************************************
* Copyright 2013-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SCB_EZI2C_UART_H)
#define CY_SCB_EZI2C_UART_H

#include "UART.h"


/***************************************
*   Initial Parameter Constants
****************************************/

#define UART_EZI2C_CLOCK_STRETCHING         (1u)
#define UART_EZI2C_MEDIAN_FILTER_ENABLE     (1u)
#define UART_EZI2C_NUMBER_OF_ADDRESSES      (0u)
#define UART_EZI2C_PRIMARY_SLAVE_ADDRESS    (8u)
#define UART_EZI2C_SECONDARY_SLAVE_ADDRESS  (9u)
#define UART_EZI2C_SUB_ADDRESS_SIZE         (0u)
#define UART_EZI2C_WAKE_ENABLE              (0u)
#define UART_EZI2C_DATA_RATE                (100u)
#define UART_EZI2C_SLAVE_ADDRESS_MASK       (254u)
#define UART_EZI2C_BYTE_MODE_ENABLE         (0u)


/***************************************
*  Conditional Compilation Parameters
****************************************/

#if(UART_SCB_MODE_UNCONFIG_CONST_CFG)

    #define UART_SUB_ADDRESS_SIZE16             (0u != UART_subAddrSize)
    #define UART_SECONDARY_ADDRESS_ENABLE       (0u != UART_numberOfAddr)

    #define UART_EZI2C_EC_AM_ENABLE         (0u != (UART_CTRL_REG & \
                                                                UART_CTRL_EC_AM_MODE))
    #define UART_EZI2C_SCL_STRETCH_ENABLE   (0u != (UART_GetSlaveInterruptMode() & \
                                                                UART_INTR_SLAVE_I2C_ADDR_MATCH))
    #define UART_EZI2C_SCL_STRETCH_DISABLE       (!UART_EZI2C_SCL_STRETCH_ENABLE)

    #define UART_SECONDARY_ADDRESS_ENABLE_CONST  (1u)
    #define UART_SUB_ADDRESS_SIZE16_CONST        (1u)
    #define UART_EZI2C_SCL_STRETCH_ENABLE_CONST  (1u)
    #define UART_EZI2C_SCL_STRETCH_DISABLE_CONST (1u)
    #define UART_EZI2C_WAKE_ENABLE_CONST         (1u)

    #if (UART_CY_SCBIP_V0 || UART_CY_SCBIP_V1)
        #define UART_EZI2C_FIFO_SIZE    (UART_FIFO_SIZE)
    #else
        #define UART_EZI2C_FIFO_SIZE    (UART_GET_FIFO_SIZE(UART_CTRL_REG & \
                                                                                    UART_CTRL_BYTE_MODE))
    #endif /* (UART_CY_SCBIP_V0 || UART_CY_SCBIP_V1) */

#else

    #define UART_SUB_ADDRESS_SIZE16         (0u != UART_EZI2C_SUB_ADDRESS_SIZE)
    #define UART_SUB_ADDRESS_SIZE16_CONST   (UART_SUB_ADDRESS_SIZE16)

    #define UART_SECONDARY_ADDRESS_ENABLE        (0u != UART_EZI2C_NUMBER_OF_ADDRESSES)
    #define UART_SECONDARY_ADDRESS_ENABLE_CONST  (UART_SECONDARY_ADDRESS_ENABLE)

    #define UART_EZI2C_SCL_STRETCH_ENABLE        (0u != UART_EZI2C_CLOCK_STRETCHING)
    #define UART_EZI2C_SCL_STRETCH_DISABLE       (!UART_EZI2C_SCL_STRETCH_ENABLE)
    #define UART_EZI2C_SCL_STRETCH_ENABLE_CONST  (UART_EZI2C_SCL_STRETCH_ENABLE)
    #define UART_EZI2C_SCL_STRETCH_DISABLE_CONST (UART_EZI2C_SCL_STRETCH_DISABLE)

    #define UART_EZI2C_WAKE_ENABLE_CONST         (0u != UART_EZI2C_WAKE_ENABLE)
    #define UART_EZI2C_EC_AM_ENABLE              (0u != UART_EZI2C_WAKE_ENABLE)

    #if (UART_CY_SCBIP_V0 || UART_CY_SCBIP_V1)
       #define UART_EZI2C_FIFO_SIZE    (UART_FIFO_SIZE)

    #else
        #define UART_EZI2C_FIFO_SIZE \
                                            UART_GET_FIFO_SIZE(UART_EZI2C_BYTE_MODE_ENABLE)
    #endif /* (UART_CY_SCBIP_V0 || UART_CY_SCBIP_V1) */

#endif /* (UART_SCB_MODE_UNCONFIG_CONST_CFG) */


/***************************************
*       Type Definitions
***************************************/

typedef struct
{
    uint32 enableClockStretch;
    uint32 enableMedianFilter;
    uint32 numberOfAddresses;
    uint32 primarySlaveAddr;
    uint32 secondarySlaveAddr;
    uint32 subAddrSize;
    uint32 enableWake;
    uint8  enableByteMode;
} UART_EZI2C_INIT_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

#if(UART_SCB_MODE_UNCONFIG_CONST_CFG)
    void UART_EzI2CInit(const UART_EZI2C_INIT_STRUCT *config);
#endif /* (UART_SCB_MODE_UNCONFIG_CONST_CFG) */

uint32 UART_EzI2CGetActivity(void);

void   UART_EzI2CSetAddress1(uint32 address);
uint32 UART_EzI2CGetAddress1(void);
void   UART_EzI2CSetBuffer1(uint32 bufSize, uint32 rwBoundary, volatile uint8 * buffer);
void   UART_EzI2CSetReadBoundaryBuffer1(uint32 rwBoundary);

#if(UART_SECONDARY_ADDRESS_ENABLE_CONST)
    void   UART_EzI2CSetAddress2(uint32 address);
    uint32 UART_EzI2CGetAddress2(void);
    void   UART_EzI2CSetBuffer2(uint32 bufSize, uint32 rwBoundary, volatile uint8 * buffer);
    void   UART_EzI2CSetReadBoundaryBuffer2(uint32 rwBoundary);
#endif /* (UART_SECONDARY_ADDRESS_ENABLE_CONST) */

#if(UART_EZI2C_SCL_STRETCH_ENABLE_CONST)
    CY_ISR_PROTO(UART_EZI2C_STRETCH_ISR);
#endif /* (UART_EZI2C_SCL_STRETCH_ENABLE_CONST) */

#if(UART_EZI2C_SCL_STRETCH_DISABLE_CONST)
    CY_ISR_PROTO(UART_EZI2C_NO_STRETCH_ISR);
#endif /* (UART_EZI2C_SCL_STRETCH_DISABLE) */


/***************************************
*            API Constants
***************************************/

/* Configuration structure constants */
#define UART_EZI2C_ONE_ADDRESS      (0u)
#define UART_EZI2C_TWO_ADDRESSES    (1u)

#define UART_EZI2C_PRIMARY_ADDRESS  (0u)
#define UART_EZI2C_BOTH_ADDRESSES   (1u)

#define UART_EZI2C_SUB_ADDR8_BITS   (0u)
#define UART_EZI2C_SUB_ADDR16_BITS  (1u)

/* UART_EzI2CGetActivity() return constants */
#define UART_EZI2C_STATUS_SECOND_OFFSET (1u)
#define UART_EZI2C_STATUS_READ1     ((uint32) (UART_INTR_SLAVE_I2C_NACK))        /* Bit [1]   */
#define UART_EZI2C_STATUS_WRITE1    ((uint32) (UART_INTR_SLAVE_I2C_WRITE_STOP))  /* Bit [3]   */
#define UART_EZI2C_STATUS_READ2     ((uint32) (UART_INTR_SLAVE_I2C_NACK >> \
                                                           UART_EZI2C_STATUS_SECOND_OFFSET)) /* Bit [0]   */
#define UART_EZI2C_STATUS_WRITE2    ((uint32) (UART_INTR_SLAVE_I2C_WRITE_STOP >> \
                                                           UART_EZI2C_STATUS_SECOND_OFFSET)) /* Bit [2]   */
#define UART_EZI2C_STATUS_ERR       ((uint32) (0x10u))                                       /* Bit [4]   */
#define UART_EZI2C_STATUS_BUSY      ((uint32) (0x20u))                                       /* Bit [5]   */
#define UART_EZI2C_CLEAR_STATUS     ((uint32) (0x1Fu))                                       /* Bits [0-4]*/
#define UART_EZI2C_CMPLT_INTR_MASK  ((uint32) (UART_INTR_SLAVE_I2C_NACK | \
                                                           UART_INTR_SLAVE_I2C_WRITE_STOP))


/***************************************
*     Vars with External Linkage
***************************************/

#if(UART_SCB_MODE_UNCONFIG_CONST_CFG)
    extern const UART_EZI2C_INIT_STRUCT UART_configEzI2C;
#endif /* (UART_SCB_MODE_UNCONFIG_CONST_CFG) */


/***************************************
*           FSM states
***************************************/

/* Returns to master when there is no data to transmit */
#define UART_EZI2C_OVFL_RETURN  (0xFFu)

/* States of EZI2C Slave FSM */
#define UART_EZI2C_FSM_OFFSET_HI8 ((uint8) (0x02u)) /* Idle state for 1 addr: waits for high byte offset */
#define UART_EZI2C_FSM_OFFSET_LO8 ((uint8) (0x00u)) /* Idle state for 2 addr: waits for low byte offset  */
#define UART_EZI2C_FSM_BYTE_WRITE ((uint8) (0x01u)) /* Data byte write state: byte by byte mode          */
#define UART_EZI2C_FSM_WAIT_STOP  ((uint8) (0x03u)) /* Discards written bytes as they do not match write
                                                                   criteria */
#define UART_EZI2C_FSM_WRITE_MASK ((uint8) (0x01u)) /* Incorporates write states: EZI2C_FSM_BYTE_WRITE and
                                                                   EZI2C_FSM_WAIT_STOP  */

#define UART_EZI2C_FSM_IDLE     ((UART_SUB_ADDRESS_SIZE16) ?      \
                                                (UART_EZI2C_FSM_OFFSET_HI8) : \
                                                (UART_EZI2C_FSM_OFFSET_LO8))

#define UART_EZI2C_CONTINUE_TRANSFER    (1u)
#define UART_EZI2C_COMPLETE_TRANSFER    (0u)

#define UART_EZI2C_NACK_RECEIVED_ADDRESS  (0u)
#define UART_EZI2C_ACK_RECEIVED_ADDRESS   (1u)

#define UART_EZI2C_ACTIVE_ADDRESS1  (0u)
#define UART_EZI2C_ACTIVE_ADDRESS2  (1u)


/***************************************
*       Macro Definitions
***************************************/

/* Access to global variables */
#if(UART_SECONDARY_ADDRESS_ENABLE_CONST)

    #define UART_EZI2C_UPDATE_LOC_STATUS(activeAddress, locStatus) \
            do{ \
                (locStatus) >>= (activeAddress); \
            }while(0)

    #define UART_EZI2C_GET_INDEX(activeAddress) \
            ((UART_EZI2C_ACTIVE_ADDRESS1 == (activeAddress)) ? \
                ((uint32) UART_indexBuf1) :                    \
                ((uint32) UART_indexBuf2))

    #define UART_EZI2C_GET_OFFSET(activeAddress) \
            ((UART_EZI2C_ACTIVE_ADDRESS1 == (activeAddress)) ? \
                ((uint32) UART_offsetBuf1) :                   \
                ((uint32) UART_offsetBuf2))

    #define UART_EZI2C_SET_INDEX(activeAddress, locIndex) \
            do{ \
                if(UART_EZI2C_ACTIVE_ADDRESS1 == (activeAddress)) \
                {    \
                    UART_indexBuf1 = (uint16) (locIndex); \
                }    \
                else \
                {    \
                    UART_indexBuf2 = (uint16) (locIndex); \
                }    \
            }while(0)

    #define UART_EZI2C_SET_OFFSET(activeAddress, locOffset) \
            do{ \
                if(UART_EZI2C_ACTIVE_ADDRESS1 == (activeAddress)) \
                {    \
                    UART_offsetBuf1 = (uint16) (locOffset); \
                }    \
                else \
                {    \
                    UART_offsetBuf2 = (uint16) (locOffset); \
                }    \
            }while(0)
#else
    #define UART_EZI2C_UPDATE_LOC_STATUS(activeAddress, locStatus)  do{ /* Empty*/ }while(0)

    #define UART_EZI2C_GET_INDEX(activeAddress)  ((uint32) (UART_indexBuf1))

    #define UART_EZI2C_GET_OFFSET(activeAddress) ((uint32) (UART_offsetBuf1))

    #define UART_EZI2C_SET_INDEX(activeAddress, locIndex) \
            do{ \
                UART_indexBuf1 = (uint16) (locIndex); \
            }while(0)

    #define UART_EZI2C_SET_OFFSET(activeAddress, locOffset) \
            do{ \
                UART_offsetBuf1 = (uint16) (locOffset); \
            }while(0)

#endif  /* (UART_SUB_ADDRESS_SIZE16_CONST) */


/* This macro only applicable for EZI2C slave without clock stretching.
* It should not be used for other pusposes. */
#define UART_EZI2C_TX_FIFO_CTRL_SET   (UART_EZI2C_TX_FIFO_CTRL | \
                                                   UART_TX_FIFO_CTRL_CLEAR)
#define UART_EZI2C_TX_FIFO_CTRL_CLEAR (UART_EZI2C_TX_FIFO_CTRL)
#define UART_FAST_CLEAR_TX_FIFO \
                            do{             \
                                UART_TX_FIFO_CTRL_REG = UART_EZI2C_TX_FIFO_CTRL_SET;   \
                                UART_TX_FIFO_CTRL_REG = UART_EZI2C_TX_FIFO_CTRL_CLEAR; \
                            }while(0)


/***************************************
*      Common Register Settings
***************************************/

#define UART_CTRL_EZI2C     (UART_CTRL_MODE_I2C)

#define UART_EZI2C_CTRL     (UART_I2C_CTRL_S_GENERAL_IGNORE | \
                                         UART_I2C_CTRL_SLAVE_MODE)

#define UART_EZI2C_CTRL_AUTO    (UART_I2C_CTRL_S_READY_ADDR_ACK      | \
                                             UART_I2C_CTRL_S_READY_DATA_ACK      | \
                                             UART_I2C_CTRL_S_NOT_READY_ADDR_NACK | \
                                             UART_I2C_CTRL_S_NOT_READY_DATA_NACK)

#define UART_EZI2C_RX_CTRL  ((UART_FIFO_SIZE - 1u)   | \
                                          UART_RX_CTRL_MSB_FIRST | \
                                          UART_RX_CTRL_ENABLED)

#define UART_EZI2C_TX_FIFO_CTRL (2u)
#define UART_TX_LOAD_SIZE       (2u)

#define UART_EZI2C_TX_CTRL  ((UART_FIFO_SIZE - 1u)   | \
                                          UART_TX_CTRL_MSB_FIRST | \
                                          UART_TX_CTRL_ENABLED)

#define UART_EZI2C_INTR_SLAVE_MASK  (UART_INTR_SLAVE_I2C_BUS_ERROR | \
                                                 UART_INTR_SLAVE_I2C_ARB_LOST  | \
                                                 UART_INTR_SLAVE_I2C_STOP)

#define UART_INTR_SLAVE_COMPLETE    (UART_INTR_SLAVE_I2C_STOP | \
                                                 UART_INTR_SLAVE_I2C_NACK | \
                                                 UART_INTR_SLAVE_I2C_WRITE_STOP)


/***************************************
*    Initialization Register Settings
***************************************/

#if(UART_SCB_MODE_EZI2C_CONST_CFG)

    #define UART_EZI2C_DEFAULT_CTRL \
                                (UART_GET_CTRL_BYTE_MODE  (UART_EZI2C_BYTE_MODE_ENABLE)    | \
                                 UART_GET_CTRL_ADDR_ACCEPT(UART_EZI2C_NUMBER_OF_ADDRESSES) | \
                                 UART_GET_CTRL_EC_AM_MODE (UART_EZI2C_WAKE_ENABLE))

    #if(UART_EZI2C_SCL_STRETCH_ENABLE_CONST)
        #define UART_EZI2C_DEFAULT_I2C_CTRL (UART_EZI2C_CTRL)
    #else
        #define UART_EZI2C_DEFAULT_I2C_CTRL (UART_EZI2C_CTRL_AUTO | UART_EZI2C_CTRL)
    #endif /* (UART_EZI2C_SCL_STRETCH_ENABLE_CONST) */


    #define UART_EZI2C_DEFAULT_RX_MATCH \
                                (UART_GET_I2C_8BIT_ADDRESS(UART_EZI2C_PRIMARY_SLAVE_ADDRESS) | \
                                 UART_GET_RX_MATCH_MASK   (UART_EZI2C_SLAVE_ADDRESS_MASK))

    #define UART_EZI2C_DEFAULT_RX_CTRL  (UART_EZI2C_RX_CTRL)
    #define UART_EZI2C_DEFAULT_TX_CTRL  (UART_EZI2C_TX_CTRL)

    #define UART_EZI2C_DEFAULT_RX_FIFO_CTRL (0u)
    #if(UART_EZI2C_SCL_STRETCH_ENABLE_CONST)
        #define UART_EZI2C_DEFAULT_TX_FIFO_CTRL (0u)
    #else
        #define UART_EZI2C_DEFAULT_TX_FIFO_CTRL (2u)
    #endif /* (UART_EZI2C_SCL_STRETCH_ENABLE_CONST) */

    /* Interrupt sources */
    #define UART_EZI2C_DEFAULT_INTR_I2C_EC_MASK (UART_NO_INTR_SOURCES)
    #define UART_EZI2C_DEFAULT_INTR_SPI_EC_MASK (UART_NO_INTR_SOURCES)
    #define UART_EZI2C_DEFAULT_INTR_MASTER_MASK (UART_NO_INTR_SOURCES)
    #define UART_EZI2C_DEFAULT_INTR_TX_MASK     (UART_NO_INTR_SOURCES)

    #if(UART_EZI2C_SCL_STRETCH_ENABLE_CONST)
        #define UART_EZI2C_DEFAULT_INTR_RX_MASK     (UART_NO_INTR_SOURCES)
        #define UART_EZI2C_DEFAULT_INTR_SLAVE_MASK  (UART_INTR_SLAVE_I2C_ADDR_MATCH | \
                                                                 UART_EZI2C_INTR_SLAVE_MASK)
    #else
        #define UART_EZI2C_DEFAULT_INTR_RX_MASK     (UART_INTR_RX_NOT_EMPTY)
        #define UART_EZI2C_DEFAULT_INTR_SLAVE_MASK  (UART_INTR_SLAVE_I2C_START | \
                                                                 UART_EZI2C_INTR_SLAVE_MASK)
    #endif /* (UART_EZI2C_SCL_STRETCH_ENABLE_CONST) */

#endif /* (UART_SCB_MODE_EZI2C_CONST_CFG) */

#endif /* (CY_SCB_EZI2C_UART_H) */


/* [] END OF FILE */
