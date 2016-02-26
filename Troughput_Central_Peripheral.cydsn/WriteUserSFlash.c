/*******************************************************************************
* File Name: WriteUserSFlash.c
*
* Version: 1.0
*
* Description:
*  This file contains the source code for User SFlash Write API.
*
* Owner:
*  sey@cypress.com, kris@cypress.com
*
* Hardware Dependency:
*  1. PSoC 4 BLE device
*
* Code Tested With:
*  1. PSoC Creator 3.1 SP1
*  2. ARM GCC 4.8.4
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <Project.h>
#include <WriteUserSFlash.h>

/***************************************
* 全局变量
***************************************/
char SetDeviceName[23]={0};
/*******************************************************************************
* Function Name: WriteUserSFlashRow
********************************************************************************
* Summary:
*        This routine calls the PSoC 4 BLE device supervisory ROM APIs to update
* the user configuration area of Supervisory Flash (SFlash).  
*
* Parameters:
*  userRowNUmber - User config SFlash row number to which data is to be written
*  dataPointer - Pointer to the data to be written. This API writes one row of
*                user config SFlash row at a time.
*
* Return:
*  uint32 - state of the user config SFlash write operation.
*
*******************************************************************************/
#if defined (__GNUC__)
#pragma GCC optimize ("O0")
#endif /* End of #if defined (__GNUC__) */
uint32 WriteUserSFlashRow(uint8 userRowNUmber, uint32 *dataPointer,uint8_t datalength,uint8_t address)
{    
    if(userRowNUmber==0)
    {
        if(address<6)
            return USER_SFLASH_WRITE_FAIL;
    }
    uint8 localCount;
	volatile uint32 retValue=0;
//	volatile uint32 cmdDataBuffer[(CY_FLASH_SIZEOF_ROW/4) + 2];
    volatile uint32 cmdDataBuffer[datalength+2];
	volatile uint32 reg1,reg2,reg3,reg4,reg5,reg6;
	
	/* Store the clock settings temporarily */
    reg1 =	CY_GET_XTND_REG32((void CYFAR *)(CYREG_CLK_SELECT));
    reg2 =  CY_GET_XTND_REG32((void CYFAR *)(CYREG_CLK_IMO_CONFIG));
    reg3 =  CY_GET_XTND_REG32((void CYFAR *)(CYREG_PWR_BG_TRIM4));
    reg4 =  CY_GET_XTND_REG32((void CYFAR *)(CYREG_PWR_BG_TRIM5));
    reg5 =  CY_GET_XTND_REG32((void CYFAR *)(CYREG_CLK_IMO_TRIM1));
    reg6 =  CY_GET_XTND_REG32((void CYFAR *)(CYREG_CLK_IMO_TRIM2));
	
	/* Initialize the clock necessary for flash programming */
	CY_SET_REG32(CYREG_CPUSS_SYSARG, 0x0000e8b6);
	CY_SET_REG32(CYREG_CPUSS_SYSREQ, 0x80000015);
	
	/******* Initialize SRAM parameters for the LOAD FLASH command ******/
	/* byte 3 (i.e. 00) is the Macro_select */
	/* byte 2 (i.e. 00) is the Start addr of page latch 
       如果是第0行，前6个字节不能修改，否则MAC地址会丢失*/
	/* byte 1 (i.e. d7) is the key 2  */
	/* byte 0 (i.e. b6) is the key 1  */
  	cmdDataBuffer[0]=0x0000d7b6|(address<<16);
	/****** Initialize SRAM parameters for the LOAD FLASH command ******/
	/* byte 3,2 and 1 are null */
	/* byte 0 (i.e. 7F) is the number of bytes to be written */
	cmdDataBuffer[1]=(0x00000000|datalength)-1;	     
	/* Initialize the SRAM buffer with data bytes */
//    cmdDataBuffer[2] = *dataPointer;
    for(localCount = 0; localCount < datalength; localCount++)    
	{
		cmdDataBuffer[localCount + 2] = dataPointer[localCount]; 
	}
	
	/* Write the following to registers to execute a LOAD FLASH bytes */
	CY_SET_REG32(CYREG_CPUSS_SYSARG, &cmdDataBuffer[0]);
	CY_SET_REG32(CYREG_CPUSS_SYSREQ, LOAD_FLASH);
	
    /****** Initialize SRAM parameters for the WRITE ROW command ******/
	/* byte 3 & 2 are null */
	/* byte 1 (i.e. 0xeb) is the key 2  */
	/* byte 0 (i.e. 0xb6) is the key 1  */
	cmdDataBuffer[0] = 0x0000ebb6;
    
	/* byte 7,6 and 5 are null */
	/* byte 4 is desired SFlash user row 
	 * Allowed values 0 - row 4
	                  1 - row 5 
					  2 - row 6
					  3 - row 7 */
	cmdDataBuffer[1] = (uint32) userRowNUmber;
	
	/* Write the following to registers to execute a WRITE USER SFlash ROW command */
	CY_SET_REG32(CYREG_CPUSS_SYSARG, &cmdDataBuffer[0]);
	CY_SET_REG32(CYREG_CPUSS_SYSREQ, WRITE_USER_SFLASH_ROW);
    
	/* Read back SYSARG for the result. 0xA0000000 = SUCCESS; */
	retValue = CY_GET_REG32(CYREG_CPUSS_SYSARG);
	
	/* Restore the clock settings after the flash programming is done */
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_CLK_SELECT),reg1);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_CLK_IMO_CONFIG),reg2);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_PWR_BG_TRIM4),reg3);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_PWR_BG_TRIM5),reg4);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_CLK_IMO_TRIM1),reg5);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_CLK_IMO_TRIM2),reg6);  
	
	return retValue;
}
#if defined (__GNUC__)
#pragma GCC reset_options
#endif /* End of #if defined (__GNUC__) */

/*******************************************************************************
* Function Name: ReadDataFromSFlash
********************************************************************************
*
*
* 
*  读取SFlash中的第一个数据
* \param  none
* \return
*  None
*       SFlash中有506Bytes字节的用户可配置的空间
******************************************************************************/
uint8_t ReadDataFromSFlash()
{
    
    uint8_t FlashData=0;
    uint8_t *sflashPtr=(uint8_t *)USER_SFLASH_BASE_ADDRESS;//SFlash的首地址
    sflashPtr=sflashPtr+6;
    FlashData=*sflashPtr;
    return FlashData;    
}

/*******************************************************************************
* Function Name: ReadDeviceNameFromSFlash
********************************************************************************
*
*
* 
*  读取存放在SFlash中的设备名
* \param  none
* \return
*  None
*       SFlash中有506Bytes字节的用户可配置的空间
******************************************************************************/
uint8_t ReadDeviceNameFromSFlash()
{
    uint8_t FlashData=0;
    uint8_t *sflashPtr=(uint8_t *)USER_SFLASH_BASE_ADDRESS;//SFlash的首地址
    FlashData=*(sflashPtr+128);//取得存放在SFlash的设备名的长度
    if(FlashData==0)
        return 0;
    else
    {
        memcpy(SetDeviceName,sflashPtr+128*2,FlashData); 
        return 1;
    }
}

/* [] END OF FILE */
