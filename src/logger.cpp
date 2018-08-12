#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>
#include "logger.h"

namespace sc {

logger::logger(): log_level_(LOG_LEVEL_ALL),
	        log_size_(DEFAULT_LOG_SIZE),
	        log_no_(DEFAULT_LOG_NUM),
	        log_fd_(-1),
	        log_buf_len_(0),
	        log_batch_(LOG_BATCH_NONE)
{
	memset(log_buf_, 0x0, sizeof(log_buf_));
	memset(log_path_, 0x0, sizeof(log_path_));
	memset(err_msg_, 0x0, sizeof(err_msg_));
}

logger::~logger()
{
	close();
}

int 
logger::open( const char* log_path, LogLevel log_level, uint32_t log_size, uint32_t log_no)
{
	log_level_ = log_level;
	log_size_  = log_size >= DEFAULT_LOG_SIZE ? DEFAULT_LOG_SIZE : log_size;
	log_no_    = log_no >= DEFAULT_LOG_NUM ? DEFAULT_LOG_NUM : log_no;
    strncpy(log_path_, log_path, sizeof(log_path_));
    
	log_fd_ = ::open(log_path_, O_CREAT | O_RDWR | O_APPEND, 0666);
	if(log_fd_ < 0) 
	{
        snprintf(err_msg_, sizeof(err_msg_), "open() %s failed: %s\n", log_path_, strerror(errno));
        return -1;
	}

	return 0;
}

void logger::close() 
{
	if(log_fd_ > 0) 
	{
		fsync(log_fd_);
		::close(log_fd_);
	}

	return ;
}

const char* logger::loglevel2string(LogLevel log_level)
{
    switch(log_level) 
    {
    case LOG_LEVEL_TRACE:
        return "TRACE";
    case LOG_LEVEL_DEBUG:
        return "DEBUG";
    case LOG_LEVEL_ERROR:
        return "ERROR";
    case LOG_LEVEL_WARN:
        return "WARN";
    case LOG_LEVEL_FATAL:
        return "FATAL";
    default:
        return "???" ; 
    }
}

char* logger::datetime2string(char* timestr)
{
	struct tm tmm;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &tmm);

    sprintf(timestr, "[%04d-%02d-%02d %02d:%02d:%02d.%.04d]",
                tmm.tm_year > 50 ? tmm.tm_year+1900 : tmm.tm_year+2000 , 
                tmm.tm_mon+1, 
                tmm.tm_mday,
                tmm.tm_hour, 
                tmm.tm_min, 
                tmm.tm_sec,
                (int)tv.tv_usec);
       
    return timestr;
}

int logger::logv(LogLevel log_level, const char *fmt, ...)
{
	if(LOG_LEVEL_NONE == (log_level & log_level_))
	{
		return 0;
	}

    char buf[128];
    int  tmp_len = 0;

    tmp_len = sprintf(log_buf_+log_buf_len_, "[%s][%s]", datetime2string(buf), loglevel2string(log_level));
    log_buf_len_ += tmp_len;

    va_list ap;
    va_start(ap, fmt);
    //tmp_len = vsnprintf(log_buf_+log_buf_len_, (LOG_BUF_LEN-log_buf_len_-1), fmt, ap);
    tmp_len = vsnprintf(log_buf_+log_buf_len_, (LOG_BUF_LEN-1), fmt, ap);
    va_end(ap);
    log_buf_len_ += tmp_len;

    switch (log_batch_)
    {
    case  LOG_BATCH_BY_SIZE:
        {
        if(log_buf_len_ < batch_arg_)  return 0;
        break;
        }
    case  LOG_BATCH_BY_COUNT:
        {
        static int count = 0;
        if(count < batch_arg_) return 0;
        ++count;
        break;
        }
    case  LOG_BATCH_BY_TIME:
        // 以后再实现 
    case  LOG_BATCH_NONE:
    default:
        // do nothing
        break;
    }

    write(log_fd_, log_buf_, log_buf_len_);
    log_buf_len_ = 0;
    
    return shift_log();
}
    

int logger::shift_log()
{
	struct stat st;

	if(stat(log_path_, &st) < 0)
    {
        /* try again */
		::close(log_fd_);

		log_fd_ = ::open(log_path_, O_CREAT | O_RDWR | O_APPEND, 0666);
		if(log_fd_ < 0) {
			// error report
		    sprintf(err_msg_, "::open(%s) failed : %s", log_path_, strerror(errno));
			return -1;
		}
        int flag = fcntl(log_fd_, F_GETFD);
        if(flag >= 0) {
            fcntl(log_fd_, F_SETFD, flag | FD_CLOEXEC);
        }

        return 0;
    }

    /* no need shift */
    if(st.st_size < log_size_) {
        return 0;
    }

	char   tmp_log_path[1024];
	char   new_log_path[1024];
    for(int i=log_no_-2; i>=0; i--) {
        if(i==0)
            sprintf(tmp_log_path, "%s", log_path_);
        else
            sprintf(tmp_log_path, "%s.%d", log_path_, i);

        if(access(tmp_log_path, F_OK) == 0) {
            sprintf(new_log_path, "%s.%d", log_path_, i+1);
            unlink(new_log_path);

            if(rename(tmp_log_path, new_log_path) < 0) {
                sprintf(err_msg_, "rename from (%s) to (%s) failed: %s", tmp_log_path, new_log_path, strerror(errno));
                return -2;
			}
        }
    }

	return 0;
}

}  /* end of namespace sc */

#if 0
using namespace sc;
#include <time.h>
#include <sys/time.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	loggerOpen(LOG_LEVEL_DEBUG ,
				LOG_TYPE_FILE,
				"./",
                "test.log",
				1024*1024*10,
				10,
				0, 1);
	int i = 0;

    Debuglogger("test start\n");
    clock_t  start, end;
    struct timeval tv = {0};

    double   duration;

    start = clock();

    char s[515] = "111111111111111111111111111111111111";

    int times = atoi(argv[1]);
    while(i< times)
    {
        Debuglogger("You input:%d: %s\n",i, s);
        i++;
    }
    end = clock();

    duration = (double)(end - start)/CLOCKS_PER_SEC;

    printf("log: %d times used %f seconds\n", times, duration);

	loggerClose();

	return 0;
}
#endif
