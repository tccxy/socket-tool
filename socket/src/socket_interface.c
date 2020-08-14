/**
 * @file socket_interface.c
 * @author zhao.wei (hw)
 * @brief 
 * @version 0.1
 * @date 2020-08-11
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "pub.h"
#include "socket_interface.h"
#include "rcvdata_structure.h"

/*
    \>fd_list
   ------              \>write_p
   \sfd1\====>\data1 \data2 \data3 \data4 \data5 \data6 \....=>rcv_data_structure
   ------        \>read_p
   \sfd2\
   ------
   \sfd3\
   ------
   \...\
*/
//fd_list 操作总结点
LLIST *FD_LIST_HANDLE = NULL;

/**
 * @brief 便利打印的实现
 * 
 * @param data 
 */
void op_ls(void *data)
{
    struct rcv_sockt_fd_msg *msg = (struct rcv_sockt_fd_msg *)data;

    printf("fd is %x \r\n", msg->s_fd);
}

/**
 * @brief 比较fd的实现函数
 * 
 * @param key 键值
 * @param data 数据
 * @return int 0代表成功
 */
u32 cmp_fd(void *key, void *data)
{
    u32 key_fd = *((u32 *)(key));
    struct rcv_sockt_fd_msg *msg = (struct rcv_sockt_fd_msg *)data;

    if ((key_fd) == (msg->s_fd))
        return SUCCESS;
    else
        return ERROR;
}

/**
 * @brief 创建sockefd的数据结构
 * 
 * @return u32 操作结果 0为成功
 */
u32 creat_socket_fd_list()
{
    FD_LIST_HANDLE = llist_create(sizeof(struct rcv_sockt_fd_msg));
    if (NULL == FD_LIST_HANDLE)
        return ERROR_HANDLE_CREAT;
    else
        return SUCCESS;
}

/**
 * @brief 查找socketfd是否已在列表中
 * 
 * @param fdkey fd键值
 * @param data 如果存在返回数据指针，不存在返回空指针
 * @return u32 操作结果 0为成功
 */
u32 find_socket_fd_list(void *fdkey, void **data)
{
    void *data_p = NULL;
    if (NULL == FD_LIST_HANDLE)
        return ERROR_HANDLE_CREAT;
    data_p = llist_ind(fdkey, cmp_fd, FD_LIST_HANDLE);
    *data = data_p;
    if (NULL == data)
        return ERROR;
    else
        return SUCCESS;
}

/**
 * @brief 新增socketfd表中一个节点
 * 
 * @param fdkey 键值
 * @param data 增加成功返回数据指针，失败返回空指针
 * @return u32 操作结果 0为成功
 */
u32 add_socket_fd_list(void *fdkey, void **data)
{
    void *data_p = NULL;
    struct rcv_sockt_fd_msg tmp_msg = {0};
    if (NULL == FD_LIST_HANDLE)
        return ERROR_HANDLE_CREAT;
    tmp_msg.s_fd = *((u32 *)fdkey);
    if (SUCCESS == llist_append(&tmp_msg, FD_LIST_HANDLE, &data_p))
    {
        DEBUG("add_socket_fd_list %p \r\n", data_p);
        *data = data_p;
        return SUCCESS;
    }
    else
        return ERROR_HANDLE_ADD;
}

/**
 * @brief 删除socketfd表中的指定的节点
 * 
 * @param fdkey 键值
 * @return u32 操作结果 0为成功
 */
u32 del_socket_fd_list(void *fdkey)
{
    if (NULL == FD_LIST_HANDLE)
        return ERROR_HANDLE_CREAT;
    if (SUCCESS == llist_del(fdkey, cmp_fd, FD_LIST_HANDLE))
        return SUCCESS;
    else
        return ERROR_HANDLE_DEL;
}

/**
 * @brief 获取socktfd表的总条目
 * 
 * @return u32 返回条目数
 */
u32 get_socket_fd_list_num()
{
    if (NULL == FD_LIST_HANDLE)
        return ERROR_HANDLE_CREAT;
    return llist_num(FD_LIST_HANDLE);
}

/**
 * @brief 向横向的循环表中插入数据
 * 
 * @param data 要写入的数据
 * @param stru 存储结构
 * @return u32 操作结果
 */
u32 write_rcv_data_stru(void *data, struct rcv_data_structure *stru)
{
    if (strlen(data) > RCV_DATA_BUF_SIZE)
        return ERROR_SINGLE_RCV_LEN;
    memcpy(stru->data_buf[stru->write_pos].data, data, strlen(data));
    stru->data_buf[stru->write_pos].data_len = strlen(data);
    //写指针循环后移
    stru->write_pos = (stru->write_pos + 1) % RCV_DATA_BUF_NUM;

    //如果写指针碰到了头指针，则读指针循环后移
    if (stru->write_pos == stru->read_pos)
        stru->read_pos = (stru->read_pos + 1) % RCV_DATA_BUF_NUM;

    return SUCCESS;
}

/**
 * @brief 从横向循环表中读出数据
 * 
 * @param data 读出的数据
 * @param stru 存储结构
 * @return u32 实际读取的数据长度
 */
u32 read_rcv_data_stru(void *data, struct rcv_data_structure *stru)
{
    u32 len = 0;
    //没有数据可以读取
    if (stru->read_pos == stru->write_pos)
        return len;
    len = stru->data_buf[stru->read_pos].data_len;
    memcpy(data, stru->data_buf[stru->read_pos].data, len);

    //读指针循环后移
    stru->read_pos = (stru->read_pos + 1) % RCV_DATA_BUF_NUM;

    return len;
}