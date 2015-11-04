#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#define CONFIG_ENV_OFFSET      (768*1024)
#define CONFIG_ENV_BLOCK_SIZE	       (512)
#define CONFIG_ENV_SIZE_8K     0x2000

#define CONFIG_MAC             "0a:04:9f:02:4c:77"

typedef	struct environment_s 
{
	uint32_t	crc;		      /* CRC32 over data bytes	*/

//#ifdef CONFIG_SYS_REDUNDAND_ENVIRONMENT
//	unsigned char	flags;	/* active/obsolete flags	*/
//#endif

	unsigned char	data[CONFIG_ENV_SIZE_8K]; /* Environment data		*/
} env_t;

int resolve_mac (char *new_mac);

