#ifndef __sc_sha1_h
#define __sc_sha1_h

namespace sc {


class sha1
{
public:
    /*
     *    out[20] ��bufferӦΪ20���ֽ�
     */
    static void hash(unsigned char* out, const unsigned char* in, unsigned int in_len);


    /*
     *    out ��buffer ӦΪ40 + 1�ֽ�����
     */
    static char* hashstr(char* out, const unsigned char* in, unsigned int in_len);
};

}  /* end of namespace sc */

#endif  /* end of __sc_sha1_h */


