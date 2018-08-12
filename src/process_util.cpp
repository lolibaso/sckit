#include "process_util.h"
#include "file_util.h"  // GetFileContent()
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <stdlib.h>
#include <syslog.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace sc {

pid_t process_util::get_pid()
{
    return ::getpid();
}

char* process_util::get_pidstring()
{
    static char buf[32];
    snprintf(buf, sizeof(buf), "%d", ::getpid());
    return buf;
}

uid_t process_util::get_uid()
{
    return ::getuid();    
}

char* process_util::get_username()
{
    static char name[256] ="unkownuser"; 
    struct passwd  pwd;
    struct passwd* result;
    char   buf[8192];

    getpwuid_r(::getuid(), &pwd, buf, sizeof(buf), &result);

    if(result) 
    {
        strncpy(name , pwd.pw_name, sizeof(name)-1);
    }

    return name;
}

char* process_util::get_hostname()
{
    static char buf[256];

    if(::gethostname(buf, sizeof(buf)) < 0)
    {
        strcpy(buf, "unknowhost");
    }
    
    return buf;
}

char* process_util::get_procname()
{
    static char buf[512];

    char* p = get_procstat();
    char* start = strchr(p, '(');
    char* end   = strchr(p, ')');

    if(start && end) 
    {
        strncpy(buf, start, end-start);
    }
    else 
    {
        strcpy(buf, "unkownproc");
    }

    return buf;
}

char* process_util::get_procstat()
{
    static char buf[4096];
    file_util::get_filecontent("/proc/self/stat", buf, sizeof(buf));
    return buf;
}

char* process_util::get_exepath()
{
    static char buf[1024];
    ssize_t n = ::readlink("/proc/self/stat", buf, sizeof(buf));
    if(n < 0) 
    {
        strcpy(buf, "unkonwexepath");
    }
    return buf;
}




/*---------------------------------------------------------------------------*\
                              Static Routines
\*---------------------------------------------------------------------------*/

/* redirect_fds(): redirect stdin, stdout, and stderr to /dev/NULL */

static void redirect_fds()
{
   (void) close(0);
   (void) close(1);
   (void) close(2);

   if (open("/dev/null", O_RDWR) != 0)
   {
       exit(1);
   }

   dup(0);
   dup(0);
}

static int do_fork(void)
{
	int ret = 0;

    switch(fork())
    {
        case 0:
            /* This is the child that will become the daemon. */
            break;

        case -1:
            /* Fork failure. */
			ret = -1;
            break;

        default:
            /* Parent: Exit. */
            _exit(0);
    }

    return ret;
}

static int getpidfromfile(const char* pidfile)
{
    char buf[64];

    int fd = open(pidfile, O_RDONLY, 0);
    if (fd < 0) 
	{
        return fd;
    }

    ssize_t n = read(fd, buf, 64);
    close(fd);
    if (n <= 0) 
	{
        return -1;
    }

    buf[63] = '\0';
    char* p = strchr(buf, '\n');
    if (p != NULL)
        *p = '\0';
    return atoi(buf); 
}

static int writepidtofile(const char *pidfile)
{
    char str[32];
    int fd = open(pidfile, O_WRONLY|O_CREAT|O_TRUNC, 0600);
    if (fd < 0) 
	{
        fprintf(stderr, "Can't open Pid File: %s", pidfile);
        return -1;
    }

	if(lockf(fd, F_TLOCK, 0) < 0)
	{
	    fprintf(stderr, "Can't lock Pid File: %s", pidfile);
		close(fd);
        return -2;
	}

    sprintf(str, "%d\n", getpid());
    ssize_t len = strlen(str);
    ssize_t ret = write(fd, str, len);
	close(fd);
    if (ret != len ) 
	{
        fprintf(stderr, "Can't Write Pid File: %s", pidfile);
        return -1;
    }

    return 0;
}

/*---------------------------------------------------------------------------*\
                              Public Routines
\*---------------------------------------------------------------------------*/

int process_util::daemon(int nochdir, int noclose)
{
    /* Fork once to go into the background. */
    if( do_fork() < 0 )
    {
        return -1; 
    }

    /* Create new session */
    if(setsid() < 0)               /* shouldn't fail */
    {
        return -2;
    }
    
    /* clear any inherited umask(2) value */
    umask(0);

    /* We're there. */
    if(! nochdir)
    {
        /* Go to a neutral corner. */
        chdir("/");
    }

    if(! noclose)
        redirect_fds();

    return 0;
}

int process_util::daemon_start(const char* pidfile)
{
	int pid = getpidfromfile(pidfile);    

	if (pid > 0) 
	{
        if (kill(pid, 0) == 0 || errno == EPERM) 
		{
            fprintf(stderr, "daemon exists, use restart\n");
            return -1;
        }
    }

    if (::getppid() == 1) {
        fprintf(stderr, "already daemon, can't start\n");
        return -1;
    }

	if(do_fork() < 0)	
	{
		return -2;	
	}

	/* Create new session */
    if(setsid() < 0)               /* shouldn't fail */
    {
        return -2;
    }

    /* clear any inherited umask(2) value */
    umask(0);	

	if(writepidtofile(pidfile) < 0)
	{
		return -3;	
	}

	/* We're there. */
    {
        /* Go to a neutral corner. */
        chdir("/");
    }

	redirect_fds();	

	return 0;
}

int process_util::daemon_stop(const char* pidfile)
{
	int pid = getpidfromfile(pidfile);
	
	if(kill(pid, SIGQUIT) < 0)	
	{
 		fprintf(stderr, "program %d not exists\n", pid);
		return -1;
	}

	for (int i = 0; i < 300; i++) 
	{
        usleep(10*1000);
        if(kill(pid, SIGQUIT) < 0)
		{
            fprintf(stderr, "program %d exited\n", pid);
            unlink(pidfile); // why?
            return 0;
        }
    }
    fprintf(stderr, "signal sended to process, but still exists after 3 seconds\n");

    return -1;	
}

int process_util::daemon_restart(const char* pidfile)
{
	int pid = getpidfromfile(pidfile);	

	if(pid > 0)
	{
		if(kill(pid, 0) == 0)
		{
			if(daemon_stop(pidfile) < 0)		
				return -1;
		}
		else if(errno == EPERM)
		{
			fprintf(stderr, "do not have permission to kill process: %d\n", pid);
			return -2;
		}
	}
	else
	{
		fprintf(stderr, "pid file not valid, just ignore\n");
	}
	
	return daemon_start(pidfile);	
}

void process_util::daemon_process(const char* cmd, const char* pidfile)
{
	if(cmd == NULL)
	{
		fprintf(stderr, "Error: bad daemon cmd. exit...\n");	
		exit(1);
	}
	
	int ret = -1;
	if(strcasecmp(cmd, "start") == 0)	
	{
		ret  = daemon_start(pidfile);
	}
	else if(strcasecmp(cmd, "stop") == 0)
	{
		ret  = daemon_stop(pidfile);
	}
	else if(strcasecmp(cmd, "restart") == 0)
	{
		ret = daemon_restart(pidfile);
	}
	else
	{
		fprintf(stderr, "Error: bad daemon cmd. exit...\n");	
	}

	exit(ret);
}


}


#if 0

#include <stdio.h>
int main(int argc, char* argv[])
{
    printf("%s\n", sc::process_util::get_exepath());
    
    return 0;
}
#endif

