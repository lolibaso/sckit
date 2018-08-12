#ifndef __sc_shm_h
#define __sc_shm_h

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>

namespace sc {

class posix_shm
{
    public:
        posix_shm(bool is_owner = false);
        posix_shm(key_t shm_key, size_t shm_size, bool is_owner = false);
        ~posix_shm();

    public:
        int     get(key_t shm_key, size_t shm_size);
        bool    is_created() const { return is_created_; }

        void*   shm()  const { return shm_; }
        size_t  size() const { return shm_size_; }
        key_t   key()  const { return shm_key_; }
        int     shmid()   const { return shm_id_; }

        const char* get_errmsg()     { return err_msg_; }

        bool    lock();
        bool    unlock();

        int     detach();
        int     destroy();
        bool    clear_all();

    private:
        bool     is_owner_;
        bool     is_created_;
        void*    shm_;
        key_t    shm_key_;
        size_t   shm_size_;
        int      shm_id_;
        char     err_msg_[512];
};


} /* end of namespace sc */

#endif /* end of __SC_SHM_H */
