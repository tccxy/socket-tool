/**
 * @file pub.h
 * @author zhao.wei (hw)
 * @brief 
 * @version 0.1
 * @date 2020-08-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef _PUB_H_
#define _PUB_H_

#include <errno.h>
#include <net/if.h>
#include <netdb.h>      // getaddrinfo()
#include <netinet/in.h> // e.g. struct sockaddr_in on OpenBSD
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h> // read()/write()
#include <getopt.h>
#include <pthread.h>
#include <termios.h>
#include <sys/sem.h>
#include <sys/ipc.h>

#include "types.h"
#include "error.h"
#include "rcvdata_structure.h"
#include "socket_interface.h"

//#define DEBUG_EN
#ifdef DEBUG_EN
#define DEBUG(format, ...) printf("File: "__FILE__             \
                                  ",(%s) Line: %05d: " format, \
                                  __func__, __LINE__, ##__VA_ARGS__)

#else
#define DEBUG(format, ...)
#endif

#endif