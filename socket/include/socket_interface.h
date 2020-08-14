/**
 * @file socket_interface.h
 * @author zhao.wei (hw)
 * @brief 
 * @version 0.1
 * @date 2020-08-11
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef _SOCKET_INTERFACE_H_
#define _SOCKET_INTERFACE_H_

#define SOCKET_SERVER_RCV_CONNECT_MAX 6 //服务端接收的最大连接数
#define RCV_DATA_BUF_SIZE 1024          //每一次接收的最大字节数
#define RCV_DATA_BUF_NUM 128            //每一个socket拥有的最大缓存个数

extern u32 global_socket_fd;//全局socketfd
/**
 * @brief 一个接收包的大小
 * 
 */
struct rcv_data_buf
{
    u8 data[RCV_DATA_BUF_SIZE];
    u32 data_len;
};

/**
 * @brief 对于每一个socket的接收真正意义的存储结构，采取循环的方式进行
 * 
 */
struct rcv_data_structure
{
    struct rcv_data_buf data_buf[RCV_DATA_BUF_NUM];
    u16 write_pos;
    u16 read_pos;
};

/**
 * @bri每一个socket的描述符有一个属于自己的数据存储
 *   
  */
struct rcv_sockt_fd_msg
{
    u32 s_fd;
    struct rcv_data_structure rcv_data;
};

void *socket_init(void *data);
u32 creat_socket_fd_list(void);
u32 find_socket_fd_list(void *fdkey, void **data);
u32 add_socket_fd_list(void *fdkey, void **data);
u32 del_socket_fd_list(void *fdkey);
u32 get_socket_fd_list_num();
u32 write_rcv_data_stru(void *data, struct rcv_data_structure *stru);
u32 read_rcv_data_stru(void *data, struct rcv_data_structure *stru);

#endif