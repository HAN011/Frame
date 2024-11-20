/**
 * @Author:CZH Ccczorange
 * @Date: 2024-11-18 16:47:43
 * @LastEditors:
 * @LastEditTime:
 * @FilePath:
 * @Description:修改了usbd_cdc_if.c中的CDC_Receive_FS函数,若使用cube生成后会被覆盖
 * 支持自定义数据长度发送
    +-----------+-----------+-----------------------+
    | Byte Index| Value (Hex)| Description           |
    +-----------+-----------+-----------------------+
    | rx_buf[0] |    0xA5   | 帧头 (Start of Frame) |
    | rx_buf[1] |           | 数据段长度 (低字节)    |
    | rx_buf[2] |           | 数据段长度 (高字节)    |
    | rx_buf[3] |           | 帧头的 CRC8 校验值     |
    | rx_buf[4] |    0x11   | 命令字 (低字节)        |
    | rx_buf[5] |    0x11   | 命令字 (高字节)        |
    | rx_buf[6] |           | 数据段的第 1 字节      |
    | rx_buf[7] |           | 数据段的第 2 字节      |
    | rx_buf[8] |           | 数据段的第 3 字节      |
    | rx_buf[9] |           | 数据段的第 4 字节      |
    | rx_buf[n-1]|           | 数据段的 CRC16 校验值 (低字节) |
    | rx_buf[n]|           | 数据段的 CRC16 校验值 (高字节) |

    测试数据：A5 04 00 F8 11 11 11 11 11 11 4B BD
    +-----------+-----------+-----------------------+
*/
#include "usb.h"
#include "crc8.h"
#include "crc16.h"
#include "memory.h"
#include "usbd_cdc_if.h"
static uint8_t reg_flag = 0;   // 注册标识符，保证板子只注册一个USB实例，多了就报错
static uint8_t *bsp_usb_rx_buffer; // 接收到的原始数据会被放在这里
uint8_t test_receive_data[10]={};
static DaemonInstance *vision_daemon_instance;
// 注意usb单个数据包(Full speed模式下)最大为64byte,超出可能会出现丢包情况
static void USBRefresh()
{
    // 重新枚举usb设备
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
    HAL_Delay(0.1);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
    HAL_Delay(0.1);
}

void USBOfflineCallback()
{
    // USBRefresh();
}

static uint8_t *USB_Init(USB_Init_Config_s conf)
{
    if (reg_flag == 1) {//已经注册了一个实例
    //打印错误日志
    while (1) LOGERROR("[bsp_usb]You cant register more instance for usb.");
    }
    bsp_usb_rx_buffer = CDCInitRxbufferNcallback(conf.tx_cbk, conf.rx_cbk); // 获取接收数据指针
    reg_flag=1;
    LOGINFO("USB init success");
    Daemon_Init_Config_s daemon_conf = {
        .callback     = USBOfflineCallback, // 离线时调用的回调函数,会重启串口接收
        .owner_id     = NULL,
        .reload_count = 10,
    };
    vision_daemon_instance = DaemonRegister(&daemon_conf);

    return bsp_usb_rx_buffer;
}
/*检验CRC8数据段*/
static uint8_t CRC8_Check_Sum(uint8_t *pchMessage, uint16_t dwLength)
{
    uint8_t ucExpected = 0;
    if ((pchMessage == 0) || (dwLength <= 2))
        return 0;
    ucExpected = crc_8(pchMessage, dwLength - 1);
    return (ucExpected == pchMessage[dwLength - 1]);
}

/*检验数据帧头*/
static uint8_t protocol_heade_Check(protocol_struct *pro, uint8_t *rx_buf)
{
    if (rx_buf[0] == PROTOCOL_CMD_ID) {
        pro->header.sof = rx_buf[0];
        if (CRC8_Check_Sum(&rx_buf[0], 4)) {
            pro->header.data_length = (rx_buf[2] << 8) | rx_buf[1];
            pro->header.crc_check   = rx_buf[3];
            pro->cmd_id             = (rx_buf[5] << 8) | rx_buf[4];
            return 1;
        }
    }
    return 0;
}
/*检验CRC16数据段*/
static uint16_t CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength)
{
    uint16_t wExpected = 0;
    if ((pchMessage == 0) || (dwLength <= 2)) {
        return 0;
    }
    wExpected = crc_16(pchMessage, dwLength - 2);
    return (((wExpected & 0xff) == pchMessage[dwLength - 2]) && (((wExpected >> 8) & 0xff) == pchMessage[dwLength - 1]));
}

static void USB_Data_Process(uint8_t *rx_buf, uint8_t *rx_data)
{
    // 放在静态区,避免反复申请栈上空间
    static protocol_struct pro;
    static uint16_t date_length;
    if (protocol_heade_Check(&pro, rx_buf))
    {
        date_length = OFFSET_BYTE + pro.header.data_length;
        if (CRC16_Check_Sum(&rx_buf[0], date_length))
        {
            memcpy(rx_data, rx_buf + 6, pro.header.data_length);
        }
    }
}

uint8_t USB_Data_Send(uint8_t *Buf, uint16_t Len){
    DaemonReload(vision_daemon_instance); // 喂狗
    return CDC_Transmit_FS(Buf, Len);
}

static void USB_Data_Decode(uint32_t recv_len)
{
    DaemonReload(vision_daemon_instance); // 喂狗
    USB_Data_Process(bsp_usb_rx_buffer, test_receive_data);
}

void example_init(uint8_t *vis_recv_buff)
{
    USB_Init_Config_s conf = {.rx_cbk = USB_Data_Decode};
    vis_recv_buff          = USB_Init(conf);
}
