/******************************************************************************
 * scalar interaction proxy.
 *
 * Copyright (c) TPV IC. All rights reserved.
 *
 * author:		zhangym
 * modification:
 * 
*******************************************************************************/
#ifndef _POWERSTATE_EXECUTOR_H_
#define _POWERSTATE_EXECUTOR_H_

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#define 	_ON_STATE		1
#define 	_OFF_STATE		0
#define 	_EINK_STATE		2

enum {
    ACQUIRE_PARTIAL_WAKE_LOCK = 0,
    RELEASE_WAKE_LOCK,
    REQUEST_STATE,
    OUR_FD_COUNT
};

/*
const char * const file_path[] = {
    "/sys/power/wake_lock",
    "/sys/power/wake_unlock",
    "/sys/power/state"
};
*/
	
typedef struct PowerStateExecutor_T 
{
  char *power_fpath; 
  	
  int  (*Set_Screen_State)(int on);
  
}PowerStateExecutor, ptr_PowerStateExecutor;

#endif

