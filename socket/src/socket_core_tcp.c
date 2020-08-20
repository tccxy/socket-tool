/**
 * @file socket_core.c
 * @author zhao.wei (hw)
 * @brief 
 * @version 0.1
 * @date 2020-08-07
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "pub.h"
//#include "socket_tool.h"

//客户端fd
static u32 client_fd[SOCKET_SERVER_RCV_CONNECT_MAX] = {0};

/**
 * @brief 客户端的线程接收函数
 * 
 * @param cfd connect_fd
 */
void *tcp_server_deal(void *cfd)
{
    char buf[RCV_DATA_BUF_SIZE] = {0};
    u32 size;
    int connect_fd = 0;
    void *data_p = NULL;
    struct rcv_sockt_fd_msg *msg = NULL;

    connect_fd = *((u32 *)(cfd));
    if (SUCCESS == find_socket_fd_list((void *)&connect_fd, &data_p))
    {
        DEBUG("find_socket_fd_list addr  %p\r\n", data_p);
        msg = (struct rcv_sockt_fd_msg *)data_p;
        DEBUG("tcp_server_deal has find %d table  \r\n", connect_fd);
    }
    while (1)
    {
        memset(buf, '\0', sizeof(buf));
        size = read(connect_fd, buf, sizeof(buf));
        if (size <= 0) //没有接收到数据，关闭描述符，释放在TCPServer申请的空间
        {
            DEBUG("%x has exit \r\n", connect_fd);
            //删除客户端fd
            pthread_mutex_lock(&global_select_fd_mutex);
            if (connect_fd == global_select_fd)
            {
                global_select_fd = 0x3f;

                //重新刷新下命令提示符,暂时先这样处理
                //printf("\r\nsockt_tool @%c >>", global_select_fd);
                //fflush(stdout);
            }
            pthread_mutex_unlock(&global_select_fd_mutex);
            for (int i = 0; i < SOCKET_SERVER_RCV_CONNECT_MAX; i++)
            {
                if (client_fd[i] == connect_fd)
                {
                    client_fd[i] = 0;
                    del_socket_fd_list((void *)&connect_fd);
                    break;
                }
            }
            close(connect_fd);

            break;
        }
        else
        {
            DEBUG("TCP_Analyzer:%s,%d %p\r\n", buf, (int)size, msg);
            write_rcv_data_stru(buf, &(msg->rcv_data)); //像buff中写入数据
        }
    }
    return SUCCESS;
}

/**
 * @brief tcp服务端初始化
 * 
 * @param addr 地址信息
 * @return u32 操作结果
 */
u32 socket_int_tcp_server(struct sockaddr_in *addr)
{
    u32 i = 0;
    u32 yes = 0;
    u32 socket_fd = 0;
    u32 connect_fd = 0;
    s32 ret = 0;
    pthread_t thread_id = 0;
    struct sockaddr_in client_address;
    socklen_t address_len;
    void *data_p = NULL;
    struct rcv_sockt_fd_msg *msg = NULL;

    socket_fd = socket(addr->sin_family, SOCK_STREAM, 0);
    if ((socket_fd) < 0)
    {
        DEBUG("creat socket_fd failure \r\n");
        return ERROR_SOCKET_CREAT;
    }
    DEBUG("creat socket_fd success \r\n");
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)); // 允许IP地址复用

    ret = bind(socket_fd, (const struct sockaddr *)addr, sizeof(struct sockaddr));
    if (ret < 0)
    {
        printf("bind failure %x ,please Wait a moment try again .\r\n", ret);
        exit(0);
        return ERROR_SOCKET_BIND;
    }
    DEBUG("bind success \r\n");
    if (listen(socket_fd, SOCKET_SERVER_RCV_CONNECT_MAX) < 0)
    {
        DEBUG("listen failure\r\n");
        return ERROR_SOCKET_LISTEN;
    }
    DEBUG("listen success \r\n");

    while (1)
    {
        connect_fd = accept(socket_fd, (struct sockaddr *)&client_address, &address_len);
        //若有新的连接
        if (connect_fd != -1)
        {
            DEBUG("a new connect is arrived %x \r\n", connect_fd);
            int flags = FALSE;
            for (i = 0; i < SOCKET_SERVER_RCV_CONNECT_MAX; i++)
            {
                if (client_fd[i] == 0)
                {
                    flags = TRUE;
                    client_fd[i] = connect_fd;
                    break;
                }
            }

            if (flags > 0)
            {
                //增加socketfd的表
                add_socket_fd_list((void *)&connect_fd, &data_p);
                DEBUG("find_socket_fd_list addr  %p\r\n", data_p);
                if (NULL != data_p)
                {
                    msg = (struct rcv_sockt_fd_msg *)(data_p);

                    if (!getpeername(connect_fd, (struct sockaddr *)&client_address, &address_len))
                    {
                        DEBUG("c_addr is%x port is %x\r\n", client_address.sin_addr.s_addr, client_address.sin_port);
                        msg->c_addr = client_address;
                    }
                    DEBUG("c_addr is%x port is %x\r\n", msg->c_addr.sin_addr.s_addr, msg->c_addr.sin_port);

                    u32 ret = pthread_create(&thread_id, NULL, tcp_server_deal, (void *)&connect_fd);
                    if (0 != ret)
                    {
                        return ERROR_SOCKET_CREAT_PTH;
                    }
                }
                else
                {
                    DEBUG("add_socket_fd_list error \r\n");
                }
            }
        }
        else
        {
            DEBUG("connect is overflow\r\n");
        }
    }
}

/**
 * @brief tcp client 处理函数
 * 
 * @param cfd 
 * @return void* 
 */
void *tcp_client_deal(void *cfd)
{
    char buf[RCV_DATA_BUF_SIZE] = {0};
    u32 size;
    int connect_fd = 0;
    void *data_p = NULL;
    struct rcv_sockt_fd_msg *msg = NULL;

    connect_fd = *((u32 *)(cfd));
    if (SUCCESS == find_socket_fd_list((void *)&connect_fd, &data_p))
    {
        DEBUG("find_socket_fd_list addr  %p\r\n", data_p);
        msg = (struct rcv_sockt_fd_msg *)data_p;
        DEBUG("tcp_server_deal has find %d table  \r\n", connect_fd);
    }
    while (1)
    {
        memset(buf, '\0', sizeof(buf));
        size = read(connect_fd, buf, sizeof(buf));
        if (size <= 0) //没有接收到数据，关闭描述符，释放在TCPServer申请的空间
        {
            DEBUG("%x has exit \r\n", connect_fd);
            //删除客户端fd
            pthread_mutex_lock(&global_select_fd_mutex);
            if (connect_fd == global_select_fd)
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
            DEBUG("TCP_Analyzer:%s,%d %p\r\n", buf, (int)size, msg);
            write_rcv_data_stru(buf, &(msg->rcv_data)); //像buff中写入数据
        }
    }
    return SUCCESS;
}

/**
 * @brief tcp客户端初始化
 * 
 * @param addr 地址信息
 * @return u32 操作结果
 */
u32 socket_int_tcp_client(struct sockaddr_in *addr)
{
    u32 socket_fd = 0;
    u32 ret = 0;
    pthread_t thread_id = 0;
    void *data_p = NULL;

    DEBUG("socket_int_tcp_client in \r\n");
    socket_fd = socket(addr->sin_family, SOCK_STREAM, 0);
    if ((socket_fd) < 0)
    {
        printf("creat socket_fd failure \r\n");
        return ERROR_SOCKET_CREAT;
    }

    if (connect(socket_fd, (struct sockaddr *)addr, sizeof(struct sockaddr_in)) == -1)
    {
        printf("connect failure \r\n");
        return ERROR_SOCKET_CONNECT;
    }
    //增加socketfd的表
    add_socket_fd_list((void *)&socket_fd, &data_p);
    ret = pthread_create(&thread_id, NULL, tcp_client_deal, (void *)&socket_fd);
    if (0 != ret)
    {
        return ERROR_SOCKET_CREAT_PTH;
    }
    global_select_fd = socket_fd;

    while(1)
    {
        sleep(10);
    }
    return 0;
}