#ifndef __sc_str_util_h
#define __sc_str_util_h
#include <stdint.h>
#include <string>
#include <vector>

namespace sc {

class str_util
{
public:        
    static size_t strlcpy(char *dst,const char *src, size_t siz);
    static size_t strlcat(char *dst,const char *src, size_t siz);

    static char* tolower(char* str);

    static char* toupper(char* str);

    static char* trim(char* str);

    static char* trim_left(char* str);

    static char* trim_right(char* str);

    static char* reverse(char* str);

    static char* cat(char* dst, const char* src, ...);

    static char* basename(char* path);

    static int   convert_full2half(const char* input, char* output, int outlen);

    static int   string2hex(const char* in, int len, unsigned char* out);

    static char* hex2string(const unsigned char* in, int len, char* out);

    static uint32_t tohash(const char* str);

    static std::vector<std::string> explode(const std::string& str, const char& ch);
};

}  /* end of namespace sc */

#endif  /* end of __sc_strutil_h */
