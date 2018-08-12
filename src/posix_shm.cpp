#include <errno.h>  /* errno  */
#include <sys/mman.h>
#include "posix_shm.h"

namespace sc {

posix_shm::posix_shm(bool is_owner): 
    is_owner_(is_owner), is_created_(true), shm_(NULL), shm_key_(0), shm_size_(0), shm_id_(-1)
{
    memset(err_msg_, 0x0, sizeof(err_msg_));
}

posix_shm::posix_shm(key_t shm_key, size_t shm_size, bool is_owner): 
    is_owner_(is_owner), is_created_(true), shm_(NULL), shm_key_(shm_key), shm_size_(shm_size), shm_id_(-1)
{
    memset(err_msg_, 0x0, sizeof(err_msg_));

    get(shm_key, shm_size);
}

posix_shm::~posix_shm()
{
    if(is_owner_) {
       detach(); 
    }
}

int posix_shm::get(key_t shm_key, size_t shm_size)
{
    /* check shm_key_ if existed */
    shm_id_ = shmget(shm_key, shm_size, IPC_CREAT|IPC_EXCL|0666);
    if(shm_id_ < 0)
    {
        is_created_ = false;

        // try to attach
        shm_id_ = shmget(shm_key, shm_size, 0666);
        if(shm_id_ < 0)
        {
            sprintf(err_msg_, "shmget() failed:%s", strerror(errno));
            return -1;
        }
    }
    else
    {
        is_created_ = true;    
    }

    // try to access 
    shm_ = shmat(shm_id_, NULL,  0);
    if((char*)-1 == shm_)
    {
        sprintf(err_msg_, "shmat() failed:%s", strerror(errno));
        shm_ = NULL;
        return -2;
    }

    shm_key_ = shm_key;
    shm_size_= shm_size;

    return 0;
}

bool posix_shm::lock()
{
    if(!shm_) {
        return false;
    }


    if(0!= mlock(shm_, shm_size_)) {
       sprintf(err_msg_, "mlock() failed:%s", strerror(errno));
       return false;
    }
   
    return true;
}

bool posix_shm::unlock()
{
    if(!shm_) {
        return false;
    }

    if(0 != munlock(shm_, shm_size_)) {
       sprintf(err_msg_, "munlock() failed:%s", strerror(errno));
       return false;
    }

    return true;
}

int posix_shm::detach()
{
    int ret = 0;

    if(shm_) {
        ret = shmdt(shm_);
        shm_ = NULL;
    }

    return ret;
}

int posix_shm::destroy()
{
    int ret = 0;

    if(shm_) {
        ret = shmctl(shm_id_, IPC_RMID, 0);
        shm_ = NULL;
    }

    return ret;
}


bool posix_shm::clear_all()
{
    if(!shm_) {
        return false;
    }
    
    memset(shm_, 0x0, shm_size_);

    return true;
}

} /* end of namespace sc */

