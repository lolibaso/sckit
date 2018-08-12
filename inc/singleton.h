#ifndef __sc_singleton_h
#define __sc_singleton_h

namespace sc
{
    template<class T>  class create_by_new
    {
    public:
        static T* instance(T* Proto) {
            return new T;
        }
        static void destroy(T* obj) {
            delete obj;
        }
    };


    template<class T>  class create_by_proto
    {
    public:
        static T* instance(T* Proto) {
            return Proto;
        }

        static void destroy(T* obj) {
            return;
        }
    };


    template < class T, template<class> class create_policy = create_by_new  > class  singleton
    {
    public:

        static T* instance() {
            if (instance_ == NULL) {
                instance_ = create_policy<T>::instance(protoinstance_);
            }

            return  instance_;
        }

        static void destroy() {
            create_policy<T>::destroy(instance_);
        }

        static void setproto(T* proto) {
            instance_ = NULL;
            protoinstance_ = proto;
        }

        static T*  instance_;
        static T*  protoinstance_;
    private:
        singleton(void);
        singleton(const singleton&);
        singleton& operator= (const singleton&);
    };

    template <class T, template <class> class creation_policy>
    T* singleton <T, creation_policy >::instance_ = NULL;

    template <class T, template <class> class creation_policy>
    T* singleton <T, creation_policy >::protoinstance_ = NULL;
}  // end of namespace sc

#endif  // end of __sc_singleton_h

