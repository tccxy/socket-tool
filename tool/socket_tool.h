/**
 * @file socket_tool.h
 * @author zhao.wei (hw)
 * @brief 头文件
 * @version 0.1
 * @date 2020-08-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef _SOCKET_TOOL_H_
#define _SOCKET_TOOL_H_

#include "pub.h"


/**
 * @brief 控制信息数据结构
 * 
 */
struct socket_tool_control
{
    u32 p_type;
    u32 w_type;
    struct sockaddr_in address;//addr msg
};




typedef void (*dealfun)( void *); //钩子

struct cmd_dealentity
{
    char *cmd;
    dealfun dealentity;
};
u32 socket_cmd_deal_tcp_server(struct socket_tool_control *control);

#endif