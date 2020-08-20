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

u32 global_select_fd = 0x3f;                                        //?的ascall码
pthread_mutex_t global_select_fd_mutex = PTHREAD_MUTEX_INITIALIZER; //select_fd 锁

static char cmd_buff[128] = {0};

static struct send_data_buf send_msg = {0};                        //发送buf
static pthread_mutex_t send_msg_mutex = PTHREAD_MUTEX_INITIALIZER; //发送数据锁

static struct cmd_dealentity cmd_table_tcp_server[] = {
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

static struct cmd_dealentity cmd_table_tcp_client[] = {
    {"?", cmd_ambiguous},
    {"help", cmd_help},
    {"quit", cmd_quit},
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
    struct socket_tool_control *control = (struct socket_tool_control *)data;

    printf("Commands may be abbreviated . Commands are:\r\n");
    printf("\r\n");
    printf("?       help    quit    \r\n");
    if (SOCKET_SERVER == control->w_type)
        printf("list        \r\n");
    printf("send    sendfile    \r\n");
    printf("recv    recvfile    \r\n");
}

void *get_key_async(void *data)
{
    u32 get_num = 0;
    struct termios stored_settings;
    struct termios new_settings;
    tcgetattr(0, &stored_settings);
    new_settings = stored_settings;
    new_settings.c_lflag &= (~ICANON);
    new_settings.c_cc[VTIME] = 0;
    new_settings.c_cc[VMIN] = 1;
    tcsetattr(0, TCSANOW, &new_settings);
    while (1)
    {
        *(u32 *)data = getchar();

        if ((*(u32 *)data == CMD_ESC) || (global_select_fd == 0x3f))
        {
            break;
        }

        pthread_mutex_lock(&send_msg_mutex);
        send_msg.send_buf[get_num] = *(u8 *)data;
        get_num++;
        if ((*(u32 *)data == CMD_ENTER) || (get_num >= SEND_DATA_BUF_SIZE))
        {
            send_msg.send_len = get_num;
            if (get_num >= SEND_DATA_BUF_SIZE)
                *(u32 *)data = CMD_ENTER;
            get_num = 0;
        }
        pthread_mutex_unlock(&send_msg_mutex);
    }

    if (*(u32 *)data == CMD_ESC)
    {
        //printf("\033[2D");
        putchar('\b'); // 删除回显
        putchar('\b'); // 删除回显
        //fflush(stdout);
        //printf("  \r\n");
    }

    //printf("input:  [%x]\n", *(u32 *)data);
    tcsetattr(0, TCSANOW, &stored_settings); // 恢复终端参数
    return SUCCESS;
}

/**
 * @brief 异步获取键值
 * 
 */
void cmd_get_key_async(void *data)
{
    pthread_t thread_id = 0;

    pthread_create(&thread_id, NULL, get_key_async, data);
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
    u8 help_msg[] =
        "\
    \r\n list     --->  List all linked sockets fd and client msg(tcp server-is-valid) \
    \r\n send     --->  Send msg to select fd input through the console\
    \r\n recv     --->  Recv msg from select fd andoutput to the console\
    \r\n sendfile --->  Send the local filefile to select fd\
    \r\n recvfile --->  Recv msg from select fd and save to file\
    ";

    printf("%s \r\n", help_msg);
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
        if (NULL == fgets(cmd_buff, sizeof(cmd_buff), stdin))
            return;
        input_fd = atoi(cmd_buff);
        DEBUG("input socket_fd %x %d\r\n", input_fd,
              find_socket_fd_list((void *)&input_fd, &data_p));

        while (1)
        {
            if (SUCCESS == find_socket_fd_list((void *)&input_fd, &data_p))
                break;
            if ('q' == cmd_buff[0])
                return;
            CMD_REINPUT_FD;
            memset(cmd_buff, 0, sizeof(cmd_buff));
            if (NULL == fgets(cmd_buff, sizeof(cmd_buff), stdin))
                return;
            input_fd = atoi(cmd_buff);
        }
        pthread_mutex_lock(&global_select_fd_mutex);
        global_select_fd = input_fd;
        pthread_mutex_unlock(&global_select_fd_mutex);
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
    u32 cmd_data = 0;

    cmd_get_key_async(&cmd_data); //异步获取键值

    while (1)
    {
        //sleep(2);
        pthread_mutex_lock(&global_select_fd_mutex);
        if ((global_select_fd == 0x3f) || (cmd_data == CMD_ESC))
        {
            pthread_mutex_unlock(&global_select_fd_mutex);
            printf("  \r\n");
            fflush(stdout);
            break;
        }
        pthread_mutex_unlock(&global_select_fd_mutex);
        //printf("cmd_data %x global_select_fd %x send_msg.send_len %d\r\n",
        //      cmd_data, global_select_fd, send_msg.send_len);

        pthread_mutex_lock(&send_msg_mutex);

        if (send_msg.send_len != send(global_select_fd, send_msg.send_buf,
                                      send_msg.send_len, 0))
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
    void *data_p = NULL;
    struct rcv_sockt_fd_msg *msg = NULL;
    struct rcv_data_structure *rcv_data = NULL;
    u32 cmd_data = 0;
    char buf[RCV_DATA_BUF_SIZE] = {0};

    cmd_get_key_async(&cmd_data); //异步获取键值

    pthread_mutex_lock(&global_select_fd_mutex);
    if (SUCCESS == find_socket_fd_list((void *)&global_select_fd, &data_p))
    {
        DEBUG("find_socket_fd_list addr  %p\r\n", data_p);
        msg = (struct rcv_sockt_fd_msg *)data_p;
        DEBUG("tcp_server_deal has find %d table  \r\n", global_select_fd);
    }
    pthread_mutex_unlock(&global_select_fd_mutex);
    rcv_data = &(msg->rcv_data);

    while (1)
    {
        //sleep(2);
        pthread_mutex_lock(&global_select_fd_mutex);
        if ((global_select_fd == 0x3f) || (cmd_data == 0x1b))
        {
            pthread_mutex_unlock(&global_select_fd_mutex);
            printf("  \r\n");
            fflush(stdout);
            break;
        }
        pthread_mutex_unlock(&global_select_fd_mutex);

        // DEBUG("cmd_data %x global_select_fd %x\r\n", cmd_data, global_select_fd);
        if (0 != read_rcv_data_stru(buf, rcv_data))
        {
            printf("%s", buf);
            fflush(stdout);
        }
    }
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
u32 socket_cmd_deal_tcp(struct socket_tool_control *control)
{
    struct cmd_dealentity *entry = NULL;
    struct cmd_dealentity *entry_start = NULL;

    if (SOCKET_SERVER == control->w_type)
        entry_start = cmd_table_tcp_server;
    else
        entry_start = cmd_table_tcp_client;

    while (1)
    {
        u8 match_flag = FALSE;
        //客户端状态下没有连接成功直接退出
        if(SOCKET_CLIENT == control->w_type && global_select_fd == 0x3f)
        {
            printf("server offline .");
            break;
        }
    

        cmd_promat();
        fflush(stdout);
        memset(cmd_buff, 0, sizeof(cmd_buff));
        if (NULL == fgets(cmd_buff, sizeof(cmd_buff), stdin))
            break;

        for (entry = entry_start; entry->cmd != NULL; entry++)
        {
            //printf("entry->cmd is %s %d %d \r\n", entry->cmd,strlen(cmd_buff),strlen(entry->cmd));
            if (SUCCESS == strncmp(cmd_buff, entry->cmd, strlen(entry->cmd)))
            {
                //DEBUG("match cmd is  %s \r\n", cmd_buff);
                match_flag = TRUE;
                entry->dealentity(control);
            }
        }
        if (FALSE == match_flag)
            PROMPT_AMBIGUOUS;
    }
    return 0;
}
