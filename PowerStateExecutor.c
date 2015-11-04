/******************************************************************************
 * Implement for urat linstener, monitor the command from the scalar.
 *
 * Copyright (c) TPV IC. All rights reserved.
 *
 * author: yuming.zhang
 * modification:
 * 
*******************************************************************************/
#include <linux/fs.h>
#include <linux/mutex.h>
#include <linux/semaphore.h>
#include <linux/signal.h>
#include <linux/string.h>

#include "PowerStateExecutor.h"

static int Set_Screen_State(int on);

const PowerStateExecutor g_PowerStateExecutor = {
	.power_fpath = "/sys/power/state",
	.Set_Screen_State = Set_Screen_State,
};

static struct file *_fd_power = NULL;

//////////////////////////////////////////////////////////////////////////////
static bool _opened = false;

static const char *off_state = "mem";
static const char *on_state = "on";
static const char *eink_state = "standby";

////////////////////////////////////////////////////////////////////////////////
static int Open_PowerFile(void){
  if (_opened)
  {
  	if (NULL == _fd_power)
  	{
			_opened = false;
			
			return -1;
		}
		else
		{
			printk ("power file opened already\n");
			return 0;	
		}
  }
  
  _fd_power = filp_open(g_PowerStateExecutor.power_fpath, O_RDWR, S_IRUSR); //read+write 
  
  if (NULL == _fd_power)
  {  
    printk("power file open %s failed\n", g_PowerStateExecutor.power_fpath);
    
    _opened = false;
		
    return -1; 
  }

  _opened = true;
  
  return 0;	
}

static int Set_Screen_State(int on){
	char buf[32];
	int ret = 0,len;
	
	if (!_opened)
	{
		Open_PowerFile();
	}
	
	if(_ON_STATE == on){
		len = sprintf(buf, "%s", on_state);
		ret = _fd_power->f_op->write(_fd_power, buf, len, &_fd_power->f_pos);
	}else if(_OFF_STATE == on){
		len = sprintf(buf, "%s", off_state);
	}else if(_EINK_STATE == on){
		len = sprintf(buf, "%s", eink_state);
	}

	return ret;
}


