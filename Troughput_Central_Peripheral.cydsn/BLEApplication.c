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
#include<project.h>
#include "BLEApplication.h"
#include<stdio.h>
#include "BLEClient.h"
#include <malloc.h>
#include "WriteUserSFlash.h"
/***************************************
* 外部全局变量声明
***************************************/
extern uint8_t StartScan;
extern uint8_t DeviceCount;
extern CYBLE_DEVICE_INFO DeviceInfo[DeviceListCount];
//extern uint8_t DeviceList[DeviceListCount];
/***************************************
* 全局变量
***************************************/
UART_UART_INIT_STRUCT configUart;
uint8_t LowPower_EN=TURE;
uint8_t RX_BUFFER[BUFFER_LENGHTH]={0};
uint8_t Buffer_Length=0;
uint8_t rx_isover=FALSE;
uint8_t RxFlag=OVER;
//uint8_t SleepMode=TURE;
CYBLE_CONN_OR_DISCONN_INFO Conn_Or_Disconn_Info;
uint8_t Conn_or_Disconn_Idx=0;
//uint8 bufferRx[UART_RX_BUFFER + 1u];     /* RX software buffer requires one extra entry for correct operation in UART mode */ 
//uint8 bufferTx[UART_TX_BUFFER];           /* TX software buffer */ 
uint8_t MTU_REQ_FLAG=TURE;
uint8_t TroughtTest_Flag=FALSE;
uint8_t charNotificationEnabled = FALSE;
uint32 totalByteCounter = 0;
uint8_t Role=Peripheral;//默认是Peripheral
uint8 buffer[20];
uint16 negotiatedMtu = DEFAULT_MTU_SIZE;
uint8_t CommandMode=AT_COMMAND_MODE;
uint32 testCount=0;
uint8_t iocapability=CYBLE_GAP_IOCAP_DISPLAY_ONLY;
CYBLE_GAP_CONN_PARAM_UPDATED_IN_CONTROLLER_T ConnParam;
uint8_t KEYBOARD=FALSE;
uint8_t AUTHFLAG=FALSE;
uint8_t SleepMode=TURE;
uint8_t Bond=FALSE;
/* All zeros passed as  argument passed to CyBle_GapRemoveDeviceFromWhiteList for 
removing all the bonding data stored */
CYBLE_GAP_BD_ADDR_T clearAllDevices = {{0,0,0,0,0,0},0};
//CYBLE_GAPP_DISC_MODE_INFO_T updateadvparam;
//uint8_t ExitThrought[]={
//    0x11,0x24,0xFB,0x1E,0x1F,0x24,0x19,0x16,0x29};
//CYBLE_CONN_HANDLE_T	connHandle=
//{
//    CYBLE_TROUGHPUT_SERVICE_CUSTOM_CHARACTERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE,
//    0x00
//};
CYBLE_GAP_BD_ADDR_T DeviceAddr;//存放本机的MAC地址,不能做为局部变量否则获取的地址有误
uint8_t _10sIsOver=FALSE;
uint8 cccdNotifEnabledValue[] = {0x01, 0x00};
CYBLE_GATT_VALUE_T cccdNotifFlagSetStruct =
{
    cccdNotifEnabledValue,
    2,
    0
};
//    更新连接间隔参数，提高透传速率，但会增加功耗
CYBLE_GAP_CONN_UPDATE_PARAM_T connParam={
    0x10,
    0x10,
    0,
    1000
};
CYBLE_GATTC_WRITE_REQ_T writeRequestData;
CYBLE_GATTC_READ_REQ_T readRequestData=CYBLE_TROUGHPUT_SERVICE_CUSTOM_CHARACTERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
//AT命令列表
char * StringArry[]={
                     AT_RESET,AT_REFAC,AT_VERSION,AT_BAUD,AT_LADDR,AT_NAME,
                     AT_CPIN,AT_TXP,AT_ADVD,AT_ADVI,AT_CONNI,AT_ROLE,AT_SCAN,
                     AT_CONNT,AT_DISCONN,AT_LOGIN,AT_TX,AT_RSSI,AT_DISALLCHAR,
                     AT,AT_SPEED,AT_NOTIFY,AT_STAUS,AT_ADVS,AT_AUTH,AT_IOCAP,
                    AT_SLEEP,AT_BOND
                    };
uint8_t STOP_SCAN_FLAG=FALSE;
uint8_t Baud_rate_idx='7';//默认是115200
/******************************************************************************
* Function Name: StackEventHandler
***************************************************************************//**
* 
*  处理蓝牙协议栈事件
* 
* \return
*  None
* 
******************************************************************************/
void StackEventHandler(uint32 eventCode, void *eventParam)
{
    CYBLE_GAPC_ADV_REPORT_T	Scan_Result;
//    CYBLE_API_RESULT_T API_RESULT=0;
    CYBLE_GATTC_FIND_INFO_RSP_PARAM_T DisAllCharResult;  
    uint8_t i=0;
    CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T * handleValueNotification;
    CYBLE_GATTS_WRITE_REQ_PARAM_T writeParam;  
//    CYBLE_GATT_HANDLE_VALUE_PAIR_T handleValuePair;
//    uint32 PassKey=0;
//    CYBLE_GATTC_READ_RSP_PARAM_T readParam;
    switch(eventCode)
    {
        case CYBLE_EVT_STACK_ON:
            printf("BLE is Ready\r\n");
            while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完成                
//            StartScan=TRUE;     
//            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
        break;
        case CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT:
            Scan_Result=*(CYBLE_GAPC_ADV_REPORT_T *)eventParam;
            HandleScanDevices(&Scan_Result);               
//            CyBle_GapcStopScan();
        break;
        case CYBLE_EVT_GAPC_SCAN_START_STOP:
            if(STOP_SCAN_FLAG)
            {
                STOP_SCAN_FLAG=FALSE;             
                printf("+SCAN_STOP\r\n");
            }
            
        break;
        //当Client扫描到Server的DB之后才会触发该事件，前提是client和server连接上
        case CYBLE_EVT_GATTC_DISCOVERY_COMPLETE:
            
        break;        
        case CYBLE_EVT_TIMEOUT:                         
            if(*(uint8_t*)eventParam==CYBLE_GAP_SCAN_TO)
            {   
                CyBle_GapcStopScan();
                printf("+SCAN_CMP\r\n");                    
            }
            //广播超时
//            if(*(uint8_t*)eventParam==CYBLE_GAP_ADV_MODE_TO)
//            {
//               printf("+ADV_TO\r\n");  
////                CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_SLOW);
//            }
        break;
        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            if(CYBLE_STATE_ADVERTISING!=CyBle_GetState())
            {
                CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_SLOW);
            }
        break;
        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
//            获取连接参数信息
            ConnParam=*(CYBLE_GAP_CONN_PARAM_UPDATED_IN_CONTROLLER_T *)eventParam;            
            if(Role==Peripheral)
            {
                printf("+CONN_EVT=OK\r\n");
//                handleValuePair.value.val[0]=0x01;
//                handleValuePair.value.val[1]=0x00;
//                handleValuePair.value.len=2;
//                handleValuePair.attrHandle=CYBLE_TROUGHPUT_SERVICE_CUSTOM_CHARACTERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
//                 printf("+WRITE=%d\r\n",CyBle_GattsWriteAttributeValue(&handleValuePair,0,&cyBle_connHandle,CYBLE_GATT_DB_PEER_INITIATED));                
//                CommandMode=THROUGHT_MODE;
                CyBle_L2capLeConnectionParamUpdateRequest(cyBle_connHandle.bdHandle,&connParam);
                #ifdef RELEASE
                CyBle_GapAuthReq(cyBle_connHandle.bdHandle,&cyBle_authInfo);
                #endif
            }
            else
            {
//                CommandMode=THROUGHT_MODE;
                printf("+CONN_EVT=%d,OK\r\n",Conn_or_Disconn_Idx);   
//                与从机连接成功后，主机发起连接间隔更新请求
                CyBle_GapcConnectionParamUpdateRequest(cyBle_connHandle.bdHandle,&connParam);
                /* Initiate an MTU exchange request CYBLE_GATT_MTU*/
                CyBle_GattcExchangeMtuReq(cyBle_connHandle,CYBLE_GATT_MTU);
            }
//            //主机发送的MTU exchange request
//            if(MTU_REQ_FLAG)
//            {
//                MTU_REQ_FLAG=FALSE;
//                /* Initiate an MTU exchange request */
//                CyBle_GattcExchangeMtuReq(cyBle_connHandle, CYBLE_GATT_MTU);
//            }
        break;
        case CYBLE_EVT_GAP_CONNECTION_UPDATE_COMPLETE: 
//            获取连接参数信息
            ConnParam=*(CYBLE_GAP_CONN_PARAM_UPDATED_IN_CONTROLLER_T *)eventParam;
//            printf("+UPDATECONN_EVT=OK\r\n");
//            if(Role==Central)
//                Timer_Enable();
        break;
//            主机接收从机的MTU交换响应
        case CYBLE_EVT_GATTC_XCHNG_MTU_RSP:            
//            negotiatedMtu=((CYBLE_GATT_XCHG_MTU_PARAM_T *)eventParam)->mtu;
//            printf("mtu is %d\r\n",negotiatedMtu);
            /* Enable notifications on the characteristic to get data from the 
             * Server.
             */
//            TroughtTest_Flag=TURE;  
//            UART_UartPutString("Calculating throughput. Please wait...\n\r");
//            Timer_Enable();
//            printf("'MTU Exchange Response' received from server device\r\n");
//            if(0x00==writeRequestData.value.val[0])
//                writeRequestData.value.val[0]=0x01;
//            writeRequestData.attrHandle=CYBLE_TROUGHPUT_SERVICE_SPEEDTEST_CHAR_HANDLE;
//            CyBle_GattcWriteCharacteristicValue(cyBle_connHandle, &writeRequestData);   
//            printf("Notify slave is %d",API_RESULT);
//            while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完成                
            break;
        /* GATT MTU exchange request - Update the negotiated MTU value */
//            做为从机时接收主机发送过来的MTU交换请求
        case CYBLE_EVT_GATTS_XCNHG_MTU_REQ:            
//            negotiatedMtu = (((CYBLE_GATT_XCHG_MTU_PARAM_T *)eventParam)->mtu < CYBLE_GATT_MTU) ?
//                            ((CYBLE_GATT_XCHG_MTU_PARAM_T *)eventParam)->mtu : CYBLE_GATT_MTU;
//            CyBle_GattsExchangeMtuRsp(cyBle_connHandle,negotiatedMtu);                            
//                            printf("mtu is %d\r\n",negotiatedMtu);
//            printf("'MTU Exchange Request' received from GATT client device\r\n");
        break;
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:   
            cyBle_discoveryModeInfo.advTo = CYBLE_FAST_ADV_TIMEOUT;
            cyBle_discoveryModeInfo.advParam->advIntvMin = CYBLE_FAST_ADV_INT_MIN;
            cyBle_discoveryModeInfo.advParam->advIntvMax = CYBLE_FAST_ADV_INT_MAX;                            
            if(Role==Peripheral)
            {
               printf("+DISCONN_EVT=0x%02X\r\n",*(uint8_t*)eventParam);
               if(!SleepMode)
               {
                   CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_SLOW);
               }
               else
               {
                   LowPower_EN=TURE;
               }
            }
            else
            {
               printf("+DISCONN_EVT=%d,0x%02X\r\n",Conn_or_Disconn_Idx,*(uint8_t*)eventParam);
               if(!SleepMode)
               {
                   CyBle_GapcStartScan(CYBLE_SCANNING_SLOW);
               }
               else
               {
                   LowPower_EN=TURE;
               }
            }
            while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据                
//            LowPower_EN=TURE;
            AUTHFLAG=FALSE;         
            CommandMode=AT_COMMAND_MODE;
            CTS_Write(CTS_OFF);
            /* RESET Uart and flush all buffers */
            UART_Stop();
            UART_SpiUartClearTxBuffer();
            UART_SpiUartClearRxBuffer();
            UART_Start();
        break;
//            从机接收主机的写请求，并且从机不需要写响应
        case CYBLE_EVT_GATTS_WRITE_CMD_REQ:
            writeParam = *(CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam;
            //自定义服务的CUSTOM Characteristic
            if(writeParam.handleValPair.attrHandle == 
               CYBLE_TROUGHPUT_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE)
            {
                if(CommandMode==THROUGHT_MODE)
                {
                    testCount+=writeParam.handleValPair.value.len;    
//                printf("0x");
//                UART_UartPutString("0x");
                    for(i=0;i<writeParam.handleValPair.value.len;i++)
                    {                    
    //                    printf("%02X",writeParam.handleValPair.value.val[i]);
                        UART_UartPutChar(writeParam.handleValPair.value.val[i]);
                    }  
                }
//                UART_UartPutString("\r\n");
//                printf("\r\n");
//                UART_SpiUartWriteTxData(testCount);
//                printf("%ld\r\n",testCount);
//                printf("+RX=<0x");
//                for(i=0;i<writeParam.handleValPair.value.len;i++)
//                {
//                    printf("%02X",writeParam.handleValPair.value.val[i]);
//                }
//                printf(">\r\n");
            } 
             //自定义服务的Client Characteristic
            if(writeParam.handleValPair.attrHandle == 
               CYBLE_TROUGHPUT_SERVICE_CUSTOM_CHARACTERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE)
            {
                charNotificationEnabled = writeParam.handleValPair.value.val[0];  
                if(charNotificationEnabled)
                {               
                    CommandMode=THROUGHT_MODE;
                    CTS_Write(CTS_ON);
                    printf("+MODE=THROUGHT_MODE\r\n");
                }
                else
                {
                    CommandMode=AT_COMMAND_MODE;
                    CTS_Write(CTS_OFF);
                    printf("+MODE=AT_COMMAND_MODE\r\n");
                }
            }   
        break;
        /* Check if the Client Characteristic Configuration Descriptor is
         * written to. Notification is enabled or disabled accordingly.
        */
//            从机接收主机的写请求，并且从机要有写响应
        case CYBLE_EVT_GATTS_WRITE_REQ:
            writeParam = *(CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam;
            //自定义服务的CUSTOM Characteristic
            if(writeParam.handleValPair.attrHandle == 
               CYBLE_TROUGHPUT_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE)
            {
                if(CommandMode==THROUGHT_MODE)
                {
                    testCount+=writeParam.handleValPair.value.len;    
//                printf("0x");
//                UART_UartPutString("0x");
                    for(i=0;i<writeParam.handleValPair.value.len;i++)
                    {                    
    //                    printf("%02X",writeParam.handleValPair.value.val[i]);
                        UART_UartPutChar(writeParam.handleValPair.value.val[i]);
                    }  
                }
//                UART_UartPutString("\r\n");
//                printf("\r\n");
//                UART_SpiUartWriteTxData(testCount);
//                printf("%ld\r\n",testCount);
//                printf("+RX=<0x");
//                for(i=0;i<writeParam.handleValPair.value.len;i++)
//                {
//                    printf("%02X",writeParam.handleValPair.value.val[i]);
//                }
//                printf(">\r\n");
            } 
             //自定义服务的Client Characteristic
            if(writeParam.handleValPair.attrHandle == 
               CYBLE_TROUGHPUT_SERVICE_CUSTOM_CHARACTERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE)
            {
                charNotificationEnabled = writeParam.handleValPair.value.val[0];  
                if(charNotificationEnabled)
                {               
                    CommandMode=THROUGHT_MODE;
                    CTS_Write(CTS_ON);
                    printf("+MODE=THROUGHT_MODE\r\n");
                }
                else
                {
                    CommandMode=AT_COMMAND_MODE;
                    CTS_Write(CTS_OFF);
                    printf("+MODE=AT_COMMAND_MODE\r\n");
                }
            }   
//            writeParam = *(CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam;
//            //自定义服务的CUSTOM Characteristic
//            if(writeParam.handleValPair.attrHandle == 
//               CYBLE_TROUGHPUT_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE)
//            {
//                testCount+=writeParam.handleValPair.value.len;
////                UART_SpiUartWriteTxData(testCount);
////                printf("%ld\r\n",testCount);
////                printf("+RX=<0x");
////                for(i=0;i<writeParam.handleValPair.value.len;i++)
////                {
////                    printf("%02X",writeParam.handleValPair.value.val[i]);
////                }
////                printf(">\r\n");
//            } 
//            //自定义服务的Client Characteristic
//            if(writeParam.handleValPair.attrHandle == 
//               CYBLE_TROUGHPUT_SERVICE_CUSTOM_CHARACTERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE)
//            {
//                charNotificationEnabled = writeParam.handleValPair.value.val[0];  
//                if(charNotificationEnabled)
//                {               
//                    CommandMode=THROUGHT_MODE;
//                    printf("+MODE=<THROUGHT_MODE>\r\n");
//                }
//                else
//                {
//                    CommandMode=AT_COMMAND_MODE;
//                    printf("+MODE=<AT_COMMAND_MODE>\r\n");
//                }
//            }   
//            //自定义服务的SPEEDTEST Characteristic
//            if(writeParam.handleValPair.attrHandle == 
//               CYBLE_TROUGHPUT_SERVICE_SPEEDTEST_CHAR_HANDLE)
//            {
//                TroughtTest_Flag = writeParam.handleValPair.value.val[0];                
//            }               
            CyBle_GattsWriteRsp(cyBle_connHandle);
        break;
//        做为主机时，接收到从机的写响应
        case CYBLE_EVT_GATTC_WRITE_RSP:
            /* Write response for the CCCD write; this means that the
             * notifications are now enabled.
             * Start timing measurements to calculate throughput.
             */
//            if(TroughtTest_Flag)//仅仅响应测试透传速率指令,其他写请求不响应
//            {                
////                TroughtTest_Flag=FALSE;
//                UART_UartPutString("Calculating throughput. Please wait...\n\r");
//                Timer_Enable();
//            }
        break;
//        case CYBLE_EVT_GATTC_READ_RSP:
//            readParam=*(CYBLE_GATTC_READ_RSP_PARAM_T*)eventParam;
//            if((readParam.value.val[0]))
//                printf("+NOTIFY=<ENABLE>\r\n");
//            else
//                printf("+NOTIFY=<DISABLE>\r\n");
//        break;
//        接收从机发送过来的透传数据            
        case CYBLE_EVT_GATTC_HANDLE_VALUE_NTF:
            if(TroughtTest_Flag==TRUE)
            {
                if(Central==Role)//仅测试透传速率时有效
                {
                    handleValueNotification = (CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *)eventParam;
                    if(handleValueNotification->handleValPair.attrHandle == 0x000E)//Notify权限的属性句柄
                    {
        //                printf("Len is %d",handleValueNotification->handleValPair.value.len);
                        totalByteCounter += handleValueNotification->handleValPair.value.len;
                    }
    //                printf("ReceviceNotification\r\n");
                }
            }
            else
            {
                if(Central==Role)//接收从机发送过来的透传数据
                {
                    handleValueNotification = (CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *)eventParam;
                    if(handleValueNotification->handleValPair.attrHandle == 0x000E)//Notify权限的属性句柄
                    {
        //                printf("Len is %d",handleValueNotification->handleValPair.value.len);
    //                    totalByteCounter += handleValueNotification->handleValPair.value.len;
    //                    printf("%ld\r\n",totalByteCounter);
//                        printf("+RX=<0x");
                          if(CommandMode==THROUGHT_MODE)
                          {
                            for(i=0;i<handleValueNotification->handleValPair.value.len;i++)
                            {
    //                            printf("%02X",handleValueNotification->handleValPair.value.val[i]);
                                UART_UartPutChar(handleValueNotification->handleValPair.value.val[i]);
                            }
                          }
//                        printf("\r\n");
                    }
                }
            }
            break;
//        case CYBLE_EVT_GATTC_READ_BY_TYPE_RSP:
//            DisAllCharResult=*((CYBLE_GATTC_READ_BY_TYPE_RSP_PARAM_T*)eventParam);
//            printf("characteristic declaration is 0x%04X\r\n",DisAllCharResult.connHandle.bdHandle);
//            printf("attrLen is %d\r\n",DisAllCharResult.attrData.attrLen);   
//            for(i=0;i<DisAllCharResult.attrData.attrLen;)
//            {
//                printf("Attribute Handle is 0x%02X%02X\r\n",DisAllCharResult.attrData.attrValue[i+1],DisAllCharResult.attrData.attrValue[i]); 
//                printf("Properties is 0x%02X\r\n",DisAllCharResult.attrData.attrValue[i+2]); 
//                printf("Pointer Handle is 0x%02X%02X\r\n",DisAllCharResult.attrData.attrValue[i+4],DisAllCharResult.attrData.attrValue[i+3]); 
//                printf("UUID is 0x%02X%02X\r\n",DisAllCharResult.attrData.attrValue[i+6],DisAllCharResult.attrData.attrValue[i+5]); 
//                i+=7;
//            }
////            printf("uuid16 is 0x%04X\r\n",DisAllCharResult.uuid.uuid16); 
////            printf("uuidFormat is 0x%04X\r\n",DisAllCharResult.uuidFormat); 
////            printf("valueHandle is 0x%04X\r\n",DisAllCharResult.valueHandle);            
//        break;
//        查找Server的所有特征值
        case CYBLE_EVT_GATTC_FIND_INFO_RSP:
            DisAllCharResult=*((CYBLE_GATTC_FIND_INFO_RSP_PARAM_T *)eventParam);
//            printf("+DISALLCHAR=<0x%02X%02X,",);
//            printf("connHandle is 0x%04X\r\n",DisAllCharResult.connHandle.bdHandle);
//            printf("uuidFormat is %d\r\n",DisAllCharResult.uuidFormat);
//            printf("byteCount is %d\r\n",DisAllCharResult.handleValueList.byteCount);
            for(i=0;i<DisAllCharResult.handleValueList.byteCount;)
            {                
                printf("+DIS_EVT=0x%02X%02X,0x%02X%02X\r\n",
                    DisAllCharResult.handleValueList.list[i+1],DisAllCharResult.handleValueList.list[i],
                    DisAllCharResult.handleValueList.list[i+3],DisAllCharResult.handleValueList.list[i+2]);
                i=i+4;
            }
        break;
        case CYBLE_EVT_GAP_AUTH_REQ:
            if(Role==Peripheral)//从机接收主机发起认证请求
            {
                CyBle_GappAuthReqReply(cyBle_connHandle.bdHandle,&cyBle_authInfo);
            }
            else
            {
//                CyBle_GappAuthReq();
            }
        break;    
        case CYBLE_EVT_GAP_PASSKEY_DISPLAY_REQUEST://显示随机产生的配对码
//            PassKey=*(uint32*)eventParam;   
            if((*(uint32 *)eventParam)<100000)//解决随机配对码最高位是0时的情况
            {
                printf("+AUTHKEY=0");
                printf("%lu\r\n",*(uint32 *)eventParam);                
            }
            else
                printf("+AUTHKEY=%lu\r\n",*(uint32 *)eventParam);
        break;
        case CYBLE_EVT_GAP_PASSKEY_ENTRY_REQUEST:
            KEYBOARD=TURE;//标记此时此设备具有输入能力
            printf("Please Input PassKey:\r\n");            
        break;
        case CYBLE_EVT_GAP_AUTH_COMPLETE:
//             printf("+AUTH=OK\r\n");
             AUTHFLAG=TURE;
        break;
        case CYBLE_EVT_GAP_AUTH_FAILED:            
            printf("AT+AUTHERR=%d\r\n",*(CYBLE_GAP_AUTH_FAILED_REASON_T*)eventParam);
            AUTHFLAG=FALSE;
        break;
        default:
        break;
    
    }
}


/******************************************************************************
* Function Name: LowPowerManagement
***************************************************************************//**
* 
*  BLE低功耗管理
* 
* \return
*  None
* 
******************************************************************************/
//uint8_t size=0,ch[10]={0},i=0;
void LowPowerManagement(void)
{
    /* Local variable to store the status of BLESS Hardware block */
		CYBLE_LP_MODE_T sleepMode;
		CYBLE_BLESS_STATE_T blessState;
	
		/* Put BLESS into Deep Sleep and check the return status */
		sleepMode = CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP);
		
		/* Disable global interrupt to prevent changes from any other interrupt ISR */
		CyGlobalIntDisable;
	
		/* Check the Status of BLESS */
		blessState = CyBle_GetBleSsState();

		if(sleepMode == CYBLE_BLESS_DEEPSLEEP)
		{
		    /* If the ECO has started or the BLESS can go to Deep Sleep, then place CPU 
			* to Deep Sleep */
			if(blessState == CYBLE_BLESS_STATE_ECO_ON || blessState == CYBLE_BLESS_STATE_DEEPSLEEP)
		    {
//                printf("Device is going into DeepSleep!\r\n");
                Status_LED_Write(LED_ON);             
                while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完才进低功耗模式                
                UART_Sleep();
		        CySysPmDeepSleep();
                UART_Wakeup();
                CyDelay(5);
                UART_SpiUartClearRxBuffer();//清除串口接收缓冲区
                UART_RX_WAKEUP_IRQ_ClearPending();
//                UART_SCB_IRQ_ClearPending();
                UART_rx_wake_ClearInterrupt();                
//                UART_RX_WAKEUP_IRQ_Disable();
//                UART_UartPutString("Device is WakeUp!\r\n");                
//                唤醒后使能串口接收中断
//                  UART_SetCustomInterruptHandler(My_ISR_UART);            
                LowPower_EN=FALSE;
                Status_LED_Write(LED_OFF);  

		 	}
		}
		else if(sleepMode == CYBLE_BLESS_SLEEP)
		{
		    if(blessState != CYBLE_BLESS_STATE_EVENT_CLOSE)
		    {
				/* If the BLESS hardware block cannot go to Deep Sleep and BLE Event has not 
				* closed yet, then place CPU to Sleep */ 
		        CySysPmSleep();  
		    }
		}
		/* Re-enable global interrupt mask after wakeup */
		CyGlobalIntEnable;           
        
}

/******************************************************************************
* Function Name: MY_ISR_USERBUTTON
***************************************************************************//**
* 
*  唤醒按键中断处理函数
* 
* \return
*  None
* 
******************************************************************************/
CY_ISR(MY_ISR_USERBUTTON)
{
//    isr_UserButton_ClearPending();
//    UserButton_ClearInterrupt();
////    清除串口接收缓冲区,防止修改了串口波特率之后,发送端未及时相应的更改,而导致串口死掉.
//    memset(RX_BUFFER,0,sizeof(RX_BUFFER));
//    Buffer_Length=0;
//    if(Peripheral==Role)
//    {
//        CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
//    }
//    StartScan=TRUE;
//    printf("test!\r\n");
}
/******************************************************************************
* Function Name: My_ISR_UART
***************************************************************************//**
* 
*  串口接收中断处理函数
* 
* \return
*  None
* 
******************************************************************************/
CY_ISR(My_ISR_UART)
{
//    uint8_t rxdata;
    if(UART_GetRxInterruptSourceMasked()&UART_INTR_RX_NOT_EMPTY)//判断接收中断的类型
    {
        rx_isover=TURE;
        //读取串口数据
//        rxdata=UART_UartGetByte();
//        if(rxdata!='\n')
//        {
//          RX_BUFFER[Buffer_Length]=rxdata;//读取串口接收缓冲区的数据
//          Buffer_Length++;
//        }
//        else
//        {          
//          RX_BUFFER[Buffer_Length]=rxdata;//读取串口接收缓冲区的数据
//          rx_isover=TURE;
//        }
    }
//    UART_SpiUartClearRxBuffer();//清除串口接收缓冲区
//    UART_RX_WAKEUP_IRQ_ClearPending();
//    UART_SCB_IRQ_ClearPending();
//    UART_rx_wake_ClearInterrupt();    
    UART_ClearRxInterruptSource(UART_INTR_RX_NOT_EMPTY);
  
}

/******************************************************************************
* Function Name: SystemInitialization
***************************************************************************//**
* 
*  BLE协议栈、外设初始化
* 
* \return
*  None
* 
******************************************************************************/
void SystemInitialization(void)
{
//    流控引脚的初始化
    CTS_SetDriveMode(CTS_DM_STRONG);//流控引脚设置为推挽输出
    CTS_Write(CTS_OFF);//流控是高电平有效
    UART_rx_wake_SetDriveMode(UART_rx_wake_DM_RES_UP);//设置RX脚为内部上拉
    UART_tx_SetDriveMode(UART_tx_DM_RES_UP);//设置TX为内部上拉
//    uint16_t counter;
    //存放Notify值的数组初始化
//    for(counter = 0; counter < 20; counter++)
//    {
//        buffer[counter] = counter;
//    }
//    初始化写请求数据
    writeRequestData.attrHandle=CYBLE_TROUGHPUT_SERVICE_CUSTOM_CHARACTERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
    writeRequestData.value=cccdNotifFlagSetStruct;
//    初始化广播间隔和广告超时时间
//    updateadvparam.advParam->advIntvMin=CYBLE_FAST_ADV_INT_MIN;
//    updateadvparam.advParam->advIntvMax=CYBLE_FAST_ADV_INT_MAX;
//    updateadvparam.advTo=CYBLE_FAST_ADV_TIMEOUT;
//    ConfigUartInit();
//    SetUartConfiguration(BAUD_9600);    
    Timer_Init();
    TimerInterrupt_StartEx(MyTimerIsr);
    UART_Start();
  //    UART_SCB_IRQ_StartEx(My_ISR_UART);   
//    isr_UserButton_StartEx(MY_ISR_USERBUTTON);
    LedInit();
    CyBle_Start(StackEventHandler);
    
}

/******************************************************************************
* Function Name: ConfigUartInit
***************************************************************************//**
* 
*  UART配置初始化
* 
* \return
*  None   
* 
******************************************************************************/
void ConfigUartInit(void)
{   
//    configUart.mode=UART_UART_MODE_STD;         /* mode: Standard */ 
//    configUart.direction=UART_UART_TX_RX;       /* direction: RX + TX */ 
//    configUart.dataBits=8;                      /* dataBits: 8 bits */
//    configUart.parity=UART_UART_PARITY_NONE;    /* parity: None */
//    configUart.stopBits=UART_UART_STOP_BITS_1;  /* stopBits: 1 bit */ 
//    configUart.oversample=16;                   /* oversample: 16u */
//    configUart.enableIrdaLowPower=0;            /* enableIrdaLowPower: disable */
//    configUart.enableMedianFilter=1;            /* enableMedianFilter: enable */
//    configUart.enableRetryNack=0;               /* enableRetryNack: disable */
//    configUart.enableInvertedRx=0;              /* enableInvertedRx: disable */
//    configUart.dropOnParityErr=0;               /* dropOnParityErr: disable */
//    configUart.dropOnFrameErr=0;                /* dropOnFrameErr: disable */
//    configUart.enableWake=0;                    /* enableWake: disable */
//    configUart.rxBufferSize=UART_RX_BUFFER;   /* rxBufferSize: software buffer 10 bytes */
//    configUart.rxBuffer=bufferRx;               /* rxBuffer: RX software buffer enable */ 
//    configUart.txBufferSize=UART_TX_BUFFER;   /* txBufferSize: software buffer 10 bytes */
//    configUart.txBuffer=bufferTx;               /* txBuffer: TX software buffer enable */
//    configUart.enableMultiproc=0;               /* enableMultiproc: disable */
//    configUart.multiprocAcceptAddr=0;           /* multiprocAcceptAddr: disable */ 
//    configUart.multiprocAddr=0;                 /* multiprocAddr: N/A for this configuration */ 
//    configUart.multiprocAddrMask=0;             /* multiprocAddrMask: N/A for this configuration */ 
//    configUart.enableInterrupt=1;               /* enableInterrupt: enable to process software buffer */ 
//    configUart.rxInterruptMask=UART_INTR_RX_NOT_EMPTY; /* rxInterruptMask: enable NOT_EMPTY for RX software buffer operations */ 
//    configUart.rxTriggerLevel=0;               /* rxTriggerLevel: N/A for this configuration */ 
//    configUart.txInterruptMask=0;              /* txInterruptMask: NOT_FULL is enabled when there is data to transmit */ 
//    configUart.txTriggerLevel=0;               /* txTriggerLevel: N/A for this configuration */ 
}
/******************************************************************************
* Function Name: SetUartConfiguration
***************************************************************************//**
* 
*  设置串口配置
* \param  DIVIDER_CLOCK:配置波特率的时钟分频，不同的频率分频系统不同。
* \return
*   CYRET_SUCCESS    //Successful
*   CYRET_BAD_PARAM  //Uknowns operation mode - no actions 
******************************************************************************/
void SetUartConfiguration(uint32_t DIVIDER_CLOCK) 
{
//    cystatus status = CYRET_SUCCESS; 
//    UART_Stop();                    /* Disable component before configuration change */ 
//    /* Change clock divider */
//    UART_CLOCK_Stop();
//    UART_CLOCK_SetFractionalDividerRegister(DIVIDER_CLOCK, 0u); 
//    UART_CLOCK_Start();
//    /* Configure to UART operation */ 
//    UART_UartInit(&configUart);          
//    UART_Start();                       /* Enable component after configuration change */     
}

/******************************************************************************
* Function Name: LedInit
***************************************************************************//**
* 
*  状态灯初始化
* 
* \return
*  None
* 
******************************************************************************/
void LedInit(void)
{
    Status_LED_SetDriveMode(Status_LED_DM_STRONG);
    Connected_LED_SetDriveMode(Connected_LED_DM_STRONG);
    Status_LED_Write(LED_OFF);
    Connected_LED_Write(LED_OFF);
}

/******************************************************************************
* Function Name: Parser_UartData
***************************************************************************//**
* 
*  解析接收到的串口数据
* \param  SerialData:指向char型的指针
* \return
*  None
* 
******************************************************************************/
void Parser_UartData(const char* SerialData)
{
    if(NULL==SerialData)
        return;
    uint8_t At_Command_Idex=0,i=0;
    uint8_t idx=0;    
    int16_t AdvMin,AdvMax;
    char8 Name[27]={0};
    char AdvData[31]={0};
    char StrAdvData[62]={0};
    char StrAdvDataLength[2]={0};
    char AdvDataLength=0;
    char AdvParamArray[2]={0};
    char AdvParam=0;
    uint8 DeviceNameLen[1]={0};
    uint32 WriteDataToFlash=0;
//    volatile uint8_t test=0;
//    double connintv=0;
//    uint16 supervisionTO=0;
    CYBLE_API_RESULT_T API_RESULT=0;
    uint8_t STAUS_T=0;
    CYBLE_GATTC_FIND_INFO_REQ_T range;    
    CYBLE_GAPP_DISC_DATA_T advDiscData;
    CYBLE_GAP_BONDED_DEV_ADDR_LIST_T bondedDevList;
//    CYBLE_CONN_HANDLE_T  connHandle; 
//     uint8 bdHandle=0;
    At_Command_Idex=Command_Identify(SerialData);
    switch(At_Command_Idex)
    {
//        测试串口是否正常----主从有效
        case _AT:
//            printf("OK\r\n");
            printf("AT+OK\r\n");
        break;
//        软复位----主从有效
        case RESET:
            if(SerialData[9]=='1')
            {
                printf("AT+OK\r\n");
                while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完成
                CySoftwareReset();
            }
            else
            {
                printf("AT+ERR=5\r\n");//表示没有该AT命令     
            }
        break;
        case REFAC:
        break;
//         当前固件版本的查询
        case VERSION:
            if(SerialData[11]=='?')
            #ifdef RELEASE
                printf("+VERSION=CY-B01_V2.0.5\r\n");
            #else
                printf("+VERSION=CY-B01_V1.1.2\r\n");
            #endif
            else
            {
                printf("AT+ERR=5\r\n");//表示没有该AT命令     
            }
        break;
//            暂时无效，固定为9600的波特率
        case BAUD:
            printf("AT+ERR=5\r\n");//表示没有该AT命令 
//            idx=SerialData[8];             
//            UartBuadRate_Handler(idx);
        break;
//            MAC地址查询----主从有效
        case LADDR:
            idx=SerialData[9]; 
            if('?'==idx)
            {
                CyBle_GetDeviceAddress(&DeviceAddr);
                printf("+LADDR=%02X%02X%02X%02X%02X%02X\r\n",                  
                    DeviceAddr.bdAddr[5],
                    DeviceAddr.bdAddr[4],
                    DeviceAddr.bdAddr[3],
                    DeviceAddr.bdAddr[2],
                    DeviceAddr.bdAddr[1],
                    DeviceAddr.bdAddr[0]                             
                  );
            }
            else
            {
                printf("AT+ERR=5\r\n");//表示没有该AT命令 
            }
        break;
//                设备名查询及修改---从机有效
        case NAME:
             idx=SerialData[8];
             if(Peripheral==Role)
             {
                switch(idx)
                {
                    case '?':
                        CyBle_GapGetLocalName(Name);
                        printf("+NAME=%s\r\n",Name);
//                        free(Name);
                    break;
                    case '<':
                        if((strchr((char*)(SerialData+9),'>')-(SerialData+9))>23)
                        {
                            printf("AT+ERR=6\r\n");//表示设置的设备名字过长
                            return;
                        }
                        if((CYBLE_STATE_CONNECTED!=CyBle_GetState()))
                        {
                            CyBle_GappStopAdvertisement();
                            memset(Name,0,sizeof(char8)*27);
                            DeviceNameLen[0]=Buffer_Length-11;
                            memcpy(Name,RX_BUFFER+9,DeviceNameLen[0]);
                            WriteUserSFlashRow(1,DeviceNameLen,1,0x00);//将设备名字长度写入SFlash中，掉电有效
                            WriteUserSFlashRow(2,Name,DeviceNameLen[0],0x00);//将设备名写入SFlash中，掉电有效
                            API_RESULT=CyBle_GapSetLocalName(Name);                            
                            if(API_RESULT==CYBLE_ERROR_OK)
                            {
                                printf("AT+OK\r\n");
//                                printf("AT+ NAME=<%s>\r\n",Name);
//                                printf("OK\r\n");
                            }
                            else
                            {
                                printf("AT+ERR=2\r\n");//表示命令设置失败
                            }
//                            free(Name);
//                            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
                        }
                        else
                        {
                                printf("AT+ERR=3\r\n");//表示当前状态不支持该命令
//                            printf("+NAME=<INVALID>\r\n"); 
                        }
                    break;
                    default:
                        printf("AT+ERR=5\r\n");//表示没有该AT命令 
                    break;
                }                   
             }
             else
             {
//                printf("+NAME=<INVALID>\r\n"); 
                printf("AT+ERR=4\r\n");//表示当前角色不支持该命令 
             }
                
        break;
        case CPIN:
        break;
//            发射功率查询及修改----从机有效
        case TXP:
            idx=SerialData[7];
            if(Role==Peripheral)
                TxPower_Handler(idx);
             else
            {
//                printf("+TXP=<INVALID>\r\n"); 
                printf("AT+ERR=4\r\n");//表示当前角色不支持该命令 
            }
        break;
//            广播数据的修改及查询----从机有效
        case ADVD:
            idx=SerialData[8];
            if(Peripheral==Role)
            {
                switch(idx)
                {
                    case '<':
                        if(CYBLE_STATE_ADVERTISING!=CyBle_GetState())
                        {
                            if((strchr((char*)(SerialData+9),'>')-(SerialData+9))>31)
                            {
                                printf("AT+ERR=9\r\n");//表示设置的广播数据长度过长
                                return;
                            }
                            memset(advDiscData.advData,0,CYBLE_GAP_MAX_ADV_DATA_LEN);
                            memcpy(StrAdvDataLength,SerialData+9,2);
                            StrToHex(&AdvDataLength,StrAdvDataLength,1);
                            advDiscData.advDataLen=AdvDataLength;
                            memcpy(StrAdvData,SerialData+12,AdvDataLength*2);
                            StrToHex(AdvData,StrAdvData,sizeof(StrAdvData)/sizeof(char)/2);
                            memcpy(advDiscData.advData,AdvData,advDiscData.advDataLen);
                            API_RESULT=CyBle_GapUpdateAdvData(&advDiscData,NULL);
                            if(CYBLE_ERROR_OK==API_RESULT)
                            {
    //                            printf("+ADVD=<SUCCESS>\r\n");
                                cyBle_discoveryModeInfo.advData->advDataLen=AdvDataLength;
                                memcpy(cyBle_discoveryModeInfo.advData->advData,AdvData,AdvDataLength);
                                printf("AT+OK\r\n");
                            }
                            else
                            {
                                printf("AT+ERR=2\r\n");//表示命令设置失败
                            }
                        }
                        else
                        {
                            printf("AT+ERR=3\r\n");//表示当前状态不支持该命令
                        }
                    break;                        
                    case '?':
                        if(CYBLE_STATE_ADVERTISING==CyBle_GetState())
                        {
                            printf("+ADVD=");
                            for(i=0;i<cyBle_discoveryModeInfo.advData->advDataLen;i++)
                            {
                                printf("%02X",cyBle_discoveryModeInfo.advData->advData[i]);
                            }
                            printf("\r\n");
                        }
                        else
                        {
                            printf("AT+ERR=3\r\n");//表示当前状态不支持该命令
                        }
                        
                    break;
                    default:
                        printf("AT+ERR=5\r\n");//表示没有该AT命令 
                    break;
                }                
            }                
            else
            {
//                printf("+ADVD=<INVALID>\r\n");
                printf("AT+ERR=4\r\n");//表示当前角色不支持该命令
            }
        break;
//            广播间隔的修改及查询----从机有效
        case ADVI:            
            if(Role==Peripheral)
            {
                idx=SerialData[8];  
                if((CYBLE_STATE_CONNECTED!=CyBle_GetState()))
                {
                    switch(idx)
                    {
                        case '?':
                        AdvMin=(cyBle_discoveryModeInfo.advParam->advIntvMin)*0.625;
                        AdvMax=(cyBle_discoveryModeInfo.advParam->advIntvMax)*0.625;                    
                        printf("+ADVI=%hdms,%hdms,%hus\r\n",AdvMin,
                        AdvMax,cyBle_discoveryModeInfo.advTo);
                        while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完成                
                        break;
                        case '<':
    //                    只有停止广播了修改才有效
                        if(CYBLE_STATE_ADVERTISING!=CyBle_GetState())
                        {
             //              获取AdvMin值
                            AdvParamArray[0]=SerialData[9];
                            AdvParamArray[1]=SerialData[10];
                            StrToHex(&AdvParam,AdvParamArray,1);
                            cyBle_discoveryModeInfo.advParam->advIntvMin=AdvParam;
        //                    获取AdvMax值
                            AdvParamArray[0]=SerialData[12];
                            AdvParamArray[1]=SerialData[13];
                            StrToHex(&AdvParam,AdvParamArray,1);
                            cyBle_discoveryModeInfo.advParam->advIntvMax=AdvParam;
        //                      获取AdvTo值
                            AdvParamArray[0]=SerialData[15];
                            AdvParamArray[1]=SerialData[16];
                            StrToHex(&AdvParam,AdvParamArray,1);
                            cyBle_discoveryModeInfo.advTo=AdvParam;
                            printf("AT+OK\r\n");
//                            printf("%s",SerialData);
//                            printf("OK\r\n");
                        }
                        else
                        {
//                             printf("+ADVI=<INVALID>\r\n");
                            printf("AT+ERR=3\r\n");//表示当前状态不支持该命令
                        }
                        break;
                        default:
                            printf("AT+ERR=5\r\n");//表示没有该AT命令 
                        break;
                    }  
                }
                else
                {
                    printf("AT+ERR=3\r\n");//表示当前状态不支持该命令
                }
                                            
            }
            else
            {
//                printf("+ADVI=<INVALID>\r\n");
                printf("AT+ERR=4\r\n");//表示当前角色不支持该命令
            }
            
        break;
//            连接间隔的修改及查询----主机有效，查询则主从有效
        case CONNI:  
            idx=SerialData[9];
            switch(idx)
            {
                case '?':
                if(CYBLE_STATE_CONNECTED==CyBle_GetState())
                {
//                    connintv=(ConnParam.connIntv)*1.25;
//                    supervisionTO=(ConnParam.supervisionTO)*10;
                    printf("+CONNI=%04X,%04X,%04X\r\n",ConnParam.connIntv,
                    ConnParam.supervisionTO,ConnParam.connLatency);
                }
                else
                {
//                    printf("+CONNI=<INVALID>\r\n");
                    printf("AT+ERR=3\r\n");//表示当前状态不支持该命令
                }
                break;
                case '<':
//                只有主机可以更改连接参数
                if(Role==Central)
                {
                    if(CYBLE_STATE_CONNECTED==CyBle_GetState())
                    {
                        //                    获取最小连接间隔
                        AdvParamArray[0]=SerialData[10];
                        AdvParamArray[1]=SerialData[11];
                        StrToHex(&AdvParam,AdvParamArray,1);
                        connParam.connIntvMin=AdvParam;
    //                    获取最大连接间隔                    
                        AdvParamArray[0]=SerialData[13];
                        AdvParamArray[1]=SerialData[14];
                        StrToHex(&AdvParam,AdvParamArray,1);
                        connParam.connIntvMax=AdvParam;
    //                    发起连接间隔更新请求
                        API_RESULT=CyBle_GapcConnectionParamUpdateRequest(cyBle_connHandle.bdHandle,&connParam);  
                        if(CYBLE_ERROR_OK==API_RESULT)
                        {
                            printf("AT+OK\r\n");
    //                        printf("%s",SerialData);
    //                        printf("OK\r\n");
                        }
                        else
                        {
    //                        printf("+CONNI=<FAIL>\r\n");
                            printf("AT+ERR=2\r\n");//表示命令设置失败
                        }
                        
                    }
                    else
                    {
                        printf("AT+ERR=3\r\n");//表示当前状态不支持该命令
                    }

                }
                else
                {
//                    printf("+CONNI=<INVALID>\r\n");
                    printf("AT+ERR=4\r\n");//表示当前角色不支持该命令
                }
                break;
                default:
                    printf("AT+ERR=5\r\n");//表示没有该AT命令 
                break;
            }
            
        break;
//              主从一体切换命令-----主从有效
        case ROLE:
            idx=SerialData[8];
            switch(idx)
            {
                case '?':
                    if(Peripheral==Role)
                    {
                        printf("+ROLE=0\r\n");
//                        printf("+ROLE=<Peripheral>\r\n");
                    }
                    else
                    {   
                        printf("+ROLE=1\r\n");
//                        printf("+ROLE=<Central>\r\n");
                    }
                break;
//                    0：从机(Slave)
                case '0':
                    if(CYBLE_STATE_CONNECTED!=CyBle_GetState())
                    {
                        WriteDataToFlash=Peripheral;//把当前的值写入Flash中,用于复位时判断是主还是从模式
                        if(USER_SFLASH_WRITE_SUCCESSFUL==WriteUserSFlashRow(0,&WriteDataToFlash,1,0x06))
                        {
                            //停止扫描,注释掉广播才正常，否则只有快广播没有慢广播
//                            CyBle_GapcStopScan();
                            //开始广播
                            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
                            Role=Peripheral;
                            printf("AT+OK\r\n");
//                            printf("+ROLE:<Peripheral>\r\n");
//                            printf("OK\r\n");
                        }  
                        
                    } 
                    else
                    {
                        printf("AT+ERR=3\r\n");//表示当前状态不支持该命令
                    }
                break;
//                     1：主机(Master)
                case '1':
                    if(CYBLE_STATE_CONNECTED!=CyBle_GetState())
                    {
                        WriteDataToFlash=Central;
                        if(USER_SFLASH_WRITE_SUCCESSFUL==WriteUserSFlashRow(0,&WriteDataToFlash,1,0x06))
                        {
//                        停止广播
//                            CyBle_GappStopAdvertisement();
//                        开始扫描
                            CyBle_GapcStartScan(CYBLE_SCANNING_FAST);
                            Role=Central;
                            printf("AT+OK\r\n");
//                            printf("+ROLE:<Central>\r\n");
//                            printf("OK\r\n");
                        }

                    }
                    else
                    {
                        printf("AT+ERR=3\r\n");//表示当前状态不支持该命令
                    }
                    
                break;
                default:
                    printf("AT+ERR=5\r\n");//表示没有该AT命令 
                break;                    
            }
        break;
////            查询所有的特征描述符----主机有效
        case DISALLCHAR:
            if(Role==Central)
            {
                range.startHandle=0x0001;
                range.endHandle=0xFFFF;       
                API_RESULT=CyBle_GattcDiscoverAllCharacteristicDescriptors(cyBle_connHandle,&range);
            }
            else
            {
                printf("AT+ERR=4\r\n");//表示当前角色不支持该命令
//                printf("+DISALLCHAR=<INVALID>\r\n");
            }
        break;
//            扫描周围的BLE设备----主机有效
        case SCAN:
            if(Role==Central)
            {
                if(SerialData[8]=='1')
                {
                    if(CYBLE_STATE_CONNECTED!=CyBle_GetState())
                    {
                        ScanDeviceInfoInit();
                        if(CYBLE_ERROR_OK==CyBle_GapcStartScan(CYBLE_SCANNING_FAST))
                        {
                            printf("AT+OK\r\n");  
                        }
                        else
                        {
                            printf("AT+ERR=2\r\n");//表示命令设置失败
                        }
                    }
                    else
                    {
                        printf("AT+ERR=3\r\n");//表示当前状态不支持该命令
                    }
                    
                }
                else if(SerialData[8]=='0')
                {
                    if(CYBLE_STATE_SCANNING==CyBle_GetState())
                    {
                        ScanDeviceInfoInit();
                        CyBle_GapcStopScan();                                   
                        STOP_SCAN_FLAG=TRUE;
                        printf("AT+OK\r\n"); 
                    }
                    else
                    {
                        printf("AT+ERR=3\r\n");//表示当前状态不支持该命令
                    }
                }
                else if(SerialData[8]=='?')
                {
                    if(0==DeviceCount)
                    {
                        printf("AT+ERR=10\r\n");//表示当前还没有扫描到周边的BLE设备
                        return;
                    }
                    for(i=0;i<DeviceCount;i++)
                    {
                        printf("+INQ_SCAN=%d,%02X%02X%02X%02X%02X%02X\r\n",
                        DeviceInfo[i].idx,
                        DeviceInfo[i].DeviceList.bdAddr[5],
                        DeviceInfo[i].DeviceList.bdAddr[4],
                        DeviceInfo[i].DeviceList.bdAddr[3],
                        DeviceInfo[i].DeviceList.bdAddr[2],
                        DeviceInfo[i].DeviceList.bdAddr[1],
                        DeviceInfo[i].DeviceList.bdAddr[0]                             
                      );
    //                  while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据                
                    }
                }
                else
                {
                    printf("AT+ERR=5\r\n");//表示没有该AT命令 
                }
            }
            else
            {
//                printf("+SCAN=<INVALID>\r\n");
                printf("AT+ERR=4\r\n");//表示当前角色不支持该命令
            }
        break;
//            连接选中的BLE设备-----主机有效
        case CONNT:
            if(Role==Central)
            {
                idx=SerialData[9];
                Connect_Device(idx);      
            }
            else
            {
//                printf("+CONNT=<INVALID>\r\n");
                printf("AT+ERR=4\r\n");//表示当前角色不支持该命令
            }
        break;
//            断开连接----主从有效
        case DISCONN:
            if(CYBLE_STATE_CONNECTED==CyBle_GetState())
            {
                idx=SerialData[11];
                if(Role==Central)
                {
                    switch(idx)
                    {
                        case '1':
                            API_RESULT=CyBle_GapDisconnect(cyBle_connHandle.bdHandle);
                            if(CYBLE_ERROR_OK!=API_RESULT)  
                            {
                                printf("AT+ERR=%d,0x%02X\r\n",idx-0x30,API_RESULT);                    
                            }
                            else
                            {
                                //断开连接后，应该清空连接列表对应的内容
                                for(i=0;i<Conn_Or_Disconn_Info.Connected_Count;i++)
                                {
                                    if(1==Conn_Or_Disconn_Info.Connect_Idx[i])
                                    {
                                        Conn_Or_Disconn_Info.Connected_Count--;
                                        memset(Conn_Or_Disconn_Info.DeviceAddr[i].bdAddr,0,6);
                                        Conn_Or_Disconn_Info.Connect_Idx[i]=0;
                                        Conn_Or_Disconn_Info.Disconn_Idx[0]=idx-0x30;
                                        Conn_or_Disconn_Idx=idx-0x30;
                                        break;
                                    }
                                }
                                printf("AT+OK\r\n");
                            }
                        break;
                        case '2':
                            API_RESULT=CyBle_GapDisconnect(cyBle_connHandle.bdHandle);
                            if(CYBLE_ERROR_OK!=API_RESULT)  
                            {
                                printf("AT+ERR=%d,0x%02X\r\n",idx-0x30,API_RESULT);                    
                            }
                            else
                            {
                                //断开连接后，应该清空连接列表对应的内容
                                for(i=0;i<Conn_Or_Disconn_Info.Connected_Count;i++)
                                {
                                    if(1==Conn_Or_Disconn_Info.Connect_Idx[i])
                                    {
                                        Conn_Or_Disconn_Info.Connected_Count--;
                                        memset(Conn_Or_Disconn_Info.DeviceAddr[i].bdAddr,0,6);
                                        Conn_Or_Disconn_Info.Connect_Idx[i]=0;
                                        Conn_Or_Disconn_Info.Disconn_Idx[0]=idx-0x30;
                                        Conn_or_Disconn_Idx=idx-0x30;
                                        break;
                                    }
                                }
                                printf("AT+OK\r\n");
                            }
                        break;
                        case '3':
                            API_RESULT=CyBle_GapDisconnect(cyBle_connHandle.bdHandle);
                            if(CYBLE_ERROR_OK!=API_RESULT)  
                            {
                                printf("AT+ERR=%d,0x%02X\r\n",idx-0x30,API_RESULT);                    
                            }
                            else
                            {
                                //断开连接后，应该清空连接列表对应的内容
                                for(i=0;i<Conn_Or_Disconn_Info.Connected_Count;i++)
                                {
                                    if(1==Conn_Or_Disconn_Info.Connect_Idx[i])
                                    {
                                        Conn_Or_Disconn_Info.Connected_Count--;
                                        memset(Conn_Or_Disconn_Info.DeviceAddr[i].bdAddr,0,6);
                                        Conn_Or_Disconn_Info.Connect_Idx[i]=0;
                                        Conn_Or_Disconn_Info.Disconn_Idx[0]=idx-0x30;
                                        Conn_or_Disconn_Idx=idx-0x30;
                                        break;
                                    }
                                }
                                printf("AT+OK\r\n");
                            }
                        break;
                        case '4':
                            API_RESULT=CyBle_GapDisconnect(cyBle_connHandle.bdHandle);
                            if(CYBLE_ERROR_OK!=API_RESULT)  
                            {
                                printf("AT+ERR=%d,0x%02X\r\n",idx-0x30,API_RESULT);                    
                            }
                            else
                            {
                                //断开连接后，应该清空连接列表对应的内容
                                for(i=0;i<Conn_Or_Disconn_Info.Connected_Count;i++)
                                {
                                    if(1==Conn_Or_Disconn_Info.Connect_Idx[i])
                                    {
                                        Conn_Or_Disconn_Info.Connected_Count--;
                                        memset(Conn_Or_Disconn_Info.DeviceAddr[i].bdAddr,0,6);
                                        Conn_Or_Disconn_Info.Connect_Idx[i]=0;
                                        Conn_Or_Disconn_Info.Disconn_Idx[0]=idx-0x30;
                                        Conn_or_Disconn_Idx=idx-0x30;
                                        break;
                                    }
                                }
                                printf("AT+OK\r\n");
                            }
                        break;  
                        default:
                            printf("AT+ERR=5\r\n");//表示没有该AT命令 
                        break;
                    }
                }
                else
                {
                    if(idx=='1')
                    {                        
                        API_RESULT=CyBle_GapDisconnect(cyBle_connHandle.bdHandle);
                        if(CYBLE_ERROR_OK!=API_RESULT)  
                        {
                            printf("AT+ERR=0x%02X\r\n",API_RESULT);                    
                        }
                    }
                    else
                    {
                        printf("AT+ERR=5\r\n");//表示没有该AT命令
                    }
                }
            }
            else
            {
                printf("AT+ERR=3\r\n");//表示当前状态不支持该命令
            }
            
        break;
        case LOGIN:
        break;
        case TX:
        break;
//            主机指令读取从机的RSSI----主从机有效
        case RSSI:
            if(CYBLE_STATE_CONNECTED==CyBle_GetState())
            {
                idx=SerialData[8];
                if(idx=='?')
                {                    
                     printf("+RSSI=%ddbm\r\n",CyBle_GetRssi());
                }                    
                else
                {
                    printf("AT+ERR=5\r\n");//表示没有该AT命令
                }
            }
            else
            {
               printf("AT+ERR=3\r\n");//表示当前状态不支持该命令 
            }
        break;     
//            透传速率测试----主有效
        case SPEED:
            idx=SerialData[9];
            if('?'==idx)
            {
                printf("%ld\r\n",testCount);
                testCount=0;
            }
            else
            {
               printf("AT+ERR=5\r\n");//表示没有该AT命令 
            }
//            if(CYBLE_STATE_CONNECTED==CyBle_GetState()&&(Central==Role))//当且仅当是连接状态且是主机状态才有效
//            {
//                idx=SerialData[9];
////                MTU_REQ_FLAG=TRUE;                
//                if('?'==idx)
//                {                           
//                    printf("Test Start\r\n");
//                    Timer_Enable();
//                    //主机发送的MTU exchange request
////                    if(MTU_REQ_FLAG^FALSE)
////                    {
////                        TroughtTest_Flag=TURE;                        
////                        MTU_REQ_FLAG=FALSE;
////                        API_RESULT=CyBle_GattcExchangeMtuReq(cyBle_connHandle, CYBLE_GATT_MTU);
//////                        printf("GattcExchangeMtuReq is %d\r\n",API_RESULT);
////                        break;
////                    }
////                    else
////                    {
//////                        TroughtTest_Flag=TURE;
//////                        charNotificationEnabled = TURE;
////                        totalByteCounter=0;
//////                        UART_UartPutString("Calculating throughput. Please wait...\n\r");
//////                        Timer_Enable();
////                        writeRequestData.value.val[0]=0x01;
////                        writeRequestData.attrHandle=CYBLE_TROUGHPUT_SERVICE_SPEEDTEST_CHAR_HANDLE;                        
////                        API_RESULT=CyBle_GattcWriteCharacteristicValue(cyBle_connHandle, &writeRequestData);
////                        if(CYBLE_ERROR_OK==API_RESULT)
////                        {
////                            CyBle_ProcessEvents();
////                        }                       
////                        printf("CyBle_GattcWriteCharacteristicDescriptors is %d\r\n",API_RESULT);
////                    }
//                }    
////            break;
//            }
//            else
//            {                
//            }
        break;
//            使能Notify属性的特征值-----主机有效，但是从机也可以有效只是不会修改NOTIFY的属性值
        case NOTIFY:
            idx=SerialData[10];
            if((CYBLE_STATE_CONNECTED==CyBle_GetState())&&(Central==Role))//主机模式下且是AT命令下才能处理NOTIFY的命令
             {                  
                switch(idx)
                {
//                    //关闭透传通道,在透传模式下不会进来这里
                    case '0':
//                        writeRequestData.value.val[0]=0x00;
//                        writeRequestData.attrHandle=CYBLE_TROUGHPUT_SERVICE_CUSTOM_CHARACTERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;                        
//                        API_RESULT=CyBle_GattcWriteCharacteristicValue(cyBle_connHandle, &writeRequestData);
//                        if(CYBLE_ERROR_OK==API_RESULT)
//                        {
                        CTS_Write(CTS_OFF);
                        CommandMode=AT_COMMAND_MODE;
                        printf("+MODE=AT_COMMAND_MODE\r\n");
//                        }
////                        printf("CyBle_GattcWriteCharacteristicValue is %d\r\n",API_RESULT);
                    break;
//                    打开透传通道
                    case '1':
                        writeRequestData.value.val[0]=0x01;
                        writeRequestData.attrHandle=CYBLE_TROUGHPUT_SERVICE_CUSTOM_CHARACTERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
                        API_RESULT=CyBle_GattcWriteWithoutResponse(cyBle_connHandle, &writeRequestData);
                        if(CYBLE_ERROR_OK==API_RESULT)
                        {
                            CommandMode=THROUGHT_MODE;
                            CTS_Write(CTS_ON);
                            printf("+MODE=THROUGHT_MODE\r\n");
                        }
//                        printf("CyBle_GattcWriteCharacteristicValue is %d\r\n",API_RESULT);
                    break;
                    case '?'://主机模式下，能接收到该命令则一定是AT命令模式
                        printf("+MODE=AT_COMMAND_MODE\r\n");                       
                    break;
                    default:
                        printf("AT+ERR=5\r\n");//表示没有该AT命令
                    break;
//                        API_RESULT=CyBle_GattcReadCharacteristicValue(cyBle_connHandle,readRequestData);
//                        printf("CyBle_GattcReadCharacteristicDescriptors is %d\r\n",API_RESULT);                        
                        
//                    break;
                }
            }
            else if((CYBLE_STATE_CONNECTED==CyBle_GetState())&&(Peripheral==Role))//从机模式下且是AT命令下才能处理NOTIFY的命令
            {
                switch(idx)
                {
                    case '0':
                        CommandMode=AT_COMMAND_MODE;
                        CTS_Write(CTS_OFF);
                        printf("+MODE=AT_COMMAND_MODE\r\n");
                    break;
                    case '1'://从机模式且AT命令模式下，打开从机给主机的透传通道
                        CommandMode=THROUGHT_MODE;
                        CTS_Write(CTS_ON);
                        printf("+MODE=THROUGHT_MODE\r\n");
                    break;
                    case '?'://从机模式下，能接收到该命令则一定是AT命令模式
                        printf("+MODE=AT_COMMAND_MODE\r\n");                       
                    break;
                    default:
                        printf("AT+ERR=5\r\n");//表示没有该AT命令
                    break;

                }
//                printf("+NOTIFY=<INVALID>\r\n");
            }
            else
            {
//                printf("+NOTIFY=<INVALID>\r\n");
                printf("AT+ERR=3\r\n");//表示当前状态不支持该命令
            }
        break;
//            当前BLE设备状态查询----主从有效
        case STAUS:
            idx=SerialData[9];            
            if(idx=='?')
            {
                STAUS_T=CyBle_GetState();
                switch(STAUS_T)
                {
                    case 0:
                            printf("+STAUS=0\r\n");
//                        printf("+STAUS=<STATE_STOPPED>\r\n");
                    break;
                    case 1:
                            printf("+STAUS=1\r\n");
//                        printf("+STAUS=<STATE_INITIALIZING>\r\n");
                    break;
                    case 2:
                            printf("+STAUS=2\r\n");
//                        printf("+STAUS=<STATE_CONNECTED>\r\n");
                    break;
                    case 3:
                            printf("+STAUS=3\r\n");
//                        printf("+STAUS=<STATE_ADVERTISING>\r\n");
                    break;
                    case 4:
                            printf("+STAUS=4\r\n");
//                        printf("+STAUS=<STATE_SCANNING>\r\n");
                    break;
                    case 5:
                            printf("+STAUS=5\r\n");
//                        printf("+STAUS=<STATE_CONNECTING>\r\n");
                    break;
                    case 6:
                            printf("+STAUS=6\r\n");
//                        printf("+STAUS=<STATE_DISCONNECTED>\r\n");
                    break;
                    default:
                    break;
                }
            }
            else
            {
                printf("AT+ERR=5\r\n");//表示没有该AT命令
            }
        break;
//            广播的打开与关闭-----从机有效
        case ADVS:            
            idx=SerialData[8];            
            if(Role==Peripheral)
            {
                if(CYBLE_STATE_CONNECTED!=CyBle_GetState())
                {
                    if(idx=='1')//开始广播
                    {
                        if(CYBLE_STATE_ADVERTISING!=CyBle_GetState())
                        {
                            API_RESULT=CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
                            if(CYBLE_ERROR_OK==API_RESULT)
                            {
    //                           printf("+ADV=<ENABLE>\r\n");
                                printf("AT+OK\r\n");
                            }
                            else
                            {
                                printf("AT+ERR=2\r\n");//表示命令设置失败
                            }
                        }
                        else
                        {
    //                        printf("+ADV=<ENABLE>\r\n");
                            printf("AT+ERR=3\r\n");//表示当前状态不支持该命令
                        }
                    }
                    else if(idx=='0')//关闭广播
                    {
                        if(CYBLE_STATE_ADVERTISING==CyBle_GetState())
                        {
                            CyBle_GappStopAdvertisement();
                            printf("AT+OK\r\n");
    //                        printf("+ADV=<DISABLE>\r\n");                        
                        }
                        else
                        {
    //                        printf("+ADV=<DISABLE>\r\n");  
                            printf("AT+ERR=3\r\n");//表示当前状态不支持该命令
                        }
                    }
                    else
                    {
                        printf("AT+ERR=5\r\n");//表示没有该AT命令
                    }
                }
                else
                {
    //                printf("+ADV=<INVALID>\r\n"); 
                    printf("AT+ERR=3\r\n");//表示当前状态不支持该命令
                }
            }
            else
            {
//                printf("+CONNT=<INVALID>\r\n");
                printf("AT+ERR=4\r\n");//表示当前角色不支持该命令
            }
        break;
//            设置设备IO的能力-----主从有效
        case IOCAP:
            idx=SerialData[9];
            if(CYBLE_STATE_CONNECTED!=CyBle_GetState())
            {                
                switch(idx)
                {
                    case '0':
                    API_RESULT=CyBle_GapSetIoCap(CYBLE_GAP_IOCAP_DISPLAY_ONLY);
                    if(API_RESULT==CYBLE_ERROR_OK)
                    {
//                        printf("+IOCAP=<DISPLAY_ONLY>\r\n");
                        printf("AT+OK\r\n");
                        iocapability=CYBLE_GAP_IOCAP_DISPLAY_ONLY;
                    }
                    else
                    {
                        printf("AT+ERR=2\r\n");//表示命令设置失败
                    }
                    break;
                    case '1':
                    API_RESULT=CyBle_GapSetIoCap(CYBLE_GAP_IOCAP_DISPLAY_YESNO);
                    if(API_RESULT==CYBLE_ERROR_OK)
                    {
//                        printf("+IOCAP=<DISPLAY_YESNO>\r\n");
                        printf("AT+OK\r\n");
                        iocapability=CYBLE_GAP_IOCAP_DISPLAY_YESNO;
                    }
                    else
                    {
                        printf("AT+ERR=2\r\n");//表示命令设置失败
                    }
                    break;
                    case '2':
                    API_RESULT=CyBle_GapSetIoCap(CYBLE_GAP_IOCAP_KEYBOARD_ONLY);
                    if(API_RESULT==CYBLE_ERROR_OK)
                    {
//                        printf("+IOCAP=<KEYBOARD_ONLY>\r\n");
                        printf("AT+OK\r\n");
                        iocapability=CYBLE_GAP_IOCAP_KEYBOARD_ONLY;
                    }
                    else
                    {
                        printf("AT+ERR=2\r\n");//表示命令设置失败
                    }
                    break;
                    case '3':
                    API_RESULT=CyBle_GapSetIoCap(CYBLE_GAP_IOCAP_NOINPUT_NOOUTPUT);
                    if(API_RESULT==CYBLE_ERROR_OK)
                    {
//                        printf("+IOCAP=<NOINPUT_NOOUTPUT>\r\n");
                        printf("AT+OK\r\n");
                        iocapability=CYBLE_GAP_IOCAP_NOINPUT_NOOUTPUT;
                    }
                    else
                    {
                        printf("AT+ERR=2\r\n");//表示命令设置失败
                    }
                    break;
                    case '4':
                    API_RESULT=CyBle_GapSetIoCap(CYBLE_GAP_IOCAP_KEYBOARD_DISPLAY);
                    if(API_RESULT==CYBLE_ERROR_OK)
                    {
//                        printf("+IOCAP=<KEYBOARD_DISPLAY>\r\n");
                        printf("AT+OK\r\n");
                        iocapability=CYBLE_GAP_IOCAP_KEYBOARD_DISPLAY;
                    }
                    else
                    {
                        printf("AT+ERR=2\r\n");//表示命令设置失败
                    }
                    break;
                    case '?':
                        switch(iocapability)
                        {
                            case 0:
                                    printf("+IOCAP=0\r\n");
//                                printf("+IOCAP=<DISPLAY_ONLY>\r\n");
                            break;
                            case 1:
                                    printf("+IOCAP=1\r\n");
//                                printf("+IOCAP=<DISPLAY_YESNO>\r\n");
                            break;
                            case 2:
                                    printf("+IOCAP=2\r\n");
//                                printf("+IOCAP=<KEYBOARD_ONLY>\r\n");
                            break;
                            case 3:
                                    printf("+IOCAP=3\r\n");
//                                printf("+IOCAP=<NOINPUT_NOOUTPUT>\r\n");
                            break;
                            case 4:
                                    printf("+IOCAP=4\r\n");
//                                printf("+IOCAP=<KEYBOARD_DISPLAY>\r\n");
                            break;
                        }
                    break;
                    default:
                        printf("AT+ERR=5\r\n");//表示没有该AT命令
                    break;
                }   
            }            
            else
            {
//                printf("+IOCAP=<INVALID>\r\n");
                printf("AT+ERR=3\r\n");//表示当前状态不支持该命令
            }
        break;
//            发起配对请求-----主从有效            
        case AUTH:
            idx=SerialData[8];
            if(idx=='?')
            {
                if(AUTHFLAG)
                        printf("+AUTH=1\r\n");
//                    printf("+AUTH=<ENABLE>\r\n");
                else
                        printf("+AUTH=0\r\n");
//                    printf("+AUTH=<DISABLE>\r\n");
            }
            else if(idx=='1')
            {
                if(!AUTHFLAG)
                {
//                  发送认证请求  
                    API_RESULT=CyBle_GapAuthReq(cyBle_connHandle.bdHandle,&cyBle_authInfo); 
                    if(API_RESULT==CYBLE_ERROR_OK)
                    {
                        printf("AT+OK\r\n");
                    }
                    else
                    {
                        printf("AT+ERR=2\r\n");//表示命令设置失败
                    }
                }                                  
                else
                {
                    printf("AT+ERR=3\r\n");//表示当前状态不支持该命令
                }
                    
//                    printf("+AUTH=<INVALID>\r\n");
            }
            else
            {
                printf("AT+ERR=5\r\n");//表示没有该AT命令
            }
        break;
//            设置低功耗能力-----主从有效            
        case SLEEP:
            idx=SerialData[9];
            switch(idx)
            {
                case '0':
//                    LowPower_EN=FALSE;
                    SleepMode=FALSE;
                    printf("AT+OK\r\n");
                break;
                case '1':
//                    LowPower_EN=TURE;
                    SleepMode=TURE;
                    printf("AT+OK\r\n");
                break;
                case '?':
                    if(SleepMode)
                            printf("+SLEEPMODE=1\r\n");
//                        printf("+SLEEPMODE=<ENABLE>\r\n");
                    else
                            printf("+SLEEPMODE=0\r\n");
//                        printf("+SLEEPMODE=<DISABLE>\r\n");
                break;
                default:
                    printf("AT+ERR=5\r\n");//表示没有该AT命令
                break;
            }
        break;
        case BOND:
            idx=SerialData[8];
            if(Role==Peripheral)
            {
                CyBle_GapGetBondedDevicesList(&bondedDevList);//防止即使掉电后，再次发起清除绑定命令仍然有效
                if(bondedDevList.count)
                    Bond=TURE;
                else
                    Bond=FALSE;
                switch(idx)
                {
                    case '0':
                        if(CYBLE_STATE_CONNECTED!=CyBle_GetState())//非连接状态时，解除绑定才有效
                        {                                                        
                            if(Bond)
                            {
                                CyBle_GapRemoveDeviceFromWhiteList(&clearAllDevices);//清除绑定先从白名单中清除再清除Flash中的信息
                                while(CYBLE_ERROR_OK != CyBle_StoreBondingData(1));
                                printf("+BOND=0\r\n");//清除绑定成功
                                Bond=FALSE;
                            }                                             
                            else
                            {
                                printf("+BOND=2\r\n");//清除绑定失败 
                            }
                        }
                        else
                        {
                            printf("AT+ERR=3\r\n");//当前的 BLE 状态下不支持该命令
                        }
                    break;
                    case '?':
                        if(Bond)
                           printf("+BOND=1\r\n");//绑定成功 
                        else
                            printf("+BOND=0\r\n");//清除绑定成功
                    break;                        
                    default:
                        printf("AT+ERR=5\r\n");//表示没有该AT命令
                    break;                        
                }                                                  
            }
            else
            {
                if(CYBLE_STATE_CONNECTED!=CyBle_GetState())
                {
                    
                }
            }
        break;
        default:
            printf("AT+ERR=5\r\n");//表示没有该AT命令
        break;
    }
        
}

/******************************************************************************
* Function Name: Command_Identify
***************************************************************************//**
* 
*  识别AT命令
* \param  SerialData:指向char型的指针
* \return 返回AT命令在AT命令列表中的索引值
*  
* 
******************************************************************************/
uint8_t Command_Identify(const char* SerialData)
{
    if(NULL==SerialData)
        return FALSE;
    uint8_t SerialArryLength=0,i=0,location;    
//    char String[20]={0};
    if(0==strcmp(SerialData,"AT\r\n"))
    {
//        printf("OK\r\n");
        return _AT;
    }
    //获取AT命令等号的位置，如“AT+RESET=1”中“=”号的位置是8.
    location=strchr((char*)SerialData,'=')-SerialData;
    if(location>11)
    {
        return 0xff;
    }
//    动态分配堆空间
    char* String=malloc(sizeof(char)*location);
    //保存AT命令等号前的数据.
    strncpy(String,SerialData,location);
    SerialArryLength=sizeof(StringArry)/sizeof(StringArry[0]);   
//    匹配AT命令列表中AT命令的位置
    for(i=0;i<SerialArryLength;i++)
    {
        if(strncmp((char*)String,StringArry[i],location)==0)
        {
            break;
        }
//        printf("%s\r\n",StringArry[i]);
//        printf("%d\r\n",strcmp((char*)String,StringArry[i]));
    }
//    释放之前分配的堆空间
    free(String);
    return i;
}

/*******************************************************************************
* Function Name: MyTimerIsr()
********************************************************************************
* Summary:
* Interrupt service routine for the timer block.
*
* Parameters:
* None
*
* Return:
* None
*
* Theory:
* The ISR is fired when the timer reaches terminal count. In the ISR, the total
* number of bytes received until now is cached locally (to avoid any potential 
* race conditions) and then the throughput is calculated from it.
* Since the ISR fires 10 seconds after the timer is started, the total number
* of bytes is divided by 10. The bytes are converted to kilobytes by dividing 
* by 1024. It is then converted to kilo bits by multiplying the result by 8.
*
* Side Effects:
* None
*
*******************************************************************************/
CY_ISR(MyTimerIsr)
{
//    uint32 cachedTotalByteCounter = totalByteCounter;
//    uint32 throughput;
    static uint32 timercount=0;
    Timer_ClearInterrupt(Timer_INTR_MASK_TC);
    _10sIsOver=TURE;
    timercount++;
    if(timercount==10001)
    {
        _10sIsOver=FALSE;
        Timer_Stop();
        timercount=0;
        printf("Send Compelete!\r\n");
        
    }
//    charNotificationEnabled=FALSE;
//    TroughtTest_Flag=FALSE;
//    TroughtTest_Flag=FALSE;
    /* Total bytes received in 10 seconds converted to Kilo bits per second */
//    printf("totalByteCounter is %ld",totalByteCounter);
//    while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完成                
//    throughput = (cachedTotalByteCounter >> 7) / 10;
//    printf("\r\n+SPEED=<%ldkbps>\r\n",throughput);      
//    CyBle_GattcWriteCharacteristicDescriptors(cyBle_connHandle, &writeRequestData);
}

/*******************************************************************************
* Function Name: SendNotification()
********************************************************************************
* Summary:
* This function creates a notification packet and sends it.
*
* Parameters:
* None
*
* Return:
* None
*
* Theory:
* The function creates a notification packet for the custom characteristic and 
* sends it over BLE. The amount of data sent for the characteristic is equal 
* to (negotiated MTU size - 3) bytes.
*
* Side Effects:
* None
*
*******************************************************************************/
void SendNotification(uint8_t*val,uint16_t len)
{
//    uint8   index;
//    uint8   uartTxData[negotiatedMtu - 3];
    CYBLE_API_RESULT_T API_RESULT=0;
//    uint16_t uartTxDataLength;
//    static uint16 uartIdleCount = UART_IDLE_TIMEOUT;
////    Uart_Char=UART_UartGetChar();
//    CYBLE_GATTS_HANDLE_VALUE_NTF_T      uartTxDataNtf;
    
    /* Create a new notification packet */
    CYBLE_GATTS_HANDLE_VALUE_NTF_T notificationPacket;
//    CYBLE_API_RESULT_T API_RESULT;
    /* Update Notification packet with the data.
     * Maximum data which can be sent is equal to (Negotiated MTU - 3) bytes.
     */
    notificationPacket.attrHandle = CYBLE_TROUGHPUT_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE;
    notificationPacket.value.val = val;   
    notificationPacket.value.len = len - 3;
//    notificationPacket.value.actualLen=0x00;
    /* Report data to BLE component */
    CyBle_GattsNotification(cyBle_connHandle, &notificationPacket);
    printf("SendNotification is %d\r\n",API_RESULT);
}
/******************************************************************************
* Function Name: SendData_toSlave
***************************************************************************//**
* 
*  Master给Slave发数据
* \param  val:指向要发送的数据的首地址
* \param  len:要发送的数据的长度
* \return none
*  
* 
******************************************************************************/
void SendData_toSlave(uint8_t*val,uint16_t len)
{
//    uint8 cccdNotifEnabledValue[] = {0x01, 0x00};
    CYBLE_API_RESULT_T API_RESULT=0;
    CYBLE_GATT_VALUE_T SendDataStruct =
    {
        val,
        len,
        0
    };
    CYBLE_GATTC_WRITE_REQ_T writeRequestData;
    writeRequestData.attrHandle=CYBLE_TROUGHPUT_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE;
    writeRequestData.value=SendDataStruct;
//    CyBle_GattcWriteCharacteristicValue(cyBle_connHandle,&writeRequestData);
    do{
        API_RESULT=CyBle_GattcWriteWithoutResponse(cyBle_connHandle,&writeRequestData);
      }
    while((CYBLE_ERROR_OK != API_RESULT)  && (CYBLE_STATE_CONNECTED == cyBle_state));
    
    
}
/******************************************************************************
* Function Name: Slave_UartHandler
***************************************************************************//**
* 
*  在Master_Slave模式下处理串口指令
* \param  none
* \return none
*  
* 
******************************************************************************/
void Master_Slave_UartHandler(uint8_t Role)
{
    uint8_t Uart_Char=0;
    uint32 passkey=0;
    uint32 pow;
    uint8   index;
    uint8   uartTxData[negotiatedMtu - 3];
    CYBLE_API_RESULT_T API_RESULT=0;
    uint16_t uartTxDataLength;
    static uint16 uartIdleCount = UART_IDLE_TIMEOUT;
//    Uart_Char=UART_UartGetChar();
    CYBLE_GATTS_HANDLE_VALUE_NTF_T      uartTxDataNtf;
    uartTxDataLength = UART_SpiUartGetRxBufferSize();//只有读取完了RX_Buffer才清0
//    UART_UartPutChar(uartTxDataLength);   
//        处理串口信息,不采用中断的方式,采取中断会影响透传速率
    if((0!=uartTxDataLength)&&((CommandMode==THROUGHT_MODE)))
    {   
         #ifdef  FLOW_CONTROL//此处最好置一GPIO口为低电平告诉发送方停止发送数据
            if(uartTxDataLength>=(UART_RX_BUFFER/4))
            {
                UART_RX_INT_DISABLE();
                CTS_Write(CTS_OFF);
            }
            else
            {
                UART_RX_INT_ENABLE();
                CTS_Write(CTS_ON);
            }
        #endif
        if(uartTxDataLength>=negotiatedMtu-3)
        {                 
            uartIdleCount=UART_IDLE_TIMEOUT;
            uartTxDataLength=negotiatedMtu-3;
        }
        else//如果小于20字节则等个100ms,如果100ms后还是少于20字节则透传出去
        {
            if(--uartIdleCount == 0)
            {
                /*uartTxDataLength remains unchanged */;
            }
            else
            {
                uartTxDataLength = 0;
            }
        }
        if(0!=uartTxDataLength)
        {
            uartIdleCount=UART_IDLE_TIMEOUT;
//            UART_UartPutChar(UART_SpiUartGetRxBufferSize());
            for(index=0;index<uartTxDataLength;index++)//不管是主机还是从机，在透传模式下先收串口接收到的数据，然后才判断是主机和从机再进行相对应的处理
            {
                uartTxData[index] = (uint8) UART_UartGetByte();
            }            
            if(0==memcmp("AT+NOTIFY=0",uartTxData,11))//在透传模式下，判断是不是要退出透传模式，因为透传模式下会认为串口接收的数据都是透传数据       
            {                
                if(Role==Central) //主机模式下退出透传模式
                {
                    writeRequestData.value.val[0]=0x00;
                    writeRequestData.attrHandle=CYBLE_TROUGHPUT_SERVICE_CUSTOM_CHARACTERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;                        
                    API_RESULT=CyBle_GattcWriteCharacteristicValue(cyBle_connHandle, &writeRequestData);
                    if(CYBLE_ERROR_OK==API_RESULT)
                    {
                        CommandMode=AT_COMMAND_MODE;
                        CTS_Write(CTS_OFF);
                        Buffer_Length=0;
                        memset(RX_BUFFER,0,sizeof(RX_BUFFER));
                        printf("+MODE=AT_COMMAND_MODE\r\n");
                    }
                }
                else //从机模式下退出透传模式
                {
                    CommandMode=AT_COMMAND_MODE;
                    CTS_Write(CTS_OFF);
                    Buffer_Length=0;
                    memset(RX_BUFFER,0,sizeof(RX_BUFFER));
                    printf("+MODE=AT_COMMAND_MODE\r\n");
                }
                return;
            }  
            if(0==memcmp("AT+NOTIFY=?",uartTxData,11))//不管主机还是从机，在透传模式下均响应该命令
            {
                Buffer_Length=0;
                memset(RX_BUFFER,0,sizeof(RX_BUFFER));
                printf("+MODE=THROUGHT_MODE\r\n");
                return;
            }
            else if(Peripheral==Role)//从机给主机发送透传数据
            {
               while(CyBle_GattGetBusStatus()!= CYBLE_STACK_STATE_FREE);//只有当蓝牙协议栈空闲时才发送数据
                uartTxDataNtf.value.val  = uartTxData;
                uartTxDataNtf.value.len  = uartTxDataLength;
                uartTxDataNtf.attrHandle = CYBLE_TROUGHPUT_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE;
//                #ifdef FLOW_CONTROL
//                    UART_RX_INT_DISABLE();
//                #endif
                do
                {
                    API_RESULT = CyBle_GattsNotification(cyBle_connHandle, &uartTxDataNtf);
                    CyBle_ProcessEvents();
                }
                while((CYBLE_ERROR_OK != API_RESULT)  && (CYBLE_STATE_CONNECTED == cyBle_state));
//                testCount+=uartTxDataLength;
            }
            else////主机给从机发送透传数据
            {
                #ifdef FLOW_CONTROL
                    UART_RX_INT_DISABLE();
                #endif
                while(CyBle_GattGetBusStatus()!= CYBLE_STACK_STATE_FREE);//只有当蓝牙协议栈空闲时才发送数据
                SendData_toSlave(uartTxData,uartTxDataLength);
            }
        }
        return ;
    }
    else if(0!=uartTxDataLength)
    {   
        RxFlag=START;
        if(KEYBOARD)
        {
           KEYBOARD=FALSE;
        /*Set the passkey to zero before receiving New passkey */
            passkey = 0u;
            pow = 100000ul;
            
            /*Get 6 digit number from UART terminal*/
            for(index = 0u; index < CYBLE_GAP_USER_PASSKEY_SIZE; index++)
            {
                while((Uart_Char = UART_UartGetChar()) == 0);
                
                /* accept the digits that are in between the range '0' and '9'  */
                if((Uart_Char >= '0') && (Uart_Char <= '9'))  
                {
                    passkey += (uint32)(Uart_Char - '0') * pow;
                    pow /= 10u;
//                    UART_UartPutChar(Uart_Char); 
                }
                else  /* If entered digit is not in between the rnage '0' and '9'*/
                {
                    printf("+AUTHKEY=INVALID\r\n");
                    break;
                }
            }            
            /*Send Pass key Response to to create an Authenticated Link*/
            API_RESULT = CyBle_GapAuthPassKeyReply(cyBle_connHandle.bdHandle,passkey,1);         
            /*Check the result of CyBle_GapAuthPassKeyReply*/
            if(API_RESULT== CYBLE_ERROR_INVALID_PARAMETER)
            {
                printf("CYBLE_ERROR_INVALID_PARAMETER \r\n");               
            }
            else if(API_RESULT==CYBLE_ERROR_MEMORY_ALLOCATION_FAILED)
            {
                printf("CYBLE_ERROR_MEMORY_ALLOCATION_FAILED\r\n");
            }
            else if(API_RESULT==CYBLE_ERROR_NO_DEVICE_ENTITY)
            {
                printf("CYBLE_ERROR_NO_DEVICE_ENTITY\r\n");
            }
            else if(API_RESULT==CYBLE_ERROR_OK)
            {
                printf("+AUTHKEY=SUCCESFULLY\r\n");
            }
        }
        else
        {
            Uart_Char=(uint8) UART_UartGetByte();
            if('\n'!=Uart_Char)
            {
    //            if(CommandMode==THROUGHT_MODE)
    //            {
    //                RX_BUFFER[Buffer_Length]=Uart_Char-0x30;//读取串口接收缓冲区的数据
    //                Buffer_Length++;
    //            }
    //            else
    //            {
                    RX_BUFFER[Buffer_Length]=Uart_Char;//读取串口接收缓冲区的数据
                    Buffer_Length++;
    //            }                      
            }
            else
            {
                uartIdleCount=CRLF_TIMEOUT;
                RxFlag=OVER;
            //串口接收的数据长度清零,以便下一条命令过来时还是存放在数组首地址
                RX_BUFFER[Buffer_Length]=Uart_Char;//读取串口接收缓冲区的数据                
                Parser_UartData((char*)RX_BUFFER);
                Buffer_Length=0;
                memset(RX_BUFFER,0,sizeof(RX_BUFFER));
    //            if(CommandMode==THROUGHT_MODE)
    //            {
    //                
    //                if(Role==Peripheral)
    //                    SendNotification(RX_BUFFER,Buffer_Length+3);
    //                else
    //                    SendData_toSlave(RX_BUFFER,Buffer_Length);
    //                //串口接收的数据长度清零,以便下一条命令过来时还是存放在数组首地址
    //                Buffer_Length=0;
    //                memset(RX_BUFFER,0,sizeof(RX_BUFFER));
    //            }
    //            else
    //            {
    //                
    //            }
            }
        }    
    }
    else
    {
        if(RxFlag)//如果串口100ms内没有接收到回车换行符时，则认为该AT命令无效
        {
            if(--uartIdleCount == 0)
            {
                RxFlag=OVER;
                uartIdleCount=CRLF_TIMEOUT;
                Buffer_Length=0;
                memset(RX_BUFFER,0,sizeof(RX_BUFFER));
                printf("AT+ERR=7\r\n");//AT命令没有回车换行
                /*uartTxDataLength remains unchanged */;
            }
        }
    }
}

/******************************************************************************
* Function Name: Connect_Device
***************************************************************************//**
* 
*  连接扫描列表中的Slave设备
* \param  idx:扫描列表中的索引号
* \return none
*  
* 
******************************************************************************/
void Connect_Device(uint8_t idx)
{
    CYBLE_API_RESULT_T API_RESULT=0;
    uint8_t i=0;
    if(Role==Central)
    {
        switch(idx)
        {
            case '1':
                API_RESULT=CyBle_GapcConnectDevice(&DeviceInfo[idx-1-0x30].DeviceList);
                if(CYBLE_ERROR_OK!=API_RESULT)  
                {
                    printf("AT+ERR=%d,0x%02X\r\n",idx-0x30,API_RESULT);                    
                }
                else
                {
                    Conn_Or_Disconn_Info.Connected_Count++;
                    Conn_Or_Disconn_Info.Connect_Idx[Conn_Or_Disconn_Info.Connected_Count-1]=idx-0x30;
                    Conn_or_Disconn_Idx=idx-0x30;
                    memcpy(Conn_Or_Disconn_Info.DeviceAddr[Conn_Or_Disconn_Info.Connected_Count-1].bdAddr,
                           DeviceInfo[idx-1-0x30].DeviceList.bdAddr,6);  
                    printf("AT+OK\r\n");
//                        CyBle_GapGetPeerBdHandle(bdHandle,&DeviceInfo[idx-1-0x30].DeviceList);
//                        printf("0x%02X\r\n",*bdHandle);  
                }
            break;
            case '2':
                API_RESULT=CyBle_GapcConnectDevice(&DeviceInfo[idx-1-0x30].DeviceList);
                if(CYBLE_ERROR_OK!=API_RESULT)  
                {
                    printf("AT+ERR=%d,0x%02X\r\n",idx-0x30,API_RESULT);                    
                }
                else
                {
                    Conn_Or_Disconn_Info.Connected_Count++;
                    Conn_Or_Disconn_Info.Connect_Idx[Conn_Or_Disconn_Info.Connected_Count-1]=idx-0x30;
                    Conn_or_Disconn_Idx=idx-0x30;
                    memcpy(Conn_Or_Disconn_Info.DeviceAddr[Conn_Or_Disconn_Info.Connected_Count-1].bdAddr,
                           DeviceInfo[idx-1-0x30].DeviceList.bdAddr,6);
                    printf("AT+OK\r\n");
                }
            break;
            case '3':
                API_RESULT=CyBle_GapcConnectDevice(&DeviceInfo[idx-1-0x30].DeviceList);
                if(CYBLE_ERROR_OK!=API_RESULT)  
                {
                    printf("AT+ERR=%d,0x%02X\r\n",idx-0x30,API_RESULT);                    
                }
                else
                {
                    Conn_Or_Disconn_Info.Connected_Count++;
                    Conn_Or_Disconn_Info.Connect_Idx[Conn_Or_Disconn_Info.Connected_Count-1]=idx-0x30;
                    Conn_or_Disconn_Idx=idx-0x30;
                    memcpy(Conn_Or_Disconn_Info.DeviceAddr[Conn_Or_Disconn_Info.Connected_Count-1].bdAddr,
                           DeviceInfo[idx-1-0x30].DeviceList.bdAddr,6);
                    printf("AT+OK\r\n");
                }
            break;
            case '4':
                API_RESULT=CyBle_GapcConnectDevice(&DeviceInfo[idx-1-0x30].DeviceList);
                if(CYBLE_ERROR_OK!=API_RESULT)  
                {
                    printf("AT+ERR=%d,0x%02X\r\n",idx-0x30,API_RESULT);                    
                }
                else
                {
                    Conn_Or_Disconn_Info.Connected_Count++;
                    Conn_Or_Disconn_Info.Connect_Idx[Conn_Or_Disconn_Info.Connected_Count-1]=idx-0x30;
                    Conn_or_Disconn_Idx=idx-0x30;
                    memcpy(Conn_Or_Disconn_Info.DeviceAddr[Conn_Or_Disconn_Info.Connected_Count-1].bdAddr,
                           DeviceInfo[idx-1-0x30].DeviceList.bdAddr,6);
                    printf("AT+OK\r\n");
                }
            break;
            case '?':
                if(CYBLE_STATE_CONNECTED==CyBle_GetState())
                {
                    for(i=0;i<Conn_Or_Disconn_Info.Connected_Count;i++)
                    {
                        printf("+INQ_CONN=%d,%02X%02X%02X%02X%02X%02X\r\n",
                        Conn_Or_Disconn_Info.Connect_Idx[i],
                        Conn_Or_Disconn_Info.DeviceAddr[i].bdAddr[5],
                        Conn_Or_Disconn_Info.DeviceAddr[i].bdAddr[4],
                        Conn_Or_Disconn_Info.DeviceAddr[i].bdAddr[3],
                        Conn_Or_Disconn_Info.DeviceAddr[i].bdAddr[2],
                        Conn_Or_Disconn_Info.DeviceAddr[i].bdAddr[1],
                        Conn_Or_Disconn_Info.DeviceAddr[i].bdAddr[0]
                      );
                    }
                }
                else
                {
                    printf("AT+ERR=3\r\n");//表示当前状态不支持该命令
                }
                
            break;
        }
    }
    else
    {
        printf("AT+ERR=4\r\n");//表示当前角色不支持该命令 
    }
}
/******************************************************************************
* Function Name: UartBuadRate_Handler
***************************************************************************//**
* 
*  设备串口波特率或者查询当前的串口波特率
* \param  idx:设置的波特率值或查询波特率的状态
* \return none
*  
* 
******************************************************************************/
void UartBuadRate_Handler(uint8_t idx)
{
    switch(idx)
    {
//                1200
        case '0':                     
             printf("AT+BAUD=<1200>\r\n");
             printf("OK\r\n");
             while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完成                
             SetUartConfiguration(BAUD_1200);
             Baud_rate_idx=idx;
        break;
//                2400
        case '1':
             printf("AT+BAUD=<2400>\r\n");
             printf("OK\r\n");
             while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完成                
             SetUartConfiguration(BAUD_2400);
             Baud_rate_idx=idx;
        break;
//                4800
        case '2':
             printf("AT+BAUD=<4800>\r\n");
             printf("OK\r\n");
             while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完成                
             SetUartConfiguration(BAUD_4800);
             Baud_rate_idx=idx;
        break;
//                9600
        case '3':
             printf("AT+BAUD=<9600>\r\n");
             printf("OK\r\n");
             while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完成                
             SetUartConfiguration(BAUD_9600);
             Baud_rate_idx=idx;
        break;
        case '4':
             printf("AT+BAUD=<19200>\r\n");
             printf("OK\r\n");
             while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完成                
             SetUartConfiguration(BAUD_19200);
             Baud_rate_idx=idx;
        break;
        case '5':
             printf("AT+BAUD=<38400>\r\n");
             printf("OK\r\n");
             while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完成                
             SetUartConfiguration(BAUD_38400);
             Baud_rate_idx=idx;
        break;
//                57600
        case '6':
             printf("AT+BAUD=<57600>\r\n");
             printf("OK\r\n");
             while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完成                
             SetUartConfiguration(BAUD_57600);
             Baud_rate_idx=idx;
        break;
        case '7':
             printf("AT+BAUD=<115200>\r\n");
             printf("OK\r\n");
             while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完成                
             SetUartConfiguration(BAUD_115200);
             Baud_rate_idx=idx;
        break;
        case '8':
             printf("AT+BAUD=<230400>\r\n");
             printf("OK\r\n");
             while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完成                
             SetUartConfiguration(BAUD_230400);
             Baud_rate_idx=idx;
        break;
        case '9':
             printf("AT+BAUD=<460800>\r\n");
             printf("OK\r\n");
             while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完成                
             SetUartConfiguration(BAUD_460800);
             Baud_rate_idx=idx;
        break;
        case 'a':
        case 'A':
             printf("AT+BAUD=<921600>\r\n");
             printf("OK\r\n");
             while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完成                
             SetUartConfiguration(BAUD_921600);
             Baud_rate_idx=idx;
        break;
        case '?':
            switch(Baud_rate_idx)
            {
//                1200
                case '0':                     
                     printf("+BAUD:<1200>\r\n");
//                             printf("OK\r\n");
                     while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完成                
//                             SetUartConfiguration(BAUD_1200);
                break;
//                2400
                case '1':
                     printf("+BAUD:<2400>\r\n");
//                             printf("OK\r\n");
                     while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完成                
//                             SetUartConfiguration(BAUD_2400);
                break;
//                4800
                case '2':
                     printf("+BAUD:<4800>\r\n");
//                             printf("OK\r\n");
                     while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完成                
//                             SetUartConfiguration(BAUD_4800);
                break;
//                9600
                case '3':
                     printf("+BAUD:<9600>\r\n");
//                             printf("OK\r\n");
                     while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完成                
//                             SetUartConfiguration(BAUD_9600);
                break;
                case '4':
                     printf("+BAUD:<19200>\r\n");
//                             printf("OK\r\n");
                     while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完成                
//                             SetUartConfiguration(BAUD_19200);
                break;
                case '5':
                     printf("+BAUD:<38400>\r\n");
//                             printf("OK\r\n");
                     while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完成                
//                             SetUartConfiguration(BAUD_38400);
                break;
//                57600
                case '6':
                     printf("+BAUD:<57600>\r\n");
//                             printf("OK\r\n");
                     while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完成                
//                             SetUartConfiguration(BAUD_57600);
                break;
                case '7':
                     printf("+BAUD:<115200>\r\n");
//                             printf("OK\r\n");
                     while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完成                
//                             SetUartConfiguration(BAUD_115200);
                break;
                case '8':
                     printf("+BAUD:<230400>\r\n");
//                             printf("OK\r\n");
                     while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完成                
//                             SetUartConfiguration(BAUD_230400);
                break;
                case '9':
                     printf("+BAUD:<460800>\r\n");
//                             printf("OK\r\n");
                     while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完成                
//                             SetUartConfiguration(BAUD_460800);
                break;
                case 'a':
                case 'A':
                     printf("+BAUD:<921600>\r\n");
//                             printf("OK\r\n");
                     while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完成                
//                             SetUartConfiguration(BAUD_921600);
                break;
                default:
                break;
             }
        break;
    }  
}
/******************************************************************************
* Function Name: TxPower_Handler
***************************************************************************//**
* 
*  设置发射功率或者查询当前的发射功率
* \param  idx:设置的发射功率值或查询发射功率的状态
* \return none
*  
* 
******************************************************************************/
void TxPower_Handler(uint8_t idx)
{
    CYBLE_BLESS_PWR_IN_DB_T bleSsPwrLvl;
    CYBLE_API_RESULT_T API_RESULT=0;
    if(CYBLE_STATE_ADVERTISING==CyBle_GetState())
    {
        bleSsPwrLvl.bleSsChId=CYBLE_LL_ADV_CH_TYPE;
    }
    else if(CYBLE_STATE_CONNECTED==CyBle_GetState())
    {
        bleSsPwrLvl.bleSsChId=CYBLE_LL_CONN_CH_TYPE;
    }
    else//如何不是广播或者连接状态则无效
    {
        printf("AT+ERR=8\r\n");//表示当前状态不是连接状态也不是广播状态，无法查询或修改发射功率 
        return;
    }
    switch(idx)
    {
        case '?':                        
            CyBle_GetTxPowerLevel(&bleSsPwrLvl);
            idx=bleSsPwrLvl.blePwrLevelInDbm;
            switch(idx)
            {
                case 1:
                        printf("+TXP=1\r\n");
//                    printf("+TXP=<-18dBm>\r\n");
                break;
                case 2:
                        printf("+TXP=2\r\n");
//                    printf("+TXP=<-12dBm>\r\n");
                break;
                case 3:
                        printf("+TXP=3\r\n");
//                    printf("+TXP=<-6dBm>\r\n");
                break;
                case 4:
                        printf("+TXP=4\r\n");
//                    printf("+TXP=<-3dBm>\r\n");
                break;
                case 5:
                        printf("+TXP=5\r\n");
//                    printf("+TXP=<-2dBm>\r\n");
                break;
                case 6:
                        printf("+TXP=6\r\n");
//                    printf("+TXP=<-1dBm>\r\n");
                break;
                case 7:
                        printf("+TXP=7\r\n");
//                    printf("+TXP=<0dBm>\r\n");
                break;
                case 8:
                        printf("+TXP=8\r\n");
//                    printf("+TXP=<3dBm>\r\n");
                break;
            }
            
        break;
        case '1':
            bleSsPwrLvl.blePwrLevelInDbm=CYBLE_LL_PWR_LVL_NEG_18_DBM;
            API_RESULT=CyBle_SetTxPowerLevel(&bleSsPwrLvl);
            if(CYBLE_ERROR_OK==API_RESULT)
            {     
                printf("AT+OK\r\n");
//                printf("AT+TXP=<-18dBm>\r\n");
//                printf("OK\r\n");
            }            
        break;
        case '2':
            bleSsPwrLvl.blePwrLevelInDbm=CYBLE_LL_PWR_LVL_NEG_12_DBM;
            API_RESULT=CyBle_SetTxPowerLevel(&bleSsPwrLvl);
            if(CYBLE_ERROR_OK==API_RESULT)
            {     
                printf("AT+OK\r\n");
//                printf("AT+TXP=<-12dBm>\r\n");
//                printf("OK\r\n");
            }            
        break;
        case '3':
            bleSsPwrLvl.blePwrLevelInDbm=CYBLE_LL_PWR_LVL_NEG_6_DBM;
            API_RESULT=CyBle_SetTxPowerLevel(&bleSsPwrLvl);
            if(CYBLE_ERROR_OK==API_RESULT)
            {     
                printf("AT+OK\r\n");
//                printf("AT+TXP=<-6dBm>\r\n");
//                printf("OK\r\n");
            }            
        break;
        case '4':
            bleSsPwrLvl.blePwrLevelInDbm=CYBLE_LL_PWR_LVL_NEG_3_DBM;
            API_RESULT=CyBle_SetTxPowerLevel(&bleSsPwrLvl);
            if(CYBLE_ERROR_OK==API_RESULT)
            {    
                printf("AT+OK\r\n");
//                printf("AT+TXP=<-3dBm>\r\n");
//                printf("OK\r\n");
            }            
        break;
        case '5':
            bleSsPwrLvl.blePwrLevelInDbm=CYBLE_LL_PWR_LVL_NEG_2_DBM;
            API_RESULT=CyBle_SetTxPowerLevel(&bleSsPwrLvl);
            if(CYBLE_ERROR_OK==API_RESULT)
            {     
                printf("AT+OK\r\n");
//                printf("AT+TXP=<-2dBm>\r\n");
//                printf("OK\r\n");
            }           
        break;
        case '6':
            bleSsPwrLvl.blePwrLevelInDbm=CYBLE_LL_PWR_LVL_NEG_1_DBM;
            API_RESULT=CyBle_SetTxPowerLevel(&bleSsPwrLvl);
            if(CYBLE_ERROR_OK==API_RESULT)
            {    
                printf("AT+OK\r\n");
//                printf("AT+TXP=<-1dBm>\r\n");
//                printf("OK\r\n");
            }            
        break;
        case '7':
            bleSsPwrLvl.blePwrLevelInDbm=CYBLE_LL_PWR_LVL_0_DBM;
            API_RESULT=CyBle_SetTxPowerLevel(&bleSsPwrLvl);
            if(CYBLE_ERROR_OK==API_RESULT)
            {   
                printf("AT+OK\r\n");
//                printf("AT+TXP=<0dBm>\r\n");
//                printf("OK\r\n");
            }      
        break;
        case '8':
            bleSsPwrLvl.blePwrLevelInDbm=CYBLE_LL_PWR_LVL_3_DBM;
            API_RESULT=CyBle_SetTxPowerLevel(&bleSsPwrLvl);
            if(CYBLE_ERROR_OK==API_RESULT)
            {     
                printf("AT+OK\r\n");
//                printf("AT+TXP=<3dBm>\r\n");
//                printf("OK\r\n");
            }           
        break;
        default:
            printf("AT+ERR=5\r\n");//表示没有该AT命令 
        break;
    }    
}
/******************************************************************************
* Function Name: StrToHex
***************************************************************************//**
* 
*  字符串转十六进制
* \param  pbDest:输出缓冲区
* \param  pbSrc :输入的字符串
* \param  nLen :输入的字符串16进制数的字节数(字符串的长度/2)
* \return none
*  
* 
******************************************************************************/
void StrToHex(char *pbDest, char *pbSrc, int nLen)
{
    unsigned char h1,h2;
    unsigned char s1,s2;
    int i;

    for (i=0; i<nLen; i++)
    {
    h1 = pbSrc[2*i];
    h2 = pbSrc[2*i+1];

    s1 = toupper(h1) - 0x30;
    if (s1 > 9) 
    s1 -= 7;

    s2 = toupper(h2) - 0x30;
    if (s2 > 9) 
    s2 -= 7;

    pbDest[i] = s1*16 + s2;
    }
}
/******************************************************************************
* Function Name: Printf函数重映射
***************************************************************************//**
* 
*  重写_write()函数，为了使能Printf函数打印串口信息
* 
* \return
*  None
* 
******************************************************************************/
#if defined(__ARMCC_VERSION)
/* For MDK/RVDS compiler revise fputc function for printf functionality */
struct __FILE
{
    int handle;
};
enum
{
    STDIN_HANDLE,
    STDOUT_HANDLE,
    STDERR_HANDLE
};
FILE __stdin = {STDIN_HANDLE};
FILE __stdout = {STDOUT_HANDLE};
FILE __stderr = {STDERR_HANDLE};
int fputc(int ch, FILE *file)
{
    int ret = EOF;
    switch( file->handle )
    {
        case STDOUT_HANDLE:
            UART_DEB_UartPutChar(ch);
            ret = ch ;
            break ;
        case STDERR_HANDLE:
            ret = ch ;
            break ;
        default:
            file = file;
            break ;
    }
    return ret ;
}
#elif defined (__ICCARM__)      /* IAR */
/* For IAR compiler revise __write() function for printf functionality */
size_t __write(int handle, const unsigned char * buffer, size_t size)
{
    size_t nChars = 0;
    if (buffer == 0)
    {
        /*
         * This means that we should flush internal buffers.  Since we
         * don't we just return.  (Remember, "handle" == -1 means that all
         * handles should be flushed.)
         */
        return (0);
    }
    for (/* Empty */; size != 0; --size)
    {
        UART_DEB_UartPutChar(*buffer++);
        ++nChars;
    }
    return (nChars);
}
#else  /* (__GNUC__)  GCC */
/* For GCC compiler revise _write() function for printf functionality */
int _write(int file, char *ptr, int len)
{
    int i;
    file = file;
    for (i = 0; i < len; i++)
    {
        UART_UartPutChar(*ptr++);//这里替代成UART的发送单字节的函数即可
    }
    return len;
}

#endif  /* (__ARMCC_VERSION) */ 


/* [] END OF FILE */
