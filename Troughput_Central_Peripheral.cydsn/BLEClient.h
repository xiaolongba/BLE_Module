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


#ifndef BLECLIENT_H
#define BLECLIENT_H
    
#include <project.h>    

/***************************************
* 宏定义
***************************************/    
#define TRUE    (1)
#define FALSE   (0)    
#define DeviceNameLength        (20)    
//可以保存10个BLE设备地址    
#define DeviceListCount         (0x0A)

/***************************************
* 函数原形
***************************************/    
void HandleScanDevices(CYBLE_GAPC_ADV_REPORT_T* scanReport);
void Parser_adv_data_scan_data(uint8_t *pdata, uint8_t total_len);
void ScanDeviceInfoInit(void);
/***************************************
* 结构体定义
***************************************/      
typedef struct
{
    CYBLE_GAP_BD_ADDR_T DeviceList;  
    uint8_t idx;
    int8_t rssi;
    int8_t DeviceName[DeviceNameLength];
    //广播包中有名字则是1，否则为0
    uint8_t ScanName;
}CYBLE_DEVICE_INFO;

/***************************************
* 宏定义
***************************************/ 
#define PUBLIC_OR_RANDOM(type)          type==1?'R':'P'  
#endif    
/* [] END OF FILE */
