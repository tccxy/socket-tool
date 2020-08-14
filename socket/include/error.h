/**
 * @file error.h
 * @author zhao.wei (hw)
 * @brief 
 * @version 0.1
 * @date 2020-08-11
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef _ERROR_H_
#define _ERROR_H_

#define SUCCESS 0
#define ERROR 1

#define FALSE 0
#define TRUE 1

#define ERROR_BASE 0xffff0001

#define ERROR_HANDLE_CREAT (ERROR_BASE + 1)
#define ERROR_HANDLE_ADD (ERROR_BASE + 2)
#define ERROR_HANDLE_DEL (ERROR_BASE + 3)
#define ERROR_SINGLE_RCV_LEN (ERROR_BASE + 4)
#define ERROR_SOCKET_CREAT (ERROR_BASE + 5)
#define ERROR_SOCKET_BIND (ERROR_BASE + 6)
#define ERROR_SOCKET_LISTEN (ERROR_BASE + 7)
#define ERROR_SOCKET_CREAT_PTH (ERROR_BASE + 8)
#define ERROR_SOCKET_CLINT_FD (ERROR_BASE + 9)

#endif