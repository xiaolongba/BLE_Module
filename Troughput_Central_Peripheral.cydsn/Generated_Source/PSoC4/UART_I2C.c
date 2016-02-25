/*******************************************************************************
* File Name: UART_I2C.c
* Version 3.0
*
* Description:
*  This file provides the source code to the API for the SCB Component in
*  I2C mode.
*
* Note:
*
*******************************************************************************
* Copyright 2013-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "UART_PVT.h"
#include "UART_I2C_PVT.h"


/***************************************
*      I2C Private Vars
***************************************/

volatile uint8 UART_state;  /* Current state of I2C FSM */

#if(UART_SCB_MODE_UNCONFIG_CONST_CFG)

    /***************************************
    *  Configuration Structure Initialization
    ***************************************/

    /* Constant configuration of I2C */
    const UART_I2C_INIT_STRUCT UART_configI2C =
    {
        UART_I2C_MODE,
        UART_I2C_OVS_FACTOR_LOW,
        UART_I2C_OVS_FACTOR_HIGH,
        UART_I2C_MEDIAN_FILTER_ENABLE,
        UART_I2C_SLAVE_ADDRESS,
        UART_I2C_SLAVE_ADDRESS_MASK,
        UART_I2C_ACCEPT_ADDRESS,
        UART_I2C_WAKE_ENABLE,
        UART_I2C_BYTE_MODE_ENABLE,
        UART_I2C_DATA_RATE,
        UART_I2C_ACCEPT_GENERAL_CALL,
    };

    /*******************************************************************************
    * Function Name: UART_I2CInit
    ********************************************************************************
    *
    * Summary:
    *  Configures the SCB for I2C operation.
    *
    * Parameters:
    *  config:  Pointer to a structure that contains the following ordered list of
    *           fields. These fields match the selections available in the
    *           customizer.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void UART_I2CInit(const UART_I2C_INIT_STRUCT *config)
    {
        uint32 medianFilter;
        uint32 locEnableWake;

        if(NULL == config)
        {
            CYASSERT(0u != 0u); /* Halt execution due to bad function parameter */
        }
        else
        {
            /* Configure pins */
            UART_SetPins(UART_SCB_MODE_I2C, UART_DUMMY_PARAM,
                                     UART_DUMMY_PARAM);

            /* Store internal configuration */
            UART_scbMode       = (uint8) UART_SCB_MODE_I2C;
            UART_scbEnableWake = (uint8) config->enableWake;
            UART_scbEnableIntr = (uint8) UART_SCB_IRQ_INTERNAL;

            UART_mode          = (uint8) config->mode;
            UART_acceptAddr    = (uint8) config->acceptAddr;

        #if (UART_CY_SCBIP_V0)
            /* Adjust SDA filter settings. Ticket ID#150521 */
            UART_SET_I2C_CFG_SDA_FILT_TRIM(UART_EC_AM_I2C_CFG_SDA_FILT_TRIM);
        #endif /* (UART_CY_SCBIP_V0) */

            /* Adjust AF and DF filter settings. Ticket ID#176179 */
            if (((UART_I2C_MODE_SLAVE != config->mode) &&
                 (config->dataRate <= UART_I2C_DATA_RATE_FS_MODE_MAX)) ||
                 (UART_I2C_MODE_SLAVE == config->mode))
            {
                /* AF = 1, DF = 0 */
                UART_I2C_CFG_ANALOG_FITER_ENABLE;
                medianFilter = UART_DIGITAL_FILTER_DISABLE;
            }
            else
            {
                /* AF = 0, DF = 1 */
                UART_I2C_CFG_ANALOG_FITER_DISABLE;
                medianFilter = UART_DIGITAL_FILTER_ENABLE;
            }

        #if (!UART_CY_SCBIP_V0)
            locEnableWake = (UART_I2C_MULTI_MASTER_SLAVE) ? (0u) : (config->enableWake);
        #else
            locEnableWake = config->enableWake;
        #endif /* (!UART_CY_SCBIP_V0) */

            /* Configure I2C interface */
            UART_CTRL_REG     = UART_GET_CTRL_BYTE_MODE  (config->enableByteMode) |
                                            UART_GET_CTRL_ADDR_ACCEPT(config->acceptAddr)     |
                                            UART_GET_CTRL_EC_AM_MODE (locEnableWake);

            UART_I2C_CTRL_REG = UART_GET_I2C_CTRL_HIGH_PHASE_OVS(config->oversampleHigh) |
                    UART_GET_I2C_CTRL_LOW_PHASE_OVS (config->oversampleLow)                          |
                    UART_GET_I2C_CTRL_S_GENERAL_IGNORE((uint32)(0u == config->acceptGeneralAddr))    |
                    UART_GET_I2C_CTRL_SL_MSTR_MODE  (config->mode);

            /* Configure RX direction */
            UART_RX_CTRL_REG      = UART_GET_RX_CTRL_MEDIAN(medianFilter) |
                                                UART_I2C_RX_CTRL;
            UART_RX_FIFO_CTRL_REG = UART_CLEAR_REG;

            /* Set default address and mask */
            UART_RX_MATCH_REG    = ((UART_I2C_SLAVE) ?
                                                (UART_GET_I2C_8BIT_ADDRESS(config->slaveAddr) |
                                                 UART_GET_RX_MATCH_MASK(config->slaveAddrMask)) :
                                                (UART_CLEAR_REG));


            /* Configure TX direction */
            UART_TX_CTRL_REG      = UART_I2C_TX_CTRL;
            UART_TX_FIFO_CTRL_REG = UART_CLEAR_REG;

            /* Configure interrupt with I2C handler but do not enable it */
            CyIntDisable    (UART_ISR_NUMBER);
            CyIntSetPriority(UART_ISR_NUMBER, UART_ISR_PRIORITY);
            (void) CyIntSetVector(UART_ISR_NUMBER, &UART_I2C_ISR);

            /* Configure interrupt sources */
        #if(!UART_CY_SCBIP_V1)
            UART_INTR_SPI_EC_MASK_REG = UART_NO_INTR_SOURCES;
        #endif /* (!UART_CY_SCBIP_V1) */

            UART_INTR_I2C_EC_MASK_REG = UART_NO_INTR_SOURCES;
            UART_INTR_RX_MASK_REG     = UART_NO_INTR_SOURCES;
            UART_INTR_TX_MASK_REG     = UART_NO_INTR_SOURCES;

            UART_INTR_SLAVE_MASK_REG  = ((UART_I2C_SLAVE) ?
                            (UART_GET_INTR_SLAVE_I2C_GENERAL(config->acceptGeneralAddr) |
                             UART_I2C_INTR_SLAVE_MASK) : (UART_CLEAR_REG));

            UART_INTR_MASTER_MASK_REG = ((UART_I2C_MASTER) ?
                                                     (UART_I2C_INTR_MASTER_MASK) :
                                                     (UART_CLEAR_REG));

            /* Configure global variables */
            UART_state = UART_I2C_FSM_IDLE;

            /* Internal slave variables */
            UART_slStatus        = 0u;
            UART_slRdBufIndex    = 0u;
            UART_slWrBufIndex    = 0u;
            UART_slOverFlowCount = 0u;

            /* Internal master variables */
            UART_mstrStatus     = 0u;
            UART_mstrRdBufIndex = 0u;
            UART_mstrWrBufIndex = 0u;
        }
    }

#else

    /*******************************************************************************
    * Function Name: UART_I2CInit
    ********************************************************************************
    *
    * Summary:
    *  Configures the SCB for the I2C operation.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void UART_I2CInit(void)
    {
    #if(UART_CY_SCBIP_V0)
        /* Adjust SDA filter settings. Ticket ID#150521 */
        UART_SET_I2C_CFG_SDA_FILT_TRIM(UART_EC_AM_I2C_CFG_SDA_FILT_TRIM);
    #endif /* (UART_CY_SCBIP_V0) */

        /* Adjust AF and DF filter settings. Ticket ID#176179 */
        UART_I2C_CFG_ANALOG_FITER_ENABLE_ADJ;

        /* Configure I2C interface */
        UART_CTRL_REG     = UART_I2C_DEFAULT_CTRL;
        UART_I2C_CTRL_REG = UART_I2C_DEFAULT_I2C_CTRL;

        /* Configure RX direction */
        UART_RX_CTRL_REG      = UART_I2C_DEFAULT_RX_CTRL;
        UART_RX_FIFO_CTRL_REG = UART_I2C_DEFAULT_RX_FIFO_CTRL;

        /* Set default address and mask */
        UART_RX_MATCH_REG     = UART_I2C_DEFAULT_RX_MATCH;

        /* Configure TX direction */
        UART_TX_CTRL_REG      = UART_I2C_DEFAULT_TX_CTRL;
        UART_TX_FIFO_CTRL_REG = UART_I2C_DEFAULT_TX_FIFO_CTRL;

        /* Configure interrupt with I2C handler but do not enable it */
        CyIntDisable    (UART_ISR_NUMBER);
        CyIntSetPriority(UART_ISR_NUMBER, UART_ISR_PRIORITY);
    #if(!UART_I2C_EXTERN_INTR_HANDLER)
        (void) CyIntSetVector(UART_ISR_NUMBER, &UART_I2C_ISR);
    #endif /* (UART_I2C_EXTERN_INTR_HANDLER) */

        /* Configure interrupt sources */
    #if(!UART_CY_SCBIP_V1)
        UART_INTR_SPI_EC_MASK_REG = UART_I2C_DEFAULT_INTR_SPI_EC_MASK;
    #endif /* (!UART_CY_SCBIP_V1) */

        UART_INTR_I2C_EC_MASK_REG = UART_I2C_DEFAULT_INTR_I2C_EC_MASK;
        UART_INTR_SLAVE_MASK_REG  = UART_I2C_DEFAULT_INTR_SLAVE_MASK;
        UART_INTR_MASTER_MASK_REG = UART_I2C_DEFAULT_INTR_MASTER_MASK;
        UART_INTR_RX_MASK_REG     = UART_I2C_DEFAULT_INTR_RX_MASK;
        UART_INTR_TX_MASK_REG     = UART_I2C_DEFAULT_INTR_TX_MASK;

        /* Configure global variables */
        UART_state = UART_I2C_FSM_IDLE;

    #if(UART_I2C_SLAVE)
        /* Internal slave variable */
        UART_slStatus        = 0u;
        UART_slRdBufIndex    = 0u;
        UART_slWrBufIndex    = 0u;
        UART_slOverFlowCount = 0u;
    #endif /* (UART_I2C_SLAVE) */

    #if(UART_I2C_MASTER)
    /* Internal master variable */
        UART_mstrStatus     = 0u;
        UART_mstrRdBufIndex = 0u;
        UART_mstrWrBufIndex = 0u;
    #endif /* (UART_I2C_MASTER) */
    }
#endif /* (UART_SCB_MODE_UNCONFIG_CONST_CFG) */


/*******************************************************************************
* Function Name: UART_I2CStop
********************************************************************************
*
* Summary:
*  Resets the I2C FSM into the default state.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void UART_I2CStop(void)
{
    UART_state = UART_I2C_FSM_IDLE;
}


#if(UART_I2C_WAKE_ENABLE_CONST)
    /*******************************************************************************
    * Function Name: UART_I2CSaveConfig
    ********************************************************************************
    *
    * Summary:
    *  Enables UART_INTR_I2C_EC_WAKE_UP interrupt source. This interrupt
    *  triggers on address match and wakes up device.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void UART_I2CSaveConfig(void)
    {
    #if (!UART_CY_SCBIP_V0)
        #if (UART_I2C_MULTI_MASTER_SLAVE_CONST && UART_I2C_WAKE_ENABLE_CONST)
            /* Enable externally clocked address match if it was not enabled before.
            * This applicable only for Multi-Master-Slave. Ticket ID#192742 */
            if (0u == (UART_CTRL_REG & UART_CTRL_EC_AM_MODE))
            {
                /* Enable external address match logic */
                UART_Stop();
                UART_CTRL_REG |= UART_CTRL_EC_AM_MODE;
                UART_Enable();
            }
        #endif /* (UART_I2C_MULTI_MASTER_SLAVE_CONST) */

        #if (UART_SCB_CLK_INTERNAL)
            /* Disable clock to internal address match logic. Ticket ID#187931 */
            UART_SCBCLK_Stop();
        #endif /* (UART_SCB_CLK_INTERNAL) */
    #endif /* (!UART_CY_SCBIP_V0) */

        UART_SetI2CExtClkInterruptMode(UART_INTR_I2C_EC_WAKE_UP);
    }


    /*******************************************************************************
    * Function Name: UART_I2CRestoreConfig
    ********************************************************************************
    *
    * Summary:
    *  Disables UART_INTR_I2C_EC_WAKE_UP interrupt source. This interrupt
    *  triggers on address match and wakes up device.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void UART_I2CRestoreConfig(void)
    {
        /* Disable wakeup interrupt on address match */
        UART_SetI2CExtClkInterruptMode(UART_NO_INTR_SOURCES);

    #if (!UART_CY_SCBIP_V0)
        #if (UART_SCB_CLK_INTERNAL)
            /* Enable clock to internal address match logic. Ticket ID#187931 */
            UART_SCBCLK_Start();
        #endif /* (UART_SCB_CLK_INTERNAL) */
    #endif /* (!UART_CY_SCBIP_V0) */
    }
#endif /* (UART_I2C_WAKE_ENABLE_CONST) */


/* [] END OF FILE */
