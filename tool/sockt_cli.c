/**
 * @file sockt_cli.c
 * @author zhao.wei (hw)
 * @brief
 * @version 0.1
 * @date 2020-08-12
 *
 * @copyright Copyright (c) 2020
 *
 */

#include "pub.h"
#include "socket_tool.h"
#include "socket_cli.h"
#include "socket_interface.h"

char cmd_buff[128] = {0};
u32 global_select_fd = 0x3f; //?的ascall码

struct cmd_dealentity cmd_table[] = {
    {"?", cmd_ambiguous},
    {"help", cmd_help},
    {"quit", cmd_quit},
    {"list", cmd_list},
    {"send", cmd_send},
    {"sendfile", cmd_sendfile},
    {"recv", cmd_recv},
    {"recvfile", cmd_recv_file},
    {NULL, NULL},
};

/**
 * @brief 模糊命令查询
 * 
 * @param data 
 */
void cmd_ambiguous(void *data)
{
    printf("Commands may be abbreviated . Commands are:\r\n");
    printf("\r\n");
    printf("?       help        \r\n");
    printf("quit    list        \r\n");
    printf("send    sendfile    \r\n");
    printf("recv    recvfile    \r\n");
}

/**
 * @brief 退出命令
 * 
 * @param data 
 */
void cmd_quit(void *data)
{
    exit(0);
}
/**
 * @brief help命令
 * 
 * @param data 
 */
void cmd_help(void *data)
{
}

/**
 * @brief list命令
 * 
 * @pram data 
 */
void cmd_list(void *data)
{
    u32 list_num = 0;
    u32 input_fd = 0;
    void *data_p = NULL;

    list_num = get_socket_fd_list_num();
    DEBUG("list is in %d list\r\n", list_num);
    if (list_num != 0)
    {
        //遍历
        socket_fd_list_travel();
        CMD_LINE;
        CMD_INPUT_FD;
        memset(cmd_buff, 0, sizeof(cmd_buff));
        fgets(cmd_buff, sizeof(cmd_buff), stdin);
        input_fd = atoi(cmd_buff);
        DEBUG("input socket_fd %x %d\r\n", input_fd,find_socket_fd_list((void *)&input_fd, &data_p));

        while (SUCCESS != find_socket_fd_list((void *)&input_fd, &data_p))
        {
            CMD_REINPUT_FD;
            memset(cmd_buff, 0, sizeof(cmd_buff));
            fgets(cmd_buff, sizeof(cmd_buff), stdin);
            input_fd = atoi(cmd_buff);
        }
        global_select_fd = input_fd;
    }
    else
    {
        CMD_NO_CONNECT;
    }
}

/**
 * @brief send命令
 * 
 * @paramdata 
 */
void cmd_send(void *data)
{
}

/**
 * @brief sendfile命令
 * 
 * @param dat 
 */
void cmd_sendfile(void *data)
{
}

/**
 * @brief recv命令
 * 
 * @param data 
*/
void cmd_recv(void *data)
{
}

/**
 * @brief recvfile命令
 * 
 * @param data 
 */
void cmd_recv_file(void *data)
{
}

/**
 * @brief cmd提示符
 * 
 */
void cmd_promat(void)
{
    if (global_select_fd != 0x3f)
        PROMPT_FD;
    else
        PROMPT;
}

/**
 * @brief 命令行处理界面
 * 
 * @param work_type 
 * @return u3 
 */
u32 socket_cmd_deal(struct socket_tool_control *control)
{
    while (1)
    {
        u8 match_flag = FALSE;
        cmd_promat();
        memset(cmd_buff, 0, sizeof(cmd_buff));
        fgets(cmd_buff, sizeof(cmd_buff), stdin);
        //DEBUG("cmd_buff is %s \r\n", cmd_buff);

        for (int i = 0; i < sizeof(cmd_table) / sizeof(struct cmd_dealentity); i++)
        {
            if ((NULL != cmd_table[i].cmd) && (SUCCESS == strncmp(cmd_buff, cmd_table[i].cmd, strlen(cmd_table[i].cmd))))
            {
                //DEBUG("match cmd is  %s \r\n", cmd_buff);
                match_flag = TRUE;
                cmd_table[i].dealentity(control);
            }
        }
        if (FALSE == match_flag)
            PROMPT_AMBIGUOUS;
    }
    return 0;
}
