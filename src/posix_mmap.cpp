#include "posix_mmap.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

namespace sc {

    posix_mmap::posix_mmap(bool is_owner):is_owner_(is_owner), addr_(NULL), length_(0), is_created_(false)
    {
    }


    posix_mmap::~posix_mmap()
    {
        if(is_owner_)
        {
            close();
        }
    }


    void* posix_mmap::open(size_t length, int prot, int flags, int fd, off_t offset)
    {
        if(is_owner_)
        {
            close();
        }

        addr_ = ::mmap(NULL, length, prot, flags, fd, offset);
        if((void*)-1 == addr_)
        {
            addr_ = NULL;
            return NULL;

        }
        length_ = length;
        is_created_ = false;

        return addr_;
    }

    void* posix_mmap::open(const char* file, size_t length)
    {
        if(is_owner_)
        {
            close();
        }

        int fd = ::open(file, O_CREAT | O_EXCL | O_RDWR, 0666);
        if(fd < 0)
        {
            if(errno != EEXIST)
            {
                return NULL;
            }
            else
            {
                fd = ::open(file, O_CREAT | O_RDWR, 0666);
                if(fd < 0) 
                {
                    return NULL;
                }
            }
        }

        is_created_ = true;
        lseek(fd, length-1, SEEK_SET);
        write(fd, "\0", 1);

        addr_ = ::mmap(NULL, length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        if((void*)-1 == addr_)
        {
            addr_ = NULL;
        }
        else
        {
            length_ = length;
        }
        ::close(fd);

        return addr_;
    }


    void posix_mmap::close()
    {
        if(NULL == addr_)
        {
            return;
        }

        if( ::munmap(addr_, length_) < 0 )
        {
            return;
        }

        addr_   = NULL;
        length_ = 0;
        is_created_ = false;

        return;
    }

    void posix_mmap::msync(bool sync)
    {
        ::msync(addr_, length_, sync ? (MS_SYNC | MS_INVALIDATE) : (MS_ASYNC | MS_INVALIDATE));
    }

} // end of namespace sc



