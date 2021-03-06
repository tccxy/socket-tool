/**
 * @file types.h
 * @author zhao.wei (hw)
 * @brief 数据类型重定向
 * @version 0.1
 * @date 2020-07-17
 * 
 * @copyright Copyright (c) 2020
 * 
 */


#ifndef _TYPES_H_
#define _TYPES_H_

#ifndef CPU_WORD_SIZE
#define CPU_WORD_SIZE 64
#endif

#if (CPU_WORD_SIZE == 32)
typedef unsigned short int  u16;
typedef signed short int    s16;

typedef unsigned int        u32;
typedef signed   int        s32;

typedef unsigned long long  u64;
typedef signed long long    s64;

#endif


#if (CPU_WORD_SIZE == 64)
typedef unsigned short int  u16;
typedef signed short int    s16;

typedef unsigned int        u32;
typedef signed   int        s32;

typedef unsigned long       u64;
typedef signed   long       s64;

#endif
typedef unsigned char       u8;
typedef signed char         s8;

typedef union _semun {
  int val;
  struct semid_ds *buf;
  char *array;
} semun;

#endif
