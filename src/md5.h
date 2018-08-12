#ifndef __sc_md5_h
#define __sc_md5_h

namespace sc {

class md5
{
public:
    static void hash(unsigned char* out, const unsigned char* in, int in_len);


    /*
     *   char out[32+1];   out[32]= 0x0;
     */
    static char* hashstr(char* out, const unsigned char* in, int in_len);
};

}  /* end of namespace sc */

#endif  /* end of __sc_md5_h */


