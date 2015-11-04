/******************************************************************************
 * scalar interaction proxy.
 *
 * Copyright (c) TPV IC. All rights reserved.
 *
 * author:
 * modification:
 * 
*******************************************************************************/
#ifndef _UART_EXECUTOR_H_
#define _UART_EXECUTOR_H_

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

typedef struct UartExecutor_T 
{
  char  *tty;
  int   baud;
  
  int  (*Read_ScalarTTyLocked)(unsigned char*, int, struct timeval*);
  
  //add by zhangym
  int  (*Write_ScalarTTyLocked)(unsigned char*, int, struct timeval*);
  
}UartExecutor, ptr_UartExecutor;

#endif
