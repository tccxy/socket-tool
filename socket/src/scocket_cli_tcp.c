/**
 * @file cocket_cli_tcp.c
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

/**
 * @brief 模糊命令查询
 * 
 * @param data 
 */
void cmd_ambiguous_tcp(void *data)
{
    printf("Commands may be abbreviated . Commands are:\r\n");
    printf("\r\n");
    printf("?            \r\n");
    printf("help         \r\n");
    printf("quit         \r\n");
    printf("list         \r\n");
    printf("send         \r\n");
    printf("recv         \r\n");
    printf("sendfile     \r\n");
    printf("recvfile     \r\n");
}

/**
 * @brief help命令
 * 
 * @param data 
 */

void cmd_help_tcp(void *data)
{
    u8 help_msg[] =
        "\
    \r\n help     --->  Display this msg\
    \r\n quit     --->  Quit this app\
    \r\n list     --->  List all linked sockets fd and client msg(only server-is-valid) \
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
void cmd_list_tcp(void *data)
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
void cmd_send_tcp(void *data)
{
    u32 cmd_data = 0;
    s32 semid;
    struct sembuf p_buf;

    p_buf.sem_num = 0;
    p_buf.sem_op = -1; //信号量减1，注意这一行的1前面有个负号
    p_buf.sem_flg = SEM_UNDO;

    cmd_get_key_async(&cmd_data); //异步获取键值
    if ((semid = semget(send_key, 1, IPC_CREAT | 0666)) == -1)
    {
        printf("send msg semget error .");
        exit(0);
    }
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
 * @brief cmd提示符
 * 
 */
void cmd_promat_tcp(void *data)
{
    if (global_select_fd != 0x3f)
        PROMPT_FD_TCP;
    else
        PROMPT_TCP;
}