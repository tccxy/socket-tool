/**
 * @file socket_interface.h
 * @author zhao.wei (hw)
 * @brief 接口头文件
 * @version 0.1
 * @date 2020-08-07
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef _RCVDATA_STRUCTURE_H_
#define _RCVDATA_STRUCTURE_H_

#include "socket_interface.h"

typedef void (llist_op_t)(void *);
typedef int (llist_cmp_t)(void *, void *);

//获取字符串
#define GETLINES(string, buf)             \
    do                                    \
    {                                     \
        printf(string);                   \
        fgets(buf, sizeof(buf), stdin);   \
        if (buf[strlen(buf) - 1] == '\n') \
            buf[strlen(buf) - 1] = '\0';  \
    } while (0)

//容错宏
#define ERRP(con, func, ret)            \
    do                                  \
    {                                   \
        if (con)                        \
        {                               \
            printf(#func " failed!\n"); \
            ret;                        \
        }                               \
    } while (0)

//以双向链的方式存储fd
/**
 * @brief 双向链节点信息
 * 
 */
struct node_t
{
    void *data;
    struct node_t *next;
    struct node_t *prev;
};

/**
 * @brief fd_list抽象数据类型
 * 
 */
typedef struct llist_t
{
    struct node_t head; //数据
    int size;           //数据的大小
    int num;            //数据个数
} LLIST;

//函数声明
LLIST *llist_create(int size);
int llist_append(void *data, LLIST *handle, void **data_pos);
int llist_prevend(void *data, LLIST *handle, void **data_pos);
int llist_index_insert(int index, void *data, LLIST *handle, void **data_pos);
int llist_del(void *key, llist_cmp_t *cmp, LLIST *handle);
int llist_all_del(void *key, llist_cmp_t *cmp, LLIST *handle);
int llist_index_del(int index, LLIST *handle);
void *llist_ind(void *key, llist_cmp_t *cmp, LLIST *handle);
void *llist_index_ind(int index, LLIST *handle);
int llist_num(LLIST *handle);
void llist_store(const char *path, LLIST *handle);
LLIST *llist_load(const char *path);
void llist_travel(LLIST *handle, llist_op_t *op, int flag);
void llist_destroy(LLIST *handle);
void llist_clean(LLIST *handle);

#endif