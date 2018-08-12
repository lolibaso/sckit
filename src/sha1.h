#ifndef __sc_sha1_h
#define __sc_sha1_h

namespace sc {


class sha1
{
public:
    /*
     *    out[20] 的buffer应为20个字节
     */
    static void hash(unsigned char* out, const unsigned char* in, unsigned int in_len);


    /*
     *    out 的buffer 应为40 + 1字节以上
     */
    static char* hashstr(char* out, const unsigned char* in, unsigned int in_len);
};

}  /* end of namespace sc */

#endif  /* end of __sc_sha1_h */


