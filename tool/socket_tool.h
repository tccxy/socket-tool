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

#include "types.h"
#include "pub.h"

#define SOCKET_TCP 1
#define SOCKET_UDP 2

#define SOCKET_SERVER 1
#define SOCKET_CLIENT 2

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


#endif