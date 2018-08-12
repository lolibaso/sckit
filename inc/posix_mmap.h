#ifndef __sc_mmap_h
#define __sc_mmap_h

#include <sys/mman.h>  // size_t & off_t

namespace sc {

class posix_mmap
{
    public:
        posix_mmap(bool is_owner=true);

        ~posix_mmap();

        void*  open(size_t length, int prot, int flags, int fd, off_t offset = 0);

        void*  open(const char *file, size_t length);

        void   close();

        void   msync(bool sync = false);

        void   set_owner(bool is_owner)  { is_owner_ = is_owner; }
        void*  addr() const    { return addr_; }
        size_t size() const   { return length_; }

    private:
        bool    is_owner_;
        void*   addr_;
        size_t  length_;
        bool    is_created_;
};

}

#endif
