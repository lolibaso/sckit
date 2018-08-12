#ifndef __sc_base64_h
#define __sc_base64_h

namespace sc {

class base64
{
public:
   static int encode(char* out, int& out_len, const unsigned char* in, int in_len); 

   static int decode(unsigned char* out, int& out_len, const char* in, int in_len);

}; /* end of struct Base64 */

}  /* end of namespace sc */

#endif /* end of __sc_base64_h */

