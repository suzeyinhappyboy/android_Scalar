#include <linux/fs.h>
#include <linux/mutex.h>
#include <linux/semaphore.h>
#include <linux/signal.h>
#include <linux/string.h>

#include "env_resolve.h"
#include "env_crc32.h"

static env_t env_pt;

////////////////////////////////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////////////////////
static unsigned char env_get_char (int index)
{
	return (env_pt.data[index]);
}

////////////////////////////////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////////////////////
static unsigned char* env_get_addr(int index)
{
	return &(env_pt.data[index]);
}

////////////////////////////////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////////////////////
static int envmatch (unsigned char *s1, int i2)
{
	while (*s1 == env_get_char(i2++))
  {
    	if (*s1++ == '=')
			{
        return(i2);
      }
  }
	    
	if (*s1 == '\0' && env_get_char(i2-1) == '=')
  {
      return(i2);
  }
	
	return(-1);
}

////////////////////////////////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////////////////////
static char *get_env (char *name)
{
	int i, nxt;

	for (i=0; env_get_char(i) != '\0'; i=nxt+1) 
	{
		int val;

		for (nxt=i; env_get_char(nxt) != '\0'; ++nxt) 
		{
			if (nxt >= CONFIG_ENV_SIZE_8K) 
			{
				return (NULL);
			}
		}
		
		if ((val=envmatch((unsigned char *)name, i)) < 0)
		{
			continue;
		}
		
		return ((char *)env_get_addr(val));
	}

	return (NULL);
}

////////////////////////////////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////////////////////
int resolve_mac (char *new_mac)
{
  char  *ptrHead = NULL; 
  char  *ptrData = NULL;
  char  *strMac  = NULL;
  char  _strNewMac[32];
  int   blk      = 0;
  struct file *fd = NULL;
  int   ret;
  
  if (NULL == new_mac)
  {
      return -1;
  }
    
  fd = filp_open("/dev/block/mmcblk0", O_RDWR, S_IRUSR); //read+write 

  if (NULL == fd) 
  {
      printk("could not open /dev/block/mmcblk0 \n");
      return 1;
  }
	
	ret = fd->f_op->llseek(fd, CONFIG_ENV_OFFSET, SEEK_SET);
	printk("xuzhc====lseek ret = [%d] \n", ret);

	memset((void *)&env_pt, 0x00, sizeof(env_pt));
	
	ret = fd->f_op->read (fd, (char*)&env_pt, CONFIG_ENV_BLOCK_SIZE, &(fd->f_pos));
	printk("xuzhc====read ret = [%d], get var %s\n", ret, env_pt.data);
	if((-1 == ret) || (0 == ret)){
		printk("there is no data or read file erro!\n");
		goto close_fd;
	}
	
	ptrHead = (char*)&env_pt;
  	blk = 1;   //for just read one block already

	//read 512 byte block, and check the env vars in every block 
  for (ptrData = env_pt.data; strlen(ptrData) > 0 && ptrData < (ptrHead + CONFIG_ENV_SIZE_8K); blk++)
	{
		for (;  ptrData < (ptrHead + (blk * CONFIG_ENV_BLOCK_SIZE)); ptrData += (strlen(ptrData) + 1))
		{
			if (strlen(ptrData) <= 0)
			{
				break;
			}

			printk("get env string %s \n", ptrData);
		}

		if (strlen(ptrData) <= 0)
		{
			break;
		}
		
    	ret = fd->f_op->read (fd, ptrHead + (blk * CONFIG_ENV_BLOCK_SIZE), CONFIG_ENV_BLOCK_SIZE, &(fd->f_pos));
		
	}

  	
	//add by zhangym, mac to str
	sprintf(_strNewMac,"%02X:%02X:%02X:%02X:%02X:%02X", 
					(u8)new_mac[0], (u8)new_mac[1],
					(u8)new_mac[2], (u8)new_mac[3],
					(u8)new_mac[4], (u8)new_mac[5]);

	printk("mac address to str == %s \n", _strNewMac);
	
	//search var
	strMac = get_env("ethaddr");
	if (NULL != strMac) 
	{
		printk("xuzhc=====pszMac = %s \n", strMac);

		if (0 == strcmp(strMac, _strNewMac))
		{
			printk("the Scalar Mac add as same as env Add!\n");
	
			goto close_fd;
		}

		printk("A new Mac Address has burn to Scalar\n");
		
		//copy the new mac address to env, add by zhangym	
		strcpy(strMac, _strNewMac);
	}
	else
	{
		char *strMac_Temp = "ethaddr=";
		strcat (strMac_Temp, _strNewMac);

		printk("there must add a var to env\n", ptrData, env_pt.data, strMac_Temp);
		
		strcpy(ptrData, strMac_Temp); 
	}

	env_pt.crc = 0;
	env_pt.crc = crc32(0, env_pt.data, CONFIG_ENV_SIZE_8K - sizeof(uint32_t)); 
	printk("xuzhc=====env_pt.crc = [%x] \n", env_pt.crc);

  //write back to eMMc
	ret = fd->f_op->llseek(fd, CONFIG_ENV_OFFSET, SEEK_SET);

	ret = fd->f_op->write(fd, (char *)&env_pt, CONFIG_ENV_SIZE_8K, &fd->f_pos);

	printk("xuzhc===write===ret = %d \n", ret);
  
    fd->f_op->fsync;

close_fd:
	filp_close(fd,NULL);

	return 0;
}
