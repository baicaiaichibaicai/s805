#ifndef	_LOGM_H_
#define _LOGM_H_


#define LOG_ERROR			3
#define LOG_WARN			5
#define LOG_INFO			7
#define	LOG_DEBUG			9
#define LOG_VERBOSE			10

#define _KERNEL 1

#if _KERNEL
#define LOGm(level, format, args...) 							\
		do {													\
			char *k = (level >= LOG_DEBUG)? KERN_DEBUG:			\
					  (level >= LOG_INFO)?  KERN_INFO:			\
					  (level >= LOG_WARN)?  KERN_WARNING:		\
					  KERN_ERR;									\
			if (level > g_log_level)							\
				break;											\
			printk("%s %s:%d ", k, __FUNCTION__, __LINE__);		\
			printk(format, ##args);								\
		} while(0)

#define LOG_HEX(level, ptr, len)								\
		do {													\
			int _i;												\
			if (level > g_log_level)							\
				break;											\
			for (_i=0; _i<len; _i++) {							\
				if((_i%0x10)==0) printk("%04x  ", _i);			\
				printk("%02x ", ptr[_i]);						\
				if((_i+1)%0x10==0) printk("\n");				\
			}													\
			if ((_i%0x10) != 0) printk("\n");					\
		} while (0)


#else


#define LOGm(level, format, args...) 							\
		do {													\
			char *k = (level >= LOG_VERBOSE)? "V":				\
					  (level >= LOG_DEBUG)? "D":				\
					  (level >= LOG_INFO)?  "I":				\
					  (level >= LOG_WARN)?  "W":				\
					  "E";										\
			if (log_level > g_log_level)						\
				break;											\
			printk("%s/%s:%d ", k, __FUNCTION__, __LINE__);		\
			printk(format, ##args);								\
		} while(0)

#define LOG_HEX(log_level, ptr, len)							\
		do {													\
			int _i;												\
			if (log_level > g_log_level)						\
				break;											\
			for (_i=0; _i<len; _i++) {							\
				if((_i%0x10)==0) printk("%04x  ", _i);			\
				printk("%02x ", ptr[_i]);						\
				if((_i+1)%0x10==0) printk("\n");				\
			}													\
			if ((_i%0x10) != 0) printk("\n");					\
		} while (0)

#endif


#define LOGE(format, args...)	LOGm(LOG_ERROR, format, ##args)
#define LOGW(format, args...)	LOGm(LOG_WARN, format, ##args)
#define LOGI(format, args...)	LOGm(LOG_INFO,  format, ##args)
#define LOGD(format, args...)	LOGm(LOG_DEBUG, format, ##args)
#define LOGV(format, args...)	LOGm(LOG_VERBOSE, format, ##args)


extern int g_log_level;

#endif /* _LOGM_H_ */



