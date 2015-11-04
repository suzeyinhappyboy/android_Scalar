/******************************************************************************
 * Implement for urat linstener, monitor the command from the scalar.
 *
 * Copyright (c) TPV IC. All rights reserved.
 *
 * author: jiping.yu
 * modification:
 * 
*******************************************************************************/

#include <linux/fs.h>
#include <linux/mutex.h>
#include <linux/semaphore.h>
#include <asm/termios.h>
#include <linux/tty.h>
#include <linux/signal.h>
#include <linux/string.h>
#include<asm/uaccess.h>

#include "UartExecutor.h" 

/////////////////////////////////Phototype define///////////////////////////////
//static void _tty_OnData(int data);
static int Read_ScalarTTyLocked(unsigned char *buf, int len, struct timeval* timeout);

//add by zhangym
static int Write_ScalarTTyLocked(unsigned char *buf, int len, struct timeval* timeout);

//set uart baudrate 
extern void set_baud(int line, unsigned long newbaud);
//get uart baudrate
extern int get_baud(int line);

////////////////////////////////Global Objects//////////////////////////////////
const UartExecutor g_UartExecutor = {
  //.tty = "/dev/ttyHS1", //mxc1",		//scalar tty
  .tty = "/dev/ttyS1", //mxc1",		//scalar tty
  .baud = B9600,	
  .Read_ScalarTTyLocked = Read_ScalarTTyLocked,
  .Write_ScalarTTyLocked = Write_ScalarTTyLocked,
};

////////////////////////////////////////////////////////////////////////////////
static struct file *_fd_tty = NULL;
static bool _opened = false;

/*delete by haishan.lin*/
//static struct mutex    	 _mutex;
//static struct semaphore  _cond;

//bool _MacSerialReed = false;
////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
 **
 **
 **  
********************************************************************************/
static int Open_ScalarTTy(void)
{
  struct termios     opt;
  //struct sigaction   saio;
  
  if (_opened)
  {
  	if (IS_ERR(_fd_tty) || (NULL == _fd_tty))
  	{
		_opened = false;
		goto open_ttyS1;
	}
	else
	{
		/***********
		if(9600 != get_baud(1)) {
			set_baud(1,9600);
		}**********/

		return 0;
	}
  }

open_ttyS1:  
  if(false == _opened)
      _fd_tty = filp_open(g_UartExecutor.tty, O_RDWR, S_IRWXU|S_IRWXG|S_IRWXO); //read+write 
  
  if (IS_ERR(_fd_tty) || (NULL == _fd_tty))
  {  
    printk("tty open %s failed,errno=%ld\n", g_UartExecutor.tty,PTR_ERR(_fd_tty));
    
    _opened = false;
    return -1; 
  }
  
  //settings of the UART
  _fd_tty->f_op->unlocked_ioctl(_fd_tty, TCGETS, (unsigned long)&opt);
  
  opt.c_iflag = 0;
  opt.c_oflag = 0;
  opt.c_lflag = 0;
  opt.c_cflag = CLOCAL | CS8 | CREAD;
  opt.c_cc[VMIN] = 0;
  opt.c_cc[VTIME] = 0;
  opt.c_cflag |= g_UartExecutor.baud;		
  
  _fd_tty->f_op->unlocked_ioctl(_fd_tty, TCSETS, (unsigned long)&opt);

  _opened = true;
  	
  return 0;
}

/*******************************************************************************
 **
 **
 **  
********************************************************************************/
static int Read_ScalarTTyLocked(unsigned char *buf, int len, struct timeval* timeout)
{
	int               ret = 0;
	int _tty_buffer = 0;

	unsigned char _complete_pkglen = 1;		//9
	//unsigned long     arg = 0;
	struct tty_struct *pTTy = NULL;

	if (-1 == Open_ScalarTTy()) { 
		return -1;
	}

/*
	if(true == _MacSerialReed){
		_complete_pkglen = 10;
	}else if(false == _MacSerialReed){
		_complete_pkglen = 9;
	}
	*/
	pTTy = ((struct tty_file_private*)(_fd_tty->private_data))->tty;
	
	_tty_buffer = pTTy->ldisc->ops->chars_in_buffer(pTTy);
	if(_tty_buffer >= _complete_pkglen) //(pTTy->ldisc->ops->chars_in_buffer(pTTy) > 0)
	{
		ret = _fd_tty->f_op->read(_fd_tty, buf, len, &_fd_tty->f_pos);
		printk ("tty have got data bytes %d and %d\n", ret,len);
	}

	return ret; 
}
/*******************************************************************************
 **
 **
 **  

********************************************************************************/
static int Write_ScalarTTyLocked(unsigned char *buf, int len, struct timeval* timeout)
{
	if(-1 == Open_ScalarTTy()){
		return -1;
	}
	set_fs(KERNEL_DS);
	return _fd_tty->f_op->write(_fd_tty, buf, len, &_fd_tty->f_pos);
}
/*******************************************************************************
 **
 **
 **  

********************************************************************************/

/*delete by haishan.lin : defined but not used*/
/*
static int Write_ScalarTTy(unsigned char *buf, int len)
{
  return 0;
}
*/

/*
static void _tty_OnData(int data)
{
  mutex_lock(&_mutex);
  
  //do some thing on Mutex eara befor release condition lock
  printk("UratListener::_OnData catched\n");
  
  up(&_cond);
  
  mutex_lock(&_mutex);
  
  return;

} */

