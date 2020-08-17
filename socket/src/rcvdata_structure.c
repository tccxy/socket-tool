/**
 * @file rcvdata_structure.c
 * @author zhao.wei (hw)
 * @brief 接受的数据结构
 * @version 0.1
 * @date 2020-08-07
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "pub.h"


/**
 * @brief 初始化
 * 
 * @param size sizeof (rcv_sockt_fd_msg)每个节点数据的大小
 * @return LLIST* handle节点
 */
LLIST *llist_create(int size)
{
   LLIST *handle = NULL;

   handle = (LLIST *)malloc(sizeof(LLIST));
   ERRP(NULL == handle, malloc handle, goto ERR1);

   handle->head.data = NULL;
   handle->head.next = &handle->head;
   handle->head.prev = &handle->head; //循环双向链表
   handle->size = size;               //struct rcv_sockt_fd_msg
   handle->num = 0;

   return handle;
ERR1:
   return NULL;
}

/**
 * @brief 尾插
 * 
 * @param data 数据
 * @param handle 操作节点
 * @param data_pos 数据的真实存储指针
 * @return int 
 */
int llist_append(void *data, LLIST *handle, void **data_pos)
{
   struct node_t *new = NULL;

   new = (struct node_t *)malloc(sizeof(struct node_t));
   ERRP(NULL == new, malloc new, goto ERR1);

   new->data = (void *)malloc(handle->size);
   ERRP(new->data == NULL, malloc new->data, goto ERR2);

   memcpy(new->data, data, handle->size);
   //用户传递数据保存到节点信息中
   //====================
   //把new节点信息插入到链表handle->head中

   new->next = &handle->head;
   new->prev = handle->head.prev;
   handle->head.prev->next = new;
   handle->head.prev = new;

   handle->num++;
   //DEBUG("llist_append %p \r\n",new->data);
   *data_pos = new->data;
   //DEBUG("llist_append %p \r\n",*data_pos);
   return 0;
ERR2:
   free(new);
ERR1:
   return -1;
}

/**
 * @brief 头插
 * 
 * @param data 数据额
 * @param handle 操作节点
 * @param data_pos 数据的真实存储指针
 * @return int 
 */
int llist_prevend(void *data, LLIST *handle, void **data_pos)
{
   //1 接受用户数据data -> int char struct => data 要保存到链表节点信息中。
   //2 定义一个链表节点，用来保存数据 new
   //3 申请空间 new
   //4 new->data
   //5 数据保存到new->data
   //6 新的节点连接到head(handle)

   struct node_t *new = NULL;

   ERRP(data == NULL, llist_prevend option, goto ERR1);

   new = (struct node_t *)malloc(sizeof(struct node_t));
   ERRP(NULL == new, malloc new, goto ERR1);

   new->data = (void *)malloc(handle->size);
   ERRP(new->data == NULL, malloc new->data, goto ERR2);

   memcpy(new->data, data, handle->size);

   new->next = handle->head.next;
   new->prev = &handle->head;
   handle->head.next->prev = new;
   handle->head.next = new;
   handle->num++;
   *data_pos = new->data;
   return 0;
ERR2:
   free(new);
ERR1:
   return -1;
}

/**
 * @brief 指定位置插入
 * 
 * @param index 索引
 * @param data 数据
 * @param handle 操作节点
 * @param data_pos 数据的真实存储指针
 * @return int 
 */
int llist_index_insert(int index, void *data, LLIST *handle, void **data_pos)
{
   struct node_t *tail = NULL;

   ERRP(index < 0 || index > handle->num, llist_index_insert option_1, goto ERR1);

   ERRP(data == NULL, llist_index_insert option_2, goto ERR1);

   struct node_t *new = NULL;

   new = (struct node_t *)malloc(sizeof(struct node_t));
   ERRP(NULL == new, malloc new, goto ERR1);

   new->data = (void *)malloc(handle->size);
   ERRP(new->data == NULL, malloc new->data, goto ERR2);

   memcpy(new->data, data, handle->size);

   for (tail = &handle->head; tail->next != &handle->head && index--; tail = tail->next)
      ;

   //新的节点添加到tail后面
   new->next = tail->next;
   new->prev = tail;
   tail->next->prev = new;
   tail->next = new;
   handle->num++;
   *data_pos = new->data;
   return 0;
ERR2:
   free(new);
ERR1:
   return -1;
}

/**
 * @brief 按照比较规则删除第一个满足的节点
 * 
 * @param key 键值
 * @param cmp 比较规则函数钩子
 * @param handle 操作节点
 * @return int 
 */
int llist_del(void *key, llist_cmp_t *cmp, LLIST *handle)
{
   struct node_t *tail = NULL;

   for (tail = handle->head.next; tail != &handle->head; tail = tail->next)
   {
      if (!cmp(key, tail->data))
      {
         tail->next->prev = tail->prev;
         tail->prev->next = tail->next;
         free(tail->data);
         free(tail);
         handle->num--;

         return 0;
      }
   }
   return -1;
}

/**
 * @brief 删除指定索引的节点
 * 
 * @param index 索引
 * @param handle 操作节点
 * @return int 
 */
int llist_index_del(int index, LLIST *handle)
{

   struct node_t *tail = NULL;

   ERRP(index < 0 || index >= handle->num, llist_index_insert option_1, goto ERR1);

   for (tail = handle->head.next; tail != &handle->head && index--; tail = tail->next)
      ;

   tail->next->prev = tail->prev;
   tail->prev->next = tail->next;
   free(tail->data);
   free(tail);
   handle->num--;

   return 0;
ERR1:
   return -1;
}

/**
 * @brief 按照比较规则删除所有满足的节点
 * 
 * @param key 键值
 * @param cmp 比较规则钩子
 * @param handle 操作节点
 * @return int 
 */
int llist_all_del(void *key, llist_cmp_t *cmp, LLIST *handle)
{
   struct node_t *tail = NULL;
   struct node_t *save = NULL;

   for (tail = handle->head.next; tail != &handle->head; tail = save)
   {
      save = tail->next;
      if (!cmp(key, tail->data))
      {
         //tail
         tail->next->prev = tail->prev;
         tail->prev->next = tail->next;
         free(tail->data);
         free(tail);
         handle->num--;
      }
   }
   return 0;
}

/**
 * @brief 按关键字查找符合比较规则的第一个数据
 * 
 * @param key 键值
 * @param cmp 比较规则钩子
 * @param handle 操作节点
 * @return void* 
 */
void *llist_ind(void *key, llist_cmp_t *cmp, LLIST *handle)
{
   struct node_t *tail = NULL;

   for (tail = handle->head.next; tail != &handle->head; tail = tail->next)
   {
      if (!cmp(key, tail->data))
      {
         return tail->data;
      }
   }

   return NULL;
}

/**
 * @brief 按指定位置返回数据
 * 
 * @param index 位置索引
 * @param handle 操作节点
 * @return void* 
 */
void *llist_index_ind(int index, LLIST *handle)
{
   struct node_t *tail = NULL;

   ERRP(index < 0 || index >= handle->num, llist_index_insert option_1, goto ERR1);

   for (tail = handle->head.next; tail != &handle->head && index--; tail = tail->next)
      ;
   return tail->data;
ERR1:
   return NULL;
}

/**
 * @brief 返回list的数据总数
 * 
 * @param handle 操作节点
 * @return int 
 */
int llist_num(LLIST *handle)
{
   return handle->num;
}

/**
 * @brief 存储
 * 
 * @param path 路径
 * @param handle 操作节点
 */
void llist_store(const char *path, LLIST *handle)
{
   FILE *fp = NULL;
   int ret;
   struct node_t *tail = NULL;

   fp = fopen(path, "w");
   ERRP(NULL == fp, fopen, goto ERR1);

   //保存数据类型
   ret = fwrite(&handle->size, sizeof(handle->size), 1, fp);
   ERRP(ret != 1, fwrite size, goto ERR2);

   //保存数据大小
   ret = fwrite(&handle->num, sizeof(handle->num), 1, fp);
   ERRP(ret != 1, fwrite num, goto ERR2);

   for (tail = handle->head.next; tail != &handle->head; tail = tail->next)
   {
      fwrite(tail->data, handle->size, 1, fp);
   }

   fclose(fp);
   return;
ERR2:
   fclose(fp);
ERR1:
   return;
}

/**
 * @brief 加载
 * 
 * @param path 路径 
 * @return LLIST* 
 */
LLIST *llist_load(const char *path)
{
   LLIST *handle = NULL;
   FILE *fp = NULL;
   int size, num;
   int ret, i;
   void *new = NULL;
   void *data_p = NULL;

   fp = fopen(path, "r");
   ERRP(fp == NULL, fopen, goto ERR1);

   ret = fread(&size, sizeof(size), 1, fp);
   ERRP(ret != 1, fread size, goto ERR2);

   ret = fread(&num, sizeof(num), 1, fp);
   ERRP(ret != 1, fread num, goto ERR2);

   handle = llist_create(size);
   ERRP(handle == NULL, llist_create, goto ERR2);

   new = (void *)malloc(size);
   ERRP(new == NULL, malloc new, goto ERR3);

   for (i = 0; i < num; i++)
   {
      ret = fread(new, size, 1, fp);
      ERRP(ret != 1, fread data, goto ERR4);

      ERRP(llist_append(new, handle, data_p) == -1, llist_aapend, goto ERR4);
   }

   fclose(fp);
   return handle;
ERR4:
   free(new);
ERR3:
   free(handle);
ERR2:
   fclose(fp);
ERR1:
   return NULL;
}

/**
 * @brief 遍历
 * 
 * @param handle 操作节点
 * @param op 打印或操作的钩子
 * @param flag 正序或逆序索引
 */
void llist_travel(LLIST *handle, llist_op_t *op, int flag) //遍历整个链表
{
   struct node_t *tail = NULL;

   if (!flag) //逆序
   {
      //next
      for (tail = handle->head.next; tail != &handle->head; tail = tail->next)
      {
         op(tail->data); //打印
      }
   }
   else
   {
      for (tail = handle->head.prev; tail != &handle->head; tail = tail->prev)
      {
         op(tail->data); //打印
      }
   }
}

/**
 * @brief 销毁
 * 
 * @param handle 操作节点
 */
void llist_destroy(LLIST *handle) //销毁链表
{
   struct node_t *tail = NULL;
   struct node_t *save = NULL;

   for (tail = handle->head.next; tail != &handle->head; tail = save)
   {
      save = tail->next;
      free(tail->data);
      free(tail);
   }
   free(handle);
}

/**
 * @brief 置空节点
 * 
 * @param handle 
 */
void llist_clean(LLIST *handle) //置空链表
{
   struct node_t *tail = NULL, *save = NULL;

   for (tail = handle->head.next; tail = &handle->head; tail = save)
   {
      save = tail->next;
      memset(tail->data, 0, handle->size);
      free(tail->data);
      free(tail);
   }

   memset(handle, 0, sizeof(LLIST));
   handle->head.next = NULL;
   handle->head.prev = NULL;

   free(handle);
}