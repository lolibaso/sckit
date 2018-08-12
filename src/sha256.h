#ifndef __sc_sha256_h
#define __sc_sha256_h

namespace sc {

class sha256
{
public:
/*
 *   * pucOut[32] 的buffer应为32个字节
 */
    static void hash(unsigned char* out, const unsigned char* in, int in_len);


/*
 *   * szOut 的buffer 应为64 + 1字节以上
 */
    static char* hashstr(char* out, const unsigned char* in, int in_len);
};

} /* end of namespace sc */

#endif  /* end of __sc_sha256_h */


