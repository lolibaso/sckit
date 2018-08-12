/*
 *  from boost::noncopyable
 */
#ifndef __sc_noncopyable_h
#define __sc_noncopyable_h

namespace sc {
namespace noncopyable_
{
class noncopyable
{
    protected:
        noncopyable() {}
        ~noncopyable() {}

    private:
        noncopyable(const noncopyable&);
        const noncopyable& operator = (const noncopyable&);
}; 
}

typedef noncopyable_::noncopyable  noncopyable;

} // namespace sc 

#endif 
