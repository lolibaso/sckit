#ifndef __sc_process_util_h
#define __sc_process_util_h
#include <sys/types.h>
#include <unistd.h>

namespace sc {

class process_util 
{
public:
    static  pid_t get_pid();
    static  uid_t get_uid();
    static  char* get_pidstring();
    static  char* get_hostname();
    static  char* get_procname();
    static  char* get_username();
    static  char* get_procstatus();
    static  char* get_procstat();
    static  char* get_exepath();

    /**
	 * @brief 成为守护进程
	 *
	 * @param nochdir 非0，改变当前目录去 /root
	 * @param noclose 非0，将 stdin/stdout/stderr 重定向到 /dev/null
	 *
	 * @return 0 success, -1 failed
	 */
    static int daemon(int nochdir, int noclose);
    static int daemon_start(const char* pidfile);
    static int daemon_restart(const char* pidfile);
    static int daemon_stop(const char* pidfile);
    static void daemon_process(const char* cmd,const char* pidfile); 
    

}; 

} // end of namespace sc 

#endif 
