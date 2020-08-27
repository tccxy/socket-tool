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

extern char cmd_buff[128];
extern struct send_data_buf send_msg;  //发送buf
extern pthread_mutex_t send_msg_mutex; //发送数据锁
extern key_t send_key;


#define CMD_ESC 0x1b
#define CMD_ENTER 0x0a
#define PROMPT_TCP printf("sockt_tool @%c >>", global_select_fd)
#define PROMPT_FD_TCP printf("sockt_tool @%x >>", global_select_fd)
#define PROMPT_UDP printf("sockt_tool @%x @all >>", global_select_fd)
#define PROMPT_F_UDP printf("sockt_tool @%x F@%s >>", global_select_fd, inet_ntoa(filter_ip.sin_addr))
#define PROMPT_C_UDP printf("sockt_tool @%x @all C@%s >>", global_select_fd, inet_ntoa(client_ip.sin_addr))
#define PROMPT_FC_UDP printf("sockt_tool @%x F@%s C@%s >>", global_select_fd, inet_ntoa(filter_ip.sin_addr), inet_ntoa(client_ip.sin_addr))
#define PROMPT_AMBIGUOUS printf("?Ambiguous command\r\n")
#define CMD_LINE printf("\r\n")
#define CMD_INPUT_FD printf("please input socket_fd to select 'q' is back\r\n")
#define CMD_INPUT_FILTERIP printf("please input filter ip 'q' is back\r\n")
#define CMD_GROUP_FILTERIP printf("please input group ip 'q' is back\r\n")
#define CMD_REINPUT_FD printf("input_fd is not found,please reinput! 'q' is back\r\n")
#define CMD_REINPUT_IP printf("input_ip is error ,please reinput! 'q' is back\r\n")
#define CMD_NO_CONNECT printf("there is no socket connect .\r\n")

void cmd_get_key_async(void *data);
void cmd_ambiguous_tcp(void *data);
void cmd_ambiguous_udp(void *data);
void cmd_quit(void *data);
void cmd_help_tcp(void *data);
void cmd_help_udp(void *data);
void cmd_list_tcp(void *data);
void cmd_send_tcp(void *data);
void cmd_send_udp(void *data);
void cmd_sendfile(void *data);
void cmd_recv(void *data);
void cmd_recv_file(void *data);
void cmd_set_group_udp(void *data);
void cmd_set_client_udp(void *data);
void cmd_set_filter_udp(void *data);
void cmd_promat_tcp(void *data);
void cmd_promat_udp(void *data);

#endif