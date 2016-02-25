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
#include "BLEClient.h"
#include <stdio.h>
/***************************************
* 全局变量
***************************************/
//uint8_t StartScan=FALSE;
//uint8_t DeviceList[DeviceListCount]={0};
CYBLE_DEVICE_INFO DeviceInfo[DeviceListCount];
//  计数扫描到的BLE设备
uint8_t DeviceCount=0;
uint8_t flag=0;
/******************************************************************************
* Function Name: HandleScanDevices
***************************************************************************//**
* 
*  处理扫描到的周边BLE设备
* 
* \return
*  None
* 
******************************************************************************/
void HandleScanDevices(CYBLE_GAPC_ADV_REPORT_T* ScanReport)
{
//    printf("Advertising or Scan Response Data is 0x%02X%02X%02X%02X%02X%02X\r\n",
//            );
    uint8_t i=0;
//    第一次扫描时,保存第一次扫描到的设备地址
    if(flag^1)
    {           
        flag=1;   
        if(CYBLE_GAPC_CONN_UNDIRECTED_ADV==ScanReport->eventType)   
        {
            DeviceCount++;
            memcpy(DeviceInfo[DeviceCount-1].DeviceList.bdAddr,ScanReport->peerBdAddr,6);
            if(CYBLE_GAP_ADDR_TYPE_PUBLIC==ScanReport->peerAddrType)
            {
                DeviceInfo[DeviceCount-1].DeviceList.type=CYBLE_GAP_ADDR_TYPE_PUBLIC;
//                DeviceInfo[DeviceCount-1].DeviceList.type='P';
            }
            else
            {
                DeviceInfo[DeviceCount-1].DeviceList.type=CYBLE_GAP_ADDR_TYPE_RANDOM;
//                DeviceInfo[DeviceCount-1].DeviceList.type='R';
            }
            DeviceInfo[DeviceCount-1].idx=DeviceCount;
            DeviceInfo[DeviceCount-1].rssi=ScanReport->rssi;
            memcpy(DeviceInfo[DeviceCount-1].DeviceList.bdAddr,ScanReport->peerBdAddr,6);
            Parser_adv_data_scan_data(ScanReport->data,ScanReport->dataLen);
            if(DeviceInfo[DeviceCount-1].ScanName)
            {
                DeviceInfo[DeviceCount-1].ScanName=FALSE;
            }
            else
            {
                memset(DeviceInfo[DeviceCount-1].DeviceName,0,sizeof(DeviceInfo[DeviceCount-1].DeviceName));
                memcpy(DeviceInfo[DeviceCount-1].DeviceName,"NULL",sizeof("NULL"));
            }
            printf("+SCAN=<%d,%d,%c,0x%02X%02X%02X%02X%02X%02X,%s>\r\n",
                    DeviceInfo[DeviceCount-1].idx,
                    DeviceInfo[DeviceCount-1].rssi,
                    PUBLIC_OR_RANDOM(DeviceInfo[DeviceCount-1].DeviceList.type),
                    DeviceInfo[DeviceCount-1].DeviceList.bdAddr[5],
                    DeviceInfo[DeviceCount-1].DeviceList.bdAddr[4],
                    DeviceInfo[DeviceCount-1].DeviceList.bdAddr[3],
                    DeviceInfo[DeviceCount-1].DeviceList.bdAddr[2],
                    DeviceInfo[DeviceCount-1].DeviceList.bdAddr[1],
                    DeviceInfo[DeviceCount-1].DeviceList.bdAddr[0],
                    DeviceInfo[DeviceCount-1].DeviceName
                    
                  );
            while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完
            return;
        }
        
        
    }
//    循环比较扫描到的地址是不是之前扫描过的,如果有之前扫描过的地址，则马上返回，否则马上把新的地址打印出来
    for(i=0;i<DeviceCount;i++)
    {        
        if(memcmp(ScanReport->peerBdAddr,DeviceInfo[i].DeviceList.bdAddr,6)==0)
        {
//            NewAddr=0;
            return;
        }
    }
    DeviceCount++;
    memcpy(DeviceInfo[DeviceCount-1].DeviceList.bdAddr,ScanReport->peerBdAddr,6);
    if(CYBLE_GAP_ADDR_TYPE_PUBLIC==ScanReport->peerAddrType)
    {
//        DeviceInfo[DeviceCount-1].DeviceList.type=CYBLE_GAP_ADDR_TYPE_PUBLIC;
        DeviceInfo[DeviceCount-1].DeviceList.type='P';
    }
    else
    {
//        DeviceInfo[DeviceCount-1].DeviceList.type=CYBLE_GAP_ADDR_TYPE_RANDOM;
        DeviceInfo[DeviceCount-1].DeviceList.type='R';
    }
    DeviceInfo[DeviceCount-1].idx=DeviceCount;        
    if(CYBLE_GAPC_CONN_UNDIRECTED_ADV==ScanReport->eventType)   
    {
        DeviceInfo[DeviceCount-1].rssi=ScanReport->rssi;
        Parser_adv_data_scan_data(ScanReport->data,ScanReport->dataLen);
        if(DeviceInfo[DeviceCount-1].ScanName)
            {
                DeviceInfo[DeviceCount-1].ScanName=FALSE;
            }
            else
            {
                memset(DeviceInfo[DeviceCount-1].DeviceName,0,sizeof(DeviceInfo[DeviceCount-1].DeviceName));
                memcpy(DeviceInfo[DeviceCount-1].DeviceName,"NULL",sizeof("NULL"));
            }
            printf("+SCAN=<%d,%d,%c,0x%02X%02X%02X%02X%02X%02X,%s>\r\n",
                    DeviceInfo[DeviceCount-1].idx,
                    DeviceInfo[DeviceCount-1].rssi,
                    DeviceInfo[DeviceCount-1].DeviceList.type,
                    DeviceInfo[DeviceCount-1].DeviceList.bdAddr[5],
                    DeviceInfo[DeviceCount-1].DeviceList.bdAddr[4],
                    DeviceInfo[DeviceCount-1].DeviceList.bdAddr[3],
                    DeviceInfo[DeviceCount-1].DeviceList.bdAddr[2],
                    DeviceInfo[DeviceCount-1].DeviceList.bdAddr[1],
                    DeviceInfo[DeviceCount-1].DeviceList.bdAddr[0],
                    DeviceInfo[DeviceCount-1].DeviceName
                    
                  );
            while((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) != 0);//等待串口缓冲区的数据发送完
    }
    
}

/******************************************************************************
* Function Name: Parser_adv_data_scan_data
***************************************************************************//**
* 
*  解析空中广播数据或者扫描响应数据
* \param  pdata:指向空中广播数据或者扫描响应数据的指针
* \param  total_len:空中广播数据或者扫描响应数据的长度
* \return
*  None
* 
******************************************************************************/
void Parser_adv_data_scan_data(uint8_t *pdata, uint8_t total_len)
{
    uint8_t* pend=pdata+total_len;
    uint8_t length=0;
    uint8_t i,ServiceCount;
    pdata++;
    while(pend>pdata)
    {
//      pdata-1表示的是对应内容的长度，pdata+1表示的是对应的内容
        switch(*pdata)
        {
            case CYBLE_GAP_ADV_FLAGS:
//                printf("Flags is %d\r\n",*(pdata+1));
                length=1+*(pdata-1);
            break;
            //比如有几个服务，那么就有几个服务的UUID，
            //广播中只要没有把全部的服务UUID，就是CYBLE_GAP_ADV_INCOMPL_16UUID
            case CYBLE_GAP_ADV_INCOMPL_16UUID:
//          服务ID的数量
                ServiceCount=(*(pdata-1)-1)/2;
                for(i=0;i<ServiceCount;i++)
                {                
                    switch(i)
                    {
                        case 0:
//                            printf("Service1 ID is: 0x%02X%02X\r\n",*(pdata+i+2),*(pdata+i+1));
                        break;                       
                        case 1:
//                            printf("Service2 ID is: 0x%02X%02X\r\n",*(pdata+i+3),*(pdata+i+2));
                        break;                    
                        case 2:
//                            printf("Service3 ID is: 0x%02X%02X\r\n",*(pdata+i+4),*(pdata+i+3));
                        break;
                        case 3:
//                            printf("Service4 ID is: 0x%02X%02X\r\n",*(pdata+i+5),*(pdata+i+4));
                        break;
                        case 4:
//                            printf("Service5 ID is: 0x%02X%02X\r\n",*(pdata+i+6),*(pdata+i+5));
                        break;
                        case 5:
//                            printf("Service6 ID is: 0x%02X%02X\r\n",*(pdata+i+7),*(pdata+i+6));
                        break;
                        case 6:
//                            printf("Service7 ID is: 0x%02X%02X\r\n",*(pdata+i+8),*(pdata+i+7));
                        break;
                        case 7:
//                            printf("Service8 ID is: 0x%02X%02X\r\n",*(pdata+i+9),*(pdata+i+8));
                        break;
                        case 8:
//                            printf("Service9 ID is: 0x%02X%02X\r\n",*(pdata+i+10),*(pdata+i+9));
                        break;
                        case 9:
//                            printf("Service10 ID is: 0x%02X%02X\r\n",*(pdata+i+11),*(pdata+i+10));
                        break;
                        default:
                        break;
                    }
                }
                length=1+*(pdata-1);
            break;
            case CYBLE_GAP_ADV_COMPL_16UUID:
                ServiceCount=(*(pdata-1)-1)/2;
                for(i=0;i<ServiceCount;i++)
                {                
                    switch(i)
                    {
                        case 0:
//                            printf("Service1 ID is: 0x%02X%02X\r\n",*(pdata+i+2),*(pdata+i+1));
                        break;                       
                        case 1:
//                            printf("Service2 ID is: 0x%02X%02X\r\n",*(pdata+i+3),*(pdata+i+2));
                        break;                    
                        case 2:
//                            printf("Service3 ID is: 0x%02X%02X\r\n",*(pdata+i+4),*(pdata+i+3));
                        break;
                        case 3:
//                            printf("Service4 ID is: 0x%02X%02X\r\n",*(pdata+i+5),*(pdata+i+4));
                        break;
                        case 4:
//                            printf("Service5 ID is: 0x%02X%02X\r\n",*(pdata+i+6),*(pdata+i+5));
                        break;
                        case 5:
//                            printf("Service6 ID is: 0x%02X%02X\r\n",*(pdata+i+7),*(pdata+i+6));
                        break;
                        case 6:
//                            printf("Service7 ID is: 0x%02X%02X\r\n",*(pdata+i+8),*(pdata+i+7));
                        break;
                        case 7:
//                            printf("Service8 ID is: 0x%02X%02X\r\n",*(pdata+i+9),*(pdata+i+8));
                        break;
                        case 8:
//                            printf("Service9 ID is: 0x%02X%02X\r\n",*(pdata+i+10),*(pdata+i+9));
                        break;
                        case 9:
//                            printf("Service10 ID is: 0x%02X%02X\r\n",*(pdata+i+11),*(pdata+i+10));
                        break;
                        default:
                        break;
                    }
                }
                length=1+*(pdata-1);
            break;           
            case CYBLE_GAP_ADV_INCOMPL_32_UUID:
                
            break;
            case CYBLE_GAP_ADV_COMPL_32_UUID:
            
            break;
            case CYBLE_GAP_ADV_INCOMPL_128_UUID:
            
            break;
            case CYBLE_GAP_ADV_COMPL_128_UUID:
            
            break;  
            case CYBLE_GAP_ADV_SHORT_NAME:
                //提前初始化DeviceName指针的前面20个Byte
                memset(DeviceInfo[DeviceCount-1].DeviceName,0,sizeof(DeviceInfo[DeviceCount-1].DeviceName));
//                memcpy(DeviceName,(pdata+1),*(pdata-1)-1);
//                printf("Shortened Local Name is: %s\r\n",DeviceName);
//                printf("%s>\r\n",DeviceName);
                memcpy(DeviceInfo[DeviceCount-1].DeviceName,(pdata+1),*(pdata-1)-1);
                length=1+*(pdata-1);
            break;
            case CYBLE_GAP_ADV_COMPL_NAME:
                //提前初始化DeviceName指针的前面20个Byte
                memset(DeviceInfo[DeviceCount-1].DeviceName,0,sizeof(DeviceInfo[DeviceCount-1].DeviceName));
//                memcpy(DeviceName,(pdata+1),*(pdata-1)-1);
                memcpy(DeviceInfo[DeviceCount-1].DeviceName,(pdata+1),*(pdata-1)-1);
                DeviceInfo[DeviceCount-1].ScanName=TRUE;
//                printf("Complete Local Name is: %s\r\n",DeviceInfo[DeviceCount-1].DeviceName);                
//                printf("%s>\r\n",DeviceName);
                length=1+*(pdata-1);
            break;
            case CYBLE_GAP_ADV_TX_PWR_LVL:
//                printf("Tx Power Level is: %ddbm\r\n",*(pdata+1));
                length=1+*(pdata-1);
            break;
            case CYBLE_GAP_ADV_CLASS_OF_DEVICE:
                
            break;
            
            case CYBLE_GAP_ADV_SMPL_PAIR_HASH_C:
                
            break;
            case CYBLE_GAP_ADV_SMPL_PAIR_RANDOM_R:
                
            break;     
            //CYBLE_GAP_ADV_SCRT_MNGR_TK_VAL的枚举值
            //跟CYBLE_GAP_ADV_DEVICE_ID相同    
            case CYBLE_GAP_ADV_DEVICE_ID:
                
            break;
            case CYBLE_GAP_ADV_SCRT_MNGR_OOB_FLAGS:
                
            break;   
            case CYBLE_GAP_ADV_SLAVE_CONN_INTRV_RANGE:
                
            break;    
            case CYBLE_GAP_ADV_SOLICIT_16UUID:
                
            break;     
            case CYBLE_GAP_ADV_SOLICIT_128UUID:
                
            break; 
            case CYBLE_GAP_ADV_SRVC_DATA_16UUID:
                
            break;    
            case CYBLE_GAP_ADV_PUBLIC_TARGET_ADDR:
                
            break;  
            case CYBLE_GAP_ADV_RANDOM_TARGET_ADDR:
                
            break;    
            case CYBLE_GAP_ADV_APPEARANCE:
                
            break;     
            case CYBLE_GAP_ADV_ADVERT_INTERVAL:
                
            break;   
            case CYBLE_GAP_ADV_LE_BT_DEVICE_ADDR:
                
            break;
            case CYBLE_GAP_ADV_LE_ROLE:
                
            break;
            case CYBLE_GAP_ADV_SMPL_PAIR_HASH_C256:
                
            break;
            case CYBLE_GAP_ADV_SMPL_PAIR_RANDOM_R256:
                
            break;
            case CYBLE_GAP_ADV_SOLICIT_32UUID:
                
            break;  
            case CYBLE_GAP_ADV_SRVC_DATA_32UUID:
                
            break;
            case CYBLE_GAP_ADV_SRVC_DATA_128UUID:
                
            break;
            case CYBLE_GAP_ADV_3D_INFO_DATA:
                
            break;
            default:                
            break;
                
        }
        pdata+=length;
    }    
//    printf("<%d,",DeviceCount);
}


/******************************************************************************
* Function Name: ScanDeviceInfoInit
***************************************************************************//**
* 
*  初始化存放扫描到的BLE设备信息空间
* 
* \return
*  None
* 
******************************************************************************/
void ScanDeviceInfoInit(void)
{
    uint8_t i;
    if(DeviceCount>DeviceListCount)
    {
        for(i=0;i<DeviceListCount;i++)
        {
           memset(DeviceInfo[i].DeviceList.bdAddr,0,6);
        
        }        
    }
    else
    {
        for(i=0;i<DeviceCount;i++)
        {
            memset(DeviceInfo[i].DeviceList.bdAddr,0,6);
        }
    }
    //计数扫描到的设备清零，以便再次扫描时扫描到的设备信息覆盖旧的信息
      DeviceCount=0;
      flag=0;

}
/* [] END OF FILE */
