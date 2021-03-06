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

char cmd_buff[128] = {0};
struct send_data_buf send_msg = {0};                        //发送buf
pthread_mutex_t send_msg_mutex = PTHREAD_MUTEX_INITIALIZER; //发送数据锁
key_t send_key = 0x200;

static struct cmd_dealentity cmd_table_tcp_server[] = {
    {"?", cmd_ambiguous_tcp},
    {"help", cmd_help_tcp},
    {"quit", cmd_quit},
    {"list", cmd_list_tcp},
    {"send", cmd_send_tcp},
    {"sendfile", cmd_sendfile},
    {"recv", cmd_recv},
    {"recvfile", cmd_recv_file},
    {NULL, NULL},
};

static struct cmd_dealentity cmd_table_tcp_client[] = {
    {"?", cmd_ambiguous_tcp},
    {"help", cmd_help_tcp},
    {"quit", cmd_quit},
    {"send", cmd_send_tcp},
    {"sendfile", cmd_sendfile},
    {"recv", cmd_recv},
    {"recvfile", cmd_recv_file},
    {NULL, NULL},
};

static struct cmd_dealentity cmd_table_udp_server[] = {
    {"?", cmd_ambiguous_udp},
    {"help", cmd_help_udp},
    {"quit", cmd_quit},
    {"setclient", cmd_set_client_udp},
    {"setgroupip", cmd_set_group_udp},
    {"setfilterip", cmd_set_filter_udp},
    {"send", cmd_send_udp},
    {"sendfile", cmd_sendfile},
    {"recv", cmd_recv},
    {"recvfile", cmd_recv_file},
    {NULL, NULL},
};


void *get_key_async(void *data)
{
    u32 get_num = 0;
    s32 semid;
    struct termios stored_settings;
    struct termios new_settings;
    struct sembuf v_buf;

    v_buf.sem_num = 0;
    v_buf.sem_op = 1;         //信号量加1
    v_buf.sem_flg = SEM_UNDO; //阻塞

    tcgetattr(0, &stored_settings);
    new_settings = stored_settings;
    new_settings.c_lflag &= (~ICANON);
    new_settings.c_cc[VTIME] = 0;
    new_settings.c_cc[VMIN] = 1;
    tcsetattr(0, TCSANOW, &new_settings);
    if ((semid = semget(send_key, 1, IPC_CREAT | 0666)) == -1)
    {
        printf("send msg semget error .");
        exit(0);
    }

    while (1)
    {
        *(u32 *)data = getchar();

        if ((*(u32 *)data == CMD_ESC) || (global_select_fd == 0x3f))
        {
            semop(semid, &v_buf, 1); //信号量+1
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
            semop(semid, &v_buf, 1); //信号量+1
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
 * @brief 命令行处理界面
 * 
 * @param work_type 
 * @return u3 
 */
u32 socket_cmd_deal(struct socket_tool_control *control)
{
    struct cmd_dealentity *entry = NULL;
    struct cmd_dealentity *entry_start = NULL;
    s32 semid;

    if (SOCKET_TCP == control->p_type)
    {
        if (SOCKET_SERVER == control->w_type)
            entry_start = cmd_table_tcp_server;
        else
            entry_start = cmd_table_tcp_client;
    }
    if (SOCKET_UDP == control->p_type)
    {
        entry_start = cmd_table_udp_server;
    }
    if ((semid = semget(send_key, 1, IPC_CREAT | 0666)) == -1)
    {
        printf("send msg semget error .");
        exit(0);
    }
    else
    {
        semun arg;
        arg.val = 1;                   //信号量的初值
        semctl(semid, 0, SETVAL, arg); //设置信号量集中的一个单独的信号量的值
    }
    while (1)
    {
        u8 match_flag = FALSE;
        //客户端状态下没有连接成功直接退出
        if (SOCKET_CLIENT == control->w_type && global_select_fd == 0x3f)
        {
            printf("server offline .\r\n");
            break;
        }
        if (SOCKET_TCP == control->p_type)
            cmd_promat_tcp(control);
        if (SOCKET_UDP == control->p_type)
            cmd_promat_udp(control);
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
