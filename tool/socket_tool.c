/**
 * @file socket_tool.c
 * @author zhao.wei (hw)
 * @brief socket_tool主程序
 * @version 0.1
 * @date 2020-08-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "socket_tool.h"
#include "socket_interface.h"

struct socket_tool_control g_socket_tool_control = {0};

static u8 help[] =
    "\
    \r\n Usage   : \
    \r\n    socket_tool [options] <p_type> [d_type] [w_type] -i <ipaddr> -p <port>\
    \r\n  options\
    \r\n     -h,--help                          get app help\
    \r\n     -P,--protocal                      set work protocal\
    \r\n     -r,--reuse                         enable port reuse\
    \r\n  p_type\
    \r\n        <UDP | TCP>\
    \r\n  d_type\
    \r\n        -4,--IPv4                       IPv4\
    \r\n        -6,--IPv6                       IPv6\
    \r\n  w_type\
    \r\n        -S,--server                     will work to server\
    \r\n        -C,--client                     will work to client\
    \r\n  -i,--ip                               local or remote ip address\
    \r\n  -p,--port                             local or remote port\
    \r\n  \
    \r\n  --Notice                              \
    \r\n         Tcp can work S/C mode ,S mode ip:port is local\
    \r\n                                C mode ip:port is remote\
    \r\n         Udp the parameter w_type is ignored\
    \r\n                                         ip:port is local\
    ";

static u8 exit_prese_msg[] =
    "\
    \r\n Usage   : \
    \r\n    socket_tool [options] <p_type> [d_type] [w_type] -i <ipaddr> -p <port>\
    \r\n Try `socket_tool -h,--help' for more information.\
    ";

/**
 * 
 * @brief 打印帮助信息
 * 
 */
void printf_help_usage()
{
    printf("socket_tool%d.%d \r\n %s \r\n", MAJOR_VER, MINOR_VER, help);
    exit(1);
}

static struct option long_options[] =
    {
        {"help", no_argument, NULL, 'h'},
        {"protocol", required_argument, NULL, 'P'},
        {"server", no_argument, NULL, 'S'},
        {"client", no_argument, NULL, 'C'},
        {"ip", required_argument, NULL, 'i'},
        {"port", required_argument, NULL, 'p'},
        {"IPv4", no_argument, NULL, '4'},
        {"IPv6", no_argument, NULL, '6'},
        {NULL, 0, NULL, 0},
};

/**
 * @brief 异常退出消息提示
 * 
 */
void exit_usage()
{
    printf("socket_tool%d.%d \r\n %s \r\n", MAJOR_VER, MINOR_VER, exit_prese_msg);
    exit(1);
}

/**
 * @brief socket_tool的命令参数解析
 * 
 * @param opt 
 * @param optarg 
 * @param argv 
 */
static void socket_tool_cmd_parse(u32 opt, u8 *optarg, u8 *argv)
{
    if ('P' == opt)
    {
        if (SUCCESS == strcmp((char *)optarg, "UDP"))
        {
            g_socket_tool_control.p_type = SOCK_DGRAM;
        }
        else if (SUCCESS == strcmp((char *)optarg, "TCP"))
        {
            g_socket_tool_control.p_type = SOCK_STREAM;
        }
        else
        {
            printf("the parameter -P is error please input UDP/TCP \r\n");
            exit(0);
        }
    }
    if ('4' == opt)
        g_socket_tool_control.address.sin_family = AF_INET;
    if ('6' == opt)
        g_socket_tool_control.address.sin_family = AF_INET6;
    if ('S' == opt)
        g_socket_tool_control.w_type = SOCKET_SERVER;
    if ('C' == opt)
        g_socket_tool_control.w_type = SOCKET_CLIENT;
    if ('i' == opt)
    {
        DEBUG("ip is %s \r\n", optarg);

        //if (inet_aton((char *)optarg, &g_socket_tool_control.address.sin_addr) < 0)
        if (inet_pton(g_socket_tool_control.address.sin_family, (char *)optarg,
                      &g_socket_tool_control.address.sin_addr) != TRUE)
            printf("the parameter -i is error please check \r\n");
    }
    if ('p' == opt)
    {
        DEBUG("port is %d \r\n", atoi((char *)optarg));
        g_socket_tool_control.address.sin_port = htons(atoi((char *)optarg));
    }
}

/**
 * @brief socket初始化程序
 * 
 * @param control 控制信息数据结构
 * 
 */
void *socket_init(void *data)
{
    struct socket_tool_control *control = (struct socket_tool_control *)data;
    DEBUG("socket_init in p_type %d w_type %d  \r\n", control->p_type, control->w_type);

    if (SOCKET_TCP == control->p_type)
    {
        if (SOCKET_SERVER == control->w_type)
            socket_int_tcp_server(&control->address);
        else
            socket_int_tcp_client(&control->address);
    }
    if (SOCKET_UDP == control->p_type)
    {
        socket_init_udp(&control->address);
    }
    DEBUG("socket_init out \r\n");
    return SUCCESS;
}

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char *argv[])
{
    u32 opt;
    u32 option_index = 0;
    char *string = "P:46SCi:p:";
    pthread_t thread_id = 0;

    while ((opt = getopt_long_only(argc, argv, string, long_options, &option_index)) != -1)
    {
        //printf("opt = %c\t\t", opt);
        //printf("optarg = %s\t\t", optarg);
        //printf("optind = %d\t\t", optind);
        //printf("argv[optind] =%s\t\t", argv[optind]);
        //printf("option_index = %d\n", option_index);
        switch (opt)
        {
        case 'P':
        case '4':
        case '6':
        case 'S':
        case 'C':
        case 'i':
        case 'p':
            socket_tool_cmd_parse(opt, optarg, argv[optind]);
            break;
        case 'h':
            printf_help_usage();
            break;
        default:
            exit_usage();
            break;
        }
    }
    creat_socket_fd_list();

    u32 ret = pthread_create(&thread_id, NULL, socket_init, (void *)&g_socket_tool_control);
    if (0 != ret)
    {
        return ERROR_SOCKET_CREAT_PTH;
    }
    DEBUG("creat socket success\r\n");
    sleep(1);

    socket_cmd_deal(&g_socket_tool_control);
}
