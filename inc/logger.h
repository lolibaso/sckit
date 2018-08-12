/*
 *  simple file log class 
 *  适用于单进程单线程, 高效快速
 */
#ifndef __sc_logger_h
#define __sc_logger_h

#include <time.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "singleton.h"

#define LOG_BUF_LEN             4096
#define DEFAULT_LOG_SIZE        1073741824    /* 1G = echo "2^30" | bc -l */
#define DEFAULT_LOG_NUM         100

namespace sc {

typedef enum LogLevel 
{
	LOG_LEVEL_NONE  = 0,      /* no log */
	LOG_LEVEL_TRACE = 1,
	LOG_LEVEL_DEBUG = 1 << 1,
	LOG_LEVEL_WARN  = 1 << 2,
	LOG_LEVEL_ERROR = 1 << 3,
	LOG_LEVEL_FATAL = 1 << 4,
	LOG_LEVEL_ALL = (LOG_LEVEL_TRACE | LOG_LEVEL_DEBUG | LOG_LEVEL_WARN | LOG_LEVEL_ERROR | LOG_LEVEL_FATAL)
} LogLevel;

typedef enum LogBatchMod
{
    LOG_BATCH_NONE     = 0,
    LOG_BATCH_BY_SIZE  = 1,   /* 一定大小强制 fsync */
    LOG_BATCH_BY_COUNT = 2,   /* 一定次数强制 fsync */
    LOG_BATCH_BY_TIME  = 3,   /* 一定时间强制 fsync */
} LogBatchMod;

class logger 
{
public:
	logger();
	~logger();

public:
	int 	open(const char*  log_path,
                 LogLevel     log_level = LOG_LEVEL_ALL,
				 uint32_t     log_size  = DEFAULT_LOG_SIZE,
                 uint32_t     log_no    = DEFAULT_LOG_NUM);


	void 	close();

    /* format log */
	int 	logv(LogLevel log_level, const char *fmt, ...);
//	int 	logv(int log_level, const char *fmt, va_list va);

	void  	set_level(LogLevel ll) { log_level_ = ll; }

    void    set_batchmodel(LogBatchMod  batch, int arg) 
    { 
        log_batch_ = batch; 
        if(log_batch_ == LOG_BATCH_NONE)
            return;
        if(log_batch_ == LOG_BATCH_BY_SIZE)
        {
            if(arg < LOG_BUF_LEN)  batch_arg_  = LOG_BUF_LEN; 
            if(arg > 8 * LOG_BUF_LEN) batch_arg_ = 8 * LOG_BUF_LEN;
        }
        batch_arg_ = arg;
    }

    void    set_logsize(uint32_t log_size) { log_size_ = log_size; }

    void    set_logno(uint32_t log_no) { log_size_ = log_no; }

    const char* errmsg() const { return err_msg_; }

private:
	int 	shift_log();

	char*   datetime2string(char* buf);

	const char* loglevel2string(LogLevel log_level);

private:
	int 	log_level_;
	char	log_path_[1024];	
	int		log_size_;
	int		log_no_;
	int 	log_fd_;

	char	log_buf_[16*LOG_BUF_LEN]; // 64k
	int		log_buf_len_;
	LogBatchMod    log_batch_;
    /** LOG_BATCH_BY_SIZE 时， batch_arg 代表最大buffer大小，该size 达到，强制 fsync
     *  LOG_BATCH_BY_COUNT时， batch_arg 代表最大计数次数，该计数达到，强制 fsync
     *  LOG_BATCH_BY_TIME 时， batch_arg 代表最大等待毫秒，该等待时间到了，强制 fsync
     */
    int     batch_arg_;       
    char    err_msg_[1024];
};

#define  LOGGER  sc::singleton<sc::logger, sc::create_by_new>::instance()

// info log
#define InfoLog(log_fmt, log_arg...) \
    do{ \
        LOGGER->logv(sc::LOG_LEVEL_TRACE,   "[%s:%d][%s] " log_fmt , \
                     __FILE__, __LINE__, __FUNCTION__, ##log_arg); \
    } while (0)


// debug log
#define DebugLog(log_fmt, log_arg...) \
    do{ \
        LOGGER->logv(sc::LOG_LEVEL_DEBUG,   "[%s:%d][%s] " log_fmt , \
                     __FILE__, __LINE__, __FUNCTION__, ##log_arg); \
    } while (0)

// warn log
#define WarnLog(log_fmt, log_arg...) \
    do{ \
        LOGGER->logv(sc::LOG_LEVEL_WARN,   "[%s:%d][%s] " log_fmt , \
                     __FILE__, __LINE__, __FUNCTION__, ##log_arg); \
    } while (0)

// error log
#define ErrorLog(log_fmt, log_arg...) \
    do{ \
        LOGGER->logv(sc::LOG_LEVEL_ERROR,   "[%s:%d][%s] " log_fmt , \
                     __FILE__, __LINE__, __FUNCTION__, ##log_arg); \
    } while (0)

// fatal log
#define FatalLog(log_fmt, log_arg...) \
    do{ \
        LOGGER->logv(sc::LOG_LEVEL_FATAL,   "[%s:%d][%s] " log_fmt , \
                     __FILE__, __LINE__, __FUNCTION__, ##log_arg); \
    } while (0)


inline int log_open(  const char* log_path,
                      LogLevel log_level = LOG_LEVEL_ALL,
					  uint32_t log_size  = DEFAULT_LOG_SIZE,
					  uint32_t log_no    = DEFAULT_LOG_NUM)
{
	return LOGGER->open(log_path, log_level, log_size, log_no);
}


inline void log_close() { LOGGER->close(); }

} /* end of namespaec sc */

#endif  /* end of log.h */

