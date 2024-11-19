### cubemx配置

![image-20241119182724582](C:\Users\HAN\AppData\Roaming\Typora\typora-user-images\image-20241119182724582.png)

![image-20241119182741691](C:\Users\HAN\AppData\Roaming\Typora\typora-user-images\image-20241119182741691.png)

![image-20241119182819080](C:\Users\HAN\AppData\Roaming\Typora\typora-user-images\image-20241119182819080.png)

### 数据结构

| Byte Index  | Value (Hex)  | Description                    |
| ----------- | ------------ | ------------------------------ |
| rx_buf[0]   | 0xA5         | 帧头 (Start of Frame)          |
| rx_buf[1]   |              | 数据段长度 (低字节)            |
| rx_buf[2]   |              | 数据段长度 (高字节)            |
| rx_buf[3]   | 参考crc_8.c  | 帧头的 CRC8 校验值             |
| rx_buf[4]   | 0x11         | 命令字 (低字节)                |
| rx_buf[5]   | 0x11         | 命令字 (高字节)                |
| rx_buf[6]   |              | 数据段的第 1 字节              |
| rx_buf[7]   |              | 数据段的第 2 字节              |
| rx_buf[8]   |              | 数据段的第 3 字节              |
| rx_buf[9]   |              | 数据段的第 4 字节              |
| rx_buf[n-1] | 参考crc_16.c | 数据段的 CRC16 校验值 (低字节) |
| rx_buf[n]   |              | 数据段的 CRC16 校验值 (高字节) |

### 测试数据

```
A5 04 00 F8 11 11 11 11 11 11 4B BD
```

### 说明

- **帧头 (Start of Frame)**: 用于标识数据帧的开始，值为 `0xA5`。
- **数据段长度**: 包括低字节和高字节，用于指示数据段的长度，支持自定义。
- **帧头的 CRC8 校验值**: 用于校验帧头的完整性。
- **命令字**: 用于指示自定义命令的类型，低字节和高字节组合为 `0x1111`。
- **数据段**: 包含实际的数据，每个字节根据需要填充。如要发送float给上位机，要先强转成4位uint8_t
- **数据段的 CRC16 校验值**: 用于校验数据段的完整性。

