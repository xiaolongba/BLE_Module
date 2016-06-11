/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>
#include "BLEApplication.h"
#include "BLEClient.h"
#include<stdio.h>
#include<string.h>
#include "WriteUserSFlash.h"
/***************************************
* 外部全局变量声明
***************************************/
//extern uint8_t  StartScan;
//extern uint8_t DeviceList[DeviceListCount];
//extern AdvDataInfo DeviceInfo;
extern uint8_t DeviceCount;
extern uint8_t LowPower_EN;
//extern char* StringArry[];
extern uint8_t rx_isover;
extern uint8_t RX_BUFFER[BUFFER_LENGHTH];
extern uint8_t Buffer_Length;
extern uint8_t charNotificationEnabled;
extern uint8_t _10sIsOver;
extern CYBLE_GATTC_WRITE_REQ_T writeRequestData;
extern uint8_t Role;
extern uint8_t TroughtTest_Flag;
extern uint8_t CommandMode;
extern uint8 buffer[20];
extern uint16 negotiatedMtu;
extern char SetDeviceName[23];
extern uint8_t Bond;
extern uint8_t ConnStaus;
extern CYBLE_GAP_BD_ADDR_T clearAllDevices;
#define WDT_COUNTER                 (CY_SYS_WDT_COUNTER1)
#define WDT_COUNTER_MASK            (CY_SYS_WDT_COUNTER1_MASK)
#define WDT_INTERRUPT_SOURCE        (CY_SYS_WDT_COUNTER1_INT) 
#define WDT_COUNTER_ENABLE          (1u)
#define WDT_1SEC                    (32767u)
//extern uint8_t DeviceConned;
//extern CYBLE_CONN_HANDLE_T	connHandle;

/*******************************************************************************
* Function Name: Timer_Interrupt
********************************************************************************
*
* Summary:
*  Handles the Interrupt Service Routine for the WDT timer.
*  It is called from common WDT ISR located in BLE component. 
*
*******************************************************************************/
void Timer_Interrupt(void)
{
    if(CySysWdtGetInterruptSource() & WDT_INTERRUPT_SOURCE)
    {
//        static uint8 led = LED_OFF;
//        
//        /* Blink LED to indicate that device advertises */
//        if(CYBLE_STATE_ADVERTISING == CyBle_GetState())
//        {
//            led ^= LED_OFF;
//            Advertising_LED_Write(led);
//        }
//        
//        /* Indicate that timer is raised to the main loop */
//        mainTimer++;
        
        /* Clears interrupt request  */
//        CySysWatchdogFeed(WDT_COUNTER);
//        CySysWdtClearInterrupt(WDT_INTERRUPT_SOURCE);
    }
}

/*******************************************************************************
* Function Name: WDT_Start
********************************************************************************
*
* Summary:
*  Configures WDT(counter 2) to trigger an interrupt every second.
*
*******************************************************************************/

void WDT_Start(void)
{
    /* Unlock the WDT registers for modification */
    CySysWdtUnlock(); 
    /* Setup ISR callback */
    Wdt_Isr_StartEx(Timer_Interrupt);
    /* Write the mode to generate interrupt on match */
    CySysWdtWriteMode(WDT_COUNTER, CY_SYS_WDT_MODE_INT_RESET);
    /* Configure the WDT counter clear on a match setting */
    CySysWdtWriteClearOnMatch(WDT_COUNTER, WDT_COUNTER_ENABLE);
    /* Configure the WDT counter match comparison value */
    CySysWdtWriteMatch(WDT_COUNTER, WDT_1SEC);//定时1秒
    /* Reset WDT counter */
    CySysWdtResetCounters(CY_SYS_WDT_COUNTER1_RESET);
    /* Enable the specified WDT counter */
    CySysWdtEnable(WDT_COUNTER_MASK);
    /* Lock out configuration changes to the Watchdog timer registers */
//    CySysWdtLock();    //不要锁定,否则无法正常喂狗
}

int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    CYBLE_GAP_BONDED_DEV_ADDR_LIST_T bondedDevList;
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
//    uint8_t test[20]={0};
//    uint32 test=12345;
    if(Central==ReadDataFromSFlash())//每次复位均要从SFlash中读取模式标志位来判断复位是主还是从模式
    {
        Role=Central;
    }
    else
    {
        Role=Peripheral;
    }
    if(ReadDeviceNameFromSFlash())
    {
        CyBle_GapSetLocalName(SetDeviceName);
    }
    SystemInitialization(); 
    WDT_Start();
//     printf("+AUTHKEY=%6ld\r\n",test);
    for(;;)
    {
        
        /* Place your application code here. */ 
//        CyDelay(10);
//        if(rx_isover)
//        {
//            rx_isover=FALSE;
//            while(0!=UART_SpiUartGetRxBufferSize())
//            {                
//                RX_BUFFER[Buffer_Length]=UART_UartGetByte();//读取串口接收缓冲区的数据
//                Buffer_Length++;
////                UART_UartPutChar(UART_UartGetByte());
//            }
//            //串口接收的数据长度清零,以便下一条命令过来时还是存放在数组首地址
//            Buffer_Length=0;
//            Parser_UartData((char*)RX_BUFFER);
//            memset(RX_BUFFER,0,sizeof(RX_BUFFER));
//        }
//        if(UART_GetRxInterruptSourceMasked()&UART_INTR_RX_NOT_EMPTY)
//        {
//            
//        }
        CySysWdtResetCounters(CY_SYS_WDT_COUNTER1_RESET);//喂狗
        Master_Slave_UartHandler(Role);
        if(ConnStaus)
        {
            ConnStaus = FALSE;
            CyBle_GapGetBondedDevicesList(&bondedDevList);//防止即使掉电后，再次发起清除绑定命令仍然有效
            if(bondedDevList.count)
                Bond=TURE;
            else
                Bond=FALSE;
            CyBle_GapRemoveDeviceFromWhiteList(&clearAllDevices);//清除绑定先从白名单中清除再清除Flash中的信息
            while(CYBLE_ERROR_OK != CyBle_StoreBondingData(1));
//            printf("+BOND=0\r\n");//清除绑定成功
            Bond=FALSE;
        }
//        if(DeviceConned)
//        {
//            DeviceConned = FALSE;
//            range.startHandle=0x0001;//开始句柄不能从0开始，只能从1开始
//            range.endHandle=0xFFFF;  
//            CyBle_GattcDiscoverAllCharacteristics(cyBle_connHandle,range);//一旦连接完成就开始查找从机的所有的特征值                
//            CyBle_ProcessEvents();
//        }
//      保存Peer Device的信息到Flash中，即绑定主机端的信息，方便下次快速重连时不用再配对
        #ifdef RELEASE
        if((cyBle_pendingFlashWrite != 0u) &&
           ((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) == 0u))
        {
            // Store Bonding informtation to flash             
            if ( CyBle_StoreBondingData(0u) == CYBLE_ERROR_OK)
            {
                if(Bond==FALSE)
                {
                    Bond=TURE;
                    printf("+BOND=1\r\n");
                }
            }
//            else
//            {
////                printf ("Bonding data storing pending\r\n");
//            }
        }
        #endif
        //主机处理10s定时器,10秒内给从机发送的数据，测试透传速率使用
//        if(_10sIsOver)
//        {
//            _10sIsOver=FALSE;
//            memcpy(test,buffer,20);
//            writeRequestData.value.val=test;
//            writeRequestData.value.len=20;
//            writeRequestData.attrHandle=CYBLE_TROUGHPUT_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE;
//            CyBle_GattcWriteWithoutResponse(cyBle_connHandle, &writeRequestData);
////            writeRequestData.value.val[0]=0x00;
////            writeRequestData.attrHandle=CYBLE_TROUGHPUT_SERVICE_SPEEDTEST_CHAR_HANDLE;
////            CyBle_GattcWriteCharacteristicValue(cyBle_connHandle, &writeRequestData);   
//            CyBle_ProcessEvents(); 
//        }
        //        测试透传速率用
//        if((TroughtTest_Flag == TURE)&&(Role == Peripheral))
//        {
//            /* Send new data only when the previous data has gone out, 
//             * which is indicated by Stack being Free.
//             */
////            printf("SendNotification\r\n");
//            if(CyBle_GattGetBusStatus() == CYBLE_STACK_STATE_FREE)
//            {
//                SendNotification(buffer,negotiated Mtu);                
//            }
//        }
//        if(LowPower_EN)
//        {
//            CyBle_ProcessEvents();            
//            LowPowerManagement();
//        }
        
//        #ifdef LOW_POWER_EN
//        /* Call CyBle_ProcessEvents to process all events before going to sleep. */
//		CyBle_ProcessEvents();            
//        LowPowerManagement();
//        #endif    
        CyBle_ProcessEvents();
    }
}

/* [] END OF FILE */
