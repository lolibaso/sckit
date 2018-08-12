#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <fnmatch.h>
#include "file_util.h"

namespace sc {

uint64_t file_util::get_filesize(const char* filename)
{
    struct stat st;

    if(stat(filename, &st) < 0)
    {
        return 0;
    }

    return st.st_size;
}

bool file_util::file_exist(const char* filename)
{
    if(!filename)
    {
        return false;
    }

    return (access(filename, F_OK) == 0);
}

int64_t file_util::get_filecontent(const char* filename, void* buf, int64_t buf_len)
{
    int fd = open(filename, O_RDONLY);
    if(fd < 0)
    {
        return -1;
    }

    int64_t n = read_fromfd(fd, buf, buf_len); 
    close(fd);

    if(n < 0) 
    {
        return -2;
    }

    return n;
}


int64_t file_util::write_filecontent(const char* filename, const void* buf, int64_t buf_len)
{
    int fd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
    if(fd < 0)
    {
        return -1;
    }

    //int64_t n = write(fd, buf, buf_len);
    int64_t n = write_tofd(fd, buf, buf_len);
    close(fd);

    if(n < 0) 
    {
        return -2;
    }

    return n;
}

const char* file_util::get_filename(const char* path)
{
    if(!path)   
    {
        return NULL;
    }

    const char* p = strrchr(path, '/');
    if(p) 
    {
        return (p+1);
    }

    return path;
}

const char* file_util::get_fileext(const char* path)
{
    if(!path)   
    {
        return NULL; 
    }
    
    const char* p = strrchr(path, '.');
    if(p)
    {
        return (p+1);
    }

    return "";
}

const char* file_util::get_filepath(const char* path)
{
    if(!path)   
    {
        return NULL; 
    }

    const char* p = strrchr(path, '/');

    if(*(p+1)!='\0') 
    {
        p++;
    }

    return path;
}

int file_util::get_files_fromdir(const char* dirpath, const char* pattern, std::vector<std::string>& files_list)
{
    struct dirent*  pdirent;
    struct stat     st;

    DIR * dir = opendir(dirpath);
    if(NULL == dir)
    {
        return  -1;
    }

    char tmp_dirpath[512];    
    while(NULL != (pdirent = readdir(dir)))
    {
        if( (0 == strcmp(pdirent->d_name, ".")) ||
            (0 == strcmp(pdirent->d_name, "..")) )
            continue;

        memset(tmp_dirpath, 0x0, sizeof(tmp_dirpath));
        int  tmp_len = strlen(dirpath);
        strcpy(tmp_dirpath, dirpath);
        if( tmp_dirpath[tmp_len-1] != '/' )
        {
            tmp_dirpath[tmp_len++] = '/';
        }
        strcat(tmp_dirpath, pdirent->d_name);
        if(stat(tmp_dirpath, &st) < 0)
        {
            continue;
        }

        if(S_ISDIR(st.st_mode))
        {
            get_files_fromdir(tmp_dirpath, pattern, files_list);
        }
        else
        {
            if( (NULL == pattern) ||
                (*pattern == '\0') ||
                (0 == fnmatch(pattern, pdirent->d_name, FNM_PATHNAME | FNM_PERIOD)) )
            {
                files_list.push_back(pdirent->d_name);
            }
        }
    }
    closedir(dir);

    return 0;
}

int file_util::writepid_tofile(const char* pidfile)
{
     char str[32];

     int fd = open(pidfile, O_WRONLY|O_CREAT|O_TRUNC, 0600);
     if(fd < 0)  return -1;

     if(lockf(fd, F_TLOCK, 0) < 0)
     {
        close(fd);
        return -1;
     }

     snprintf(str, sizeof(str), "%d\n", (int)getpid());
     ssize_t len = strlen(str);
     //ssize_t ret = write(fd, str, len);
     ssize_t ret = write_tofd(fd, str, len);
     if (ret != len )
     {
         close(fd);
         return -1;
     }

    close(fd);

    return 0;
}

int file_util::getpid_fromfile(const char* pidfile)
{
    char buffer[32]= {0};

    int fd = open(pidfile, O_RDONLY, 0);
    if (fd < 0)
     return fd;


    //ssize_t rd = read(fd, buffer, 32);
    ssize_t rd = read_fromfd(fd, buffer, 32);
    close(fd);

    if (rd <= 0)
     return -1;


    char* p = strchr(buffer, '\n');
    if (p != NULL)
     *p = '\0';

    return atoi(buffer);
}

int 
file_util::read_fromfd(int fd, void* buf, int buf_len)
{
    int total_read = 0;

    while (total_read < buf_len) 
    {
        int n_read = read(fd, ((unsigned char*)buf + total_read), buf_len - total_read);
        if(n_read == -1)
        {
            if(errno == EINTR) continue;
            return total_read == 0 ? -1 : total_read;
        }
        total_read += n_read;
    }
        
    return total_read;
}

int 
file_util::write_tofd(int fd, const void* buf, int buf_len)
{
    int total_write = 0;

    while (total_write < buf_len) 
    {
        int n_write = write(fd, ((unsigned char*)buf + total_write), buf_len - total_write);
        if(n_write == -1)
        {
            if(errno == EINTR) continue;
            return total_write == 0 ? -1 : total_write;
        }
        total_write += n_write;
    }
        
    return total_write;
}

}  /* end of namespace sc */

#if 0
#include <stdio.h>
int main(int argc, char* argv[])
{
    std::vector<std::string>  vs;

    int nums  = 0;
    sc::file_util::get_files_fromdir(argv[1], argv[2], vs);

    for(int i=0; i<vs.size(); i++)
    {
        printf("No.%04d %s\n", i+1, vs[i].c_str());
    }
    
    return 0;
}
#endif

