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

struct sockaddr_in client_ip;
struct sockaddr_in filter_ip;
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
    \r\n setclient    --->  set client ip msg (only server-is-valid)\
    \r\n setfilterip  --->  Set a valid receive ip ,default receive all (only server-is-valid) \
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
}

/**
 * @brief 
 * 
 */
void cmd_set_client_udp(void *data)
{

}

/**
 * @brief 
 * 
 * @param data 
 */
void cmd_set_filter_udp(void *data)
{

}

/**
 * @brief cmd提示符
 * 
 */
void cmd_promat_udp(void)
{
    if (global_select_fd != 0x3f)
        PROMPT_UDP;
    else
        PROMPT_TCP;
}