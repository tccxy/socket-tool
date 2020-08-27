/**
 * @file socket_core_udp_client.c
 * @author zhao.wei (hw)
 * @brief 
 * @version 0.1
 * @date 2020-08-19
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "pub.h"

/**
 * @brief 设置udp组ip
 * 
 * @param group_ip 组ip 224.x.x.x
 * @return u32 
 */
u32 socket_set_udp_group(u8 *group_ip)
{
    struct ip_mreq group = {0};
    u32 ret;

    //格式错误
    if (INADDR_NONE == inet_addr((char *)(group_ip)))
        return ERROR_SOCKET_SET;
    group.imr_multiaddr.s_addr = inet_addr((char *)(group_ip));
    group.imr_interface.s_addr = inet_addr("0.0.0.0");
    ret = setsockopt(global_select_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &group, sizeof(group));
    if (ret < 0)
    {
        printf("setsockopt failure %x ,please Wait a moment try again .\r\n", ret);
        return ERROR_SOCKET_SET;
    }
    return SUCCESS;
}

/**
 * @brief udp的deal处理
 * 
 * @param cfd 
 * @return void* 
 */
void *udp_deal(void *cfd)
{
    char buf[RCV_DATA_BUF_SIZE] = {0};
    u32 size;
    s32 socket_fd = 0;
    void *data_p = NULL;
    struct sockaddr_in client = {0};
    u32 client_address_size = (sizeof(client));
    struct rcv_sockt_fd_msg *msg = NULL;

    socket_fd = *((u32 *)(cfd));
    if (SUCCESS == find_socket_fd_list((void *)&socket_fd, &data_p))
    {
        DEBUG("find_socket_fd_list addr  %p\r\n", data_p);
        msg = (struct rcv_sockt_fd_msg *)data_p;
        DEBUG("udp_deal has find %d table  \r\n", socket_fd);
    }
    while (1)
    {
        memset(buf, '\0', sizeof(buf));
        //size = read(socket_fd, buf, sizeof(buf));
        size = recvfrom(socket_fd, buf, sizeof(buf), 0, (struct sockaddr *)&client,
                        &client_address_size);
        if (size <= 0) //没有接收到数据，关闭描述符，释放在TCPServer申请的空间
        {
            DEBUG("%x has exit \r\n", connect_fd);
            //删除客户端fd
            pthread_mutex_lock(&global_select_fd_mutex);
            if (socket_fd == global_select_fd)
            {
                global_select_fd = 0x3f;

                //重新刷新下命令提示符,暂时先这样处理
                //printf("\r\nsockt_tool @%c >>", global_select_fd);
                //fflush(stdout);
            }
            pthread_mutex_unlock(&global_select_fd_mutex);

            break;
        }
        else
        {
            DEBUG("cip %x fip %x \r\n", client.sin_addr.s_addr, msg->c_addr.sin_addr.s_addr);
            if (msg->pad == TRUE) //使能了消息过滤
            {
                if (client.sin_addr.s_addr == msg->c_addr.sin_addr.s_addr)
                {
                    write_rcv_data_stru(buf, &(msg->rcv_data)); //像buff中写入数据
                }
            }
            else
            {
                write_rcv_data_stru(buf, &(msg->rcv_data)); //像buff中写入数据
            }
        }
    }
    return SUCCESS;
}

/**
 * @brief udp服务端初始化
 * 
 * @param addr 地址信息
 * @return u32 操作结果
 */
u32 socket_init_udp(struct sockaddr_in *addr)
{
    s32 socket_fd = 0;
    s32 ret;
    void *data_p = NULL;
    pthread_t thread_id = 0;

    /* 构造用于UDP通信的套接字 */
    socket_fd = socket(addr->sin_family, SOCK_DGRAM, 0);
    if (socket_fd < 0)
    {
        printf("creat socket fail\n");
        return ERROR_SOCKET_CREAT;
    }

    int opt = 1;
    // sockfd为需要端口复用的套接字
    ret = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt));
    if (ret < 0)
    {
        printf("setsockopt reuse failure %x  .\r\n", ret);
        return ERROR_SOCKET_SET;
    }

    ret = bind(socket_fd, (const struct sockaddr *)addr, sizeof(struct sockaddr));
    if (ret < 0)
    {
        printf("bind failure %x ,please Wait a moment try again .\r\n", ret);
        return ERROR_SOCKET_BIND;
    }

    global_select_fd = socket_fd;
    //增加socketfd的表
    add_socket_fd_list((void *)&socket_fd, &data_p);
    ret = pthread_create(&thread_id, NULL, udp_deal, (void *)&socket_fd);
    if (0 != ret)
    {
        return ERROR_SOCKET_CREAT_PTH;
    }
    while (1)
    {
        sleep(10);
    }
    return SUCCESS;
}
