#ifndef _USB_H_
#define _USB_H_
#include <stdio.h>
#include <stdint.h>
#include "usbd_cdc_if.h"
#include "usb.h"
#include "daemon.h"
#define PROTOCOL_CMD_ID 0XA5
#define OFFSET_BYTE     8 // 出数据段外，其他部分所占字节数
typedef struct
{
    struct
    {
        uint8_t sof;
        uint16_t data_length;
        uint8_t crc_check; // 帧头CRC校验
    } header;              // 数据帧头
    uint16_t cmd_id;       // 数据ID
    uint16_t frame_tail;   // 帧尾CRC校验
} protocol_struct;
// typedef void (*USBCallback)(uint8_t *rx_buf, uint32_t Len, uint8_t *rx_data); // USB事件回调函数类型
typedef struct
{
    USBCallback tx_cbk;
    USBCallback rx_cbk;
} USB_Init_Config_s;
uint8_t USB_Data_Send(uint8_t *Buf, uint16_t Len);
void example_init(uint8_t *vis_recv_buff);

#endif
