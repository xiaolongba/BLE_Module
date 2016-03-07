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
//extern CYBLE_CONN_HANDLE_T	connHandle;
int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */
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
        Master_Slave_UartHandler(Role);
//      保存Peer Device的信息到Flash中，即绑定主机端的信息，方便下次快速重连时不用再配对
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
//                SendNotification(buffer,negotiatedMtu);                
//            }
//        }
        if(LowPower_EN)
        {
            CyBle_ProcessEvents();            
            LowPowerManagement();
        }
        
//        #ifdef LOW_POWER_EN
//        /* Call CyBle_ProcessEvents to process all events before going to sleep. */
//		CyBle_ProcessEvents();            
//        LowPowerManagement();
//        #endif    
        CyBle_ProcessEvents();
    }
}

/* [] END OF FILE */
