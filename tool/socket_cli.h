/**
 * @file socket_cli.h
 * @author zhao.wei (hw)
 * @brief 
 * @version 0.1
 * @date 2020-08-13
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef _SOCKET_CLI_H_
#define _SOCKET_CLI_H_


#define PROMPT printf("sockt_tool>>")
#define PROMPT_AMBIGUOUS printf("?Ambiguous command\r\n")

void cmd_ambiguous(void *data);
void cmd_quit(void *data);
void cmd_help(void *data);
void cmd_list(void *data);
void cmd_send(void *data);
void cmd_sendfile(void *data);
void cmd_recv(void *data);
void cmd_recv_file(void *data);

#endif