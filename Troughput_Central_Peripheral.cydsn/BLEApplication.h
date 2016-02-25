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
#ifndef BLEAPPLICATION_H
#define BLEAPPLICATION_H
#include <project.h>    
/***************************************
* 函数原形
***************************************/    
void StackEventHandler(uint32 eventCode, void *eventParam);    
void LowPowerManagement(void);
int _write(int file, char *ptr, int len);
CY_ISR_PROTO(MY_ISR_USERBUTTON);
void SystemInitialization(void);
void LedInit(void);
CY_ISR_PROTO(My_ISR_UART);
CY_ISR_PROTO(MyTimerIsr);
void Parser_UartData(const char* SerialData);
uint8_t Command_Identify(const char* SerialData);
void ConfigUartInit(void);
void SetUartConfiguration(uint32_t DIVIDER_CLOCK);
void SendNotification(uint8_t*val,uint16_t len);
void SendData_toSlave(uint8_t*val,uint16_t len);
void Master_Slave_UartHandler(uint8_t Role);
void Connect_Device(uint8_t idx);
void UartBuadRate_Handler(uint8_t idx);
void TxPower_Handler(uint8_t idx);
void StrToHex(char *pbDest, char *pbSrc, int nLen);
/***************************************
* 宏定义
***************************************/

//#define LOW_POWER_EN
#define LED_OFF             (1)
#define LED_ON              (0)
#define CTS_OFF             (1)
#define CTS_ON              (0)
#define TURE                (1)
#define FALSE               (0)
#define BUFFER_LENGHTH      (48u)
#define AT_COMMAND_LENGTH   (18)
#define MAX_CONNECT_COUNT   (0x04u)
#define UART_TX_BUFFER      (20)
#define UART_RX_BUFFER      (2048u)
#define Peripheral          (0u)
#define Central             (1u)
#define MAX_MTU_SIZE        (51u)
#define DEFAULT_MTU_SIZE    (23u)
#define THROUGHT_MODE       (1u)
#define AT_COMMAND_MODE     (0u)    
#define UART_IDLE_TIMEOUT   (7000u)
#define FLOW_CONTROL
#define UART_RX_INTR_MASK     0x00000004
#define UART_RX_INT_ENABLE()  UART_INTR_RX_MASK_REG |= UART_RX_INTR_MASK
#define UART_RX_INT_DISABLE() UART_INTR_RX_MASK_REG &= ~UART_RX_INTR_MASK
/***************************************
* 串口波特率设置
* Div SCBCLK=48MHz/BAUD_RATE*OVERSAMPLE-1
* 其中注释的是CYPRESS不支持的波特率
***************************************/
#define BAUD_1200           (2499u)   
#define BAUD_2400           (1249u)
#define BAUD_4800           (624u)
#define BAUD_9600           (312u)
//#define BAUD_14400          (207u)
#define BAUD_19200          (155u)
//#define BAUD_28800          (103u)
#define BAUD_38400          (77u)
#define BAUD_57600          (51u)
//#define BAUD_64000          (46u)
//#define BAUD_76800          (38u)
#define BAUD_115200         (25u)   /* UART: 115200 kbps with OVS = 16. Required SCBCLK = 1.846 MHz, Div = 26 */ 
//#define BAUD_128000         (22u)
#define BAUD_230400         (12u)
//#define BAUD_345600         (8u)
#define BAUD_460800         (6u)
#define BAUD_921600         (2u)

      


/***************************************
* AT命令宏定义
***************************************/
#define AT_RESET        "AT+RESET"
#define AT_REFAC        "AT+REFAC"
#define AT_VERSION      "AT+VERSION"
#define AT_BAUD         "AT+BAUD"
#define AT_LADDR        "AT+LADDR"
#define AT_NAME         "AT+NAME"
#define AT_CPIN         "AT+CPIN"
#define AT_TXP          "AT+TXP"
#define AT_ADVD         "AT+ADVD"
#define AT_ADVI         "AT+ADVI"
#define AT_CONNI        "AT+CONNI"
#define AT_ROLE         "AT+ROLE"
#define AT_SCAN         "AT+SCAN"
#define AT_CONNT        "AT+CONNT"
#define AT_DISCONN      "AT+DISCONN"
#define AT_LOGIN        "AT+LOGIN"
#define AT_TX           "AT+TX"
#define AT_RSSI         "AT+RSSI"
#define AT_DISALLCHAR   "AT+DISALLCHAR"
#define AT              "AT"
#define AT_SPEED        "AT+SPEED"
#define AT_NOTIFY       "AT+NOTIFY"
#define AT_STAUS        "AT+STAUS"
#define AT_ADVS         "AT+ADVS"
#define AT_AUTH         "AT+AUTH"
#define AT_IOCAP        "AT+IOCAP"

/***************************************
* AT命令枚举定义
***************************************/
typedef enum
{
//    功能：模块复位
    RESET = 0x00u,
//    功能：恢复出厂设置
    REFAC,
//    功能：模块固件版本查询
    VERSION,
//    功能：UART 波特率查询、修改
    BAUD,
//    功能：本机蓝牙地址查询
    LADDR,
//    功能：设备名查询、修改 
    NAME,
//    功能： 密码状态查询、修改 
    CPIN,
//    功能：RF 发射功率查询、修改
    TXP,
//    功能：广播数据查询、修改
    ADVD,
//    功能：广播周期查询、修改
    ADVI,
//    功能：连接周期查询、修改
    CONNI,
//    功能：主从模式切换指令(主机指令)
    ROLE,
//    功能：扫描 BLE 设备(主机指令) 
    SCAN,
//    功能：连接 BLE 设备(主机指令)
    CONNT,
//    功能：与 BLE 从机设备断开连接(主机指令)
    DISCONN,
//    功能：密码登录(主机指令)
    LOGIN,
//    功能：发送数据(主机指令)
    TX,
//    功能：信号强度查询(主机指令)
    RSSI,
//      功能：查询所有特征值(主机指令)
    DISALLCHAR,
//      功能：AT命令测试  
    _AT,
//    功能：透传速度测试
    SPEED,
//    功能：打开透传通道
    NOTIFY,
//    功能：查询当前蓝牙状态
    STAUS,
//    功能：广播的开关    
    ADVS,
//    功能：认证绑定    
    AUTH,    
//    功能：认证时设备IO的显示能力 
    IOCAP
}AT_COMMAND;

/***************************************
* 连接及断开连接的各个参数定义
***************************************/
typedef struct
{
    //连接的设备索引
    uint8_t Connect_Idx[MAX_CONNECT_COUNT];
    uint8_t Disconn_Idx[MAX_CONNECT_COUNT];
    //实际连接个数
    uint8_t Connected_Count;
    //保存连接的设备的MAC地址
    CYBLE_GAP_BD_ADDR_T DeviceAddr[MAX_CONNECT_COUNT];
}CYBLE_CONN_OR_DISCONN_INFO;


#endif

/* [] END OF FILE */
