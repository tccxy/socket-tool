/**
 * @file socket_cli_udp.c
 * @author zhao.wei (hw)
 * @brief 
 * @version 0.1
 * @date 2020-08-26
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "pub.h"
#include "socket_cli.h"
#include "socket_tool.h"

static struct sockaddr_in client_ip;
static struct sockaddr_in filter_ip;
static u8 filter_flag = FALSE;
static u8 client_flag = FALSE;

/**
 * @brief 模糊命令查询
 * 
 * @param data 
 */
void cmd_ambiguous_udp(void *data)
{
    printf("Commands may be abbreviated . Commands are:\r\n");
    printf("\r\n");
    printf("?           help    quit    \r\n");
    printf("setclient        \r\n");
    printf("setfilterip        \r\n");
    printf("setgroupip        \r\n");
    printf("send       sendfile    \r\n");
    printf("recv       recvfile    \r\n");
}

/**
 * @brief help命令
 * 
 * @param data 
 */
void cmd_help_udp(void *data)
{
    u8 help_msg[] =
        "\
    \r\n send         --->  Send msg to select fd input through the console\
    \r\n recv         --->  Recv msg from select fd andoutput to the console\
    \r\n setclient    --->  set client ip msg\
    \r\n setfilterip  --->  Set a valid receive ip ,default receive all\
    \r\n setgroupip   --->  Set group ip \
    \r\n sendfile     --->  Send the local filefile to select fd\
    \r\n recvfile     --->  Recv msg from select fd and save to file\
    ";

    printf("%s \r\n", help_msg);
}

/**
 * @brief 设置udp的组ip
 * 
 * @param data 
 */
void cmd_set_group_udp(void *data)
{
    struct socket_tool_control *control = (struct socket_tool_control *)data;
    s32 ret = 0;
    char get_ip[128] = {0};
    struct ip_mreq group = {0};

    CMD_LINE;
    CMD_GROUP_FILTERIP;
    memset(cmd_buff, 0, sizeof(cmd_buff));
    if (NULL == fgets(cmd_buff, sizeof(cmd_buff), stdin))
        return;

    while (1)
    {
        if ('q' == cmd_buff[0])
            return;
        memset(get_ip, 0, sizeof(get_ip));
        memcpy(get_ip, cmd_buff, strlen(cmd_buff) - 1);
        ret = inet_pton(control->address.sin_family, (char *)get_ip, &group.imr_multiaddr.s_addr);
        //printf("cmd_set_filter_udp %s ret %x\r\n", get_ip, ret);
        if (ret != TRUE)
            CMD_REINPUT_IP;
        else
            break;
        memset(cmd_buff, 0, sizeof(cmd_buff));
        if (NULL == fgets(cmd_buff, sizeof(cmd_buff), stdin))
            return;
    }
    group.imr_interface.s_addr = inet_addr("0.0.0.0");
    ret = setsockopt(global_select_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &group, sizeof(group));
    if (ret < 0)
    {
        printf("setgroup ip failure %x ,please Wait a moment try again .\r\n", ret);
        exit(0);
        return;
    }
}

/**
 * @brief 
 * 
 */
void cmd_set_client_udp(void *data)
{
    struct socket_tool_control *control = (struct socket_tool_control *)data;
    s32 ret = 0;
    char get_ip[128] = {0};
    u8 check_flag = FALSE;
    u8 i = 0;
    u16 port = 0;

    CMD_LINE;
    CMD_INPUT_FILTERIP;
    memset(cmd_buff, 0, sizeof(cmd_buff));
    if (NULL == fgets(cmd_buff, sizeof(cmd_buff), stdin))
        return;

    while (1)
    {
        if ('q' == cmd_buff[0])
            return;
        memset(get_ip, 0, sizeof(get_ip));
        for (i = 0; i < strlen(cmd_buff); i++)
        {
            if (cmd_buff[i] == ':')
            {
                check_flag = TRUE;
                break;
            }
        }
        if (FALSE == check_flag)
        {
            check_flag = FALSE;
            printf("please check format ip:port.\r\n");
        }
        else
        {
            port = atoi((char *)&cmd_buff[i + 1]);
            memset(get_ip, 0, sizeof(get_ip));
            memcpy(get_ip, cmd_buff, i);
            ret = inet_pton(control->address.sin_family, (char *)get_ip, &client_ip.sin_addr);
            DEBUG("cmd_set_client_udp %s ret %x\r\n", get_ip, ret);
            if (ret != TRUE)
                CMD_REINPUT_IP;
            else
                break;
        }
        memset(cmd_buff, 0, sizeof(cmd_buff));
        if (NULL == fgets(cmd_buff, sizeof(cmd_buff), stdin))
            return;
    }

    //printf("client_ip.sin_addr %x %s %d\r\n", client_ip.sin_addr, inet_ntoa(client_ip.sin_addr), port);
    client_ip.sin_port = htons(port);
    client_ip.sin_family = control->address.sin_family;
    client_flag = TRUE;
}

/**
 * @brief 
 * 
 * @param data 
 */
void cmd_set_filter_udp(void *data)
{
    struct socket_tool_control *control = (struct socket_tool_control *)data;
    void *data_p = NULL;
    s32 ret = 0;
    char get_ip[128] = {0};
    struct rcv_sockt_fd_msg *msg = NULL;

    CMD_LINE;
    CMD_INPUT_FILTERIP;
    memset(cmd_buff, 0, sizeof(cmd_buff));
    if (NULL == fgets(cmd_buff, sizeof(cmd_buff), stdin))
        return;

    while (1)
    {
        if ('q' == cmd_buff[0])
            return;
        memset(get_ip, 0, sizeof(get_ip));
        memcpy(get_ip, cmd_buff, strlen(cmd_buff) - 1);
        ret = inet_pton(control->address.sin_family, (char *)get_ip, &filter_ip.sin_addr);
        //printf("cmd_set_filter_udp %s ret %x\r\n", get_ip, ret);
        if (ret != TRUE)
            CMD_REINPUT_IP;
        else
            break;
        memset(cmd_buff, 0, sizeof(cmd_buff));
        if (NULL == fgets(cmd_buff, sizeof(cmd_buff), stdin))
            return;
    }
    inet_aton((char *)get_ip, &filter_ip.sin_addr);
    if (SUCCESS == find_socket_fd_list((void *)&global_select_fd, &data_p))
    {
        msg = (struct rcv_sockt_fd_msg *)data_p;
        msg->pad = TRUE;
        msg->c_addr.sin_addr = filter_ip.sin_addr;
    }
    DEBUG("filter_ip.sin_addr %x %s \r\n", filter_ip.sin_addr, inet_ntoa(filter_ip.sin_addr));
    filter_flag = TRUE;
}

/**
 * @brief send命令
 * 
 * @paramdata 
 */
void cmd_send_udp(void *data)
{
    u32 cmd_data = 0;
    s32 semid;
    struct sembuf p_buf;

    p_buf.sem_num = 0;
    p_buf.sem_op = -1; //信号量减1，注意这一行的1前面有个负号
    p_buf.sem_flg = SEM_UNDO;

    if (FALSE == client_flag)
    {
        printf("please input client msg .");
        return;
    }
    if ((semid = semget(send_key, 1, IPC_CREAT | 0666)) == -1)
    {
        printf("send msg semget error .");
        exit(0);
    }

    cmd_get_key_async(&cmd_data); //异步获取键值

    while (1)
    {
        //sleep(2);
        semop(semid, &p_buf, 1);

        pthread_mutex_lock(&global_select_fd_mutex);
        if ((global_select_fd == 0x3f) || (cmd_data == CMD_ESC))
        {
            pthread_mutex_unlock(&global_select_fd_mutex);
            printf("  \r\n");
            fflush(stdout);
            break;
        }
        pthread_mutex_unlock(&global_select_fd_mutex);

        //printf("client_ip.sin_addr %x %s %d msg %d\r\n", client_ip.sin_addr,
        //     inet_ntoa(client_ip.sin_addr), client_ip.sin_port, send_msg.send_len);

        pthread_mutex_lock(&send_msg_mutex);

        if (send_msg.send_len != sendto(global_select_fd, send_msg.send_buf,
                                        send_msg.send_len, 0, (struct sockaddr *)&client_ip, sizeof(client_ip)))
        {
            printf("send error .may be the connect has exit \r\n");
            fflush(stdout);
            pthread_mutex_unlock(&send_msg_mutex);
            break;
        }
        else
        {
            send_msg.send_len = 0;
        }
        pthread_mutex_unlock(&send_msg_mutex);
    }
}

/**
 * @brief cmd提示符
 * 
 */
void cmd_promat_udp(void *data)
{
    if (TRUE == filter_flag && FALSE == client_flag)
        PROMPT_F_UDP;
    else if (FALSE == filter_flag && TRUE == client_flag)
        PROMPT_C_UDP;
    else if (TRUE == filter_flag && TRUE == client_flag)
        PROMPT_FC_UDP;
    else
        PROMPT_UDP;
}