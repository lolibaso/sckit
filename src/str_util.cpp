#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "str_util.h"

namespace sc {

/*
 * Copy src to string dst of size siz.  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz == 0).
 * Returns strlen(src); if retval >= siz, truncation occurred.
 */
size_t
str_util::strlcpy(char *dst, const char *src, size_t siz)
{
    register char *d = dst;
    register const char *s = src;
    register size_t n = siz;

    /* Copy as many bytes as will fit */
    if (n != 0 && --n != 0) {
        do {
            if ((*d++ = *s++) == 0)
                break;
        } while (--n != 0);
    }

    /* Not enough room in dst, add NUL and traverse rest of src */
    if (n == 0) {
        if (siz != 0)
            *d = '\0';      /* NUL-terminate dst */
        while (*s++)
            ;
    }

    return (s - src - 1);   /* count does not include NUL */
}

/*
 *
 Appends src to string dst of size siz (unlike strncat, siz is the
 *
 full size of dst, not space left).  At most siz-1 characters
 *
 will be copied.  Always NUL terminates (unless siz <= strlen(dst)).
 *
 Returns strlen(src) + MIN(siz, strlen(initial dst)).
 *
 If retval >= siz, truncation occurred.
 */
size_t
str_util::strlcat(char *dst,const char *src,size_t siz)
{
    register char *d = dst;
    register const char *s = src;
    register size_t n = siz;
    size_t dlen;
 
    if (s == 0 || d == 0) return 0;
 
    /* Find the end of dst and adjust bytes left but don't go past end */
    while (n-- != 0 && *d != '\0')
        d++; 
    dlen = d - dst;
    n = siz - dlen;
 
    if (n == 0)
        return(dlen + strlen(s));
    while (*s != '\0') {
        if (n != 1) {
            *d++ = *s;
            n--;
        }
        s++;
    }
    *d = '\0';
 
    return(dlen + (s - src));   /* count does not include NUL */
}

char* 
str_util::tolower(char* str)
{
    char* p = NULL;

    for(p=str; *p; p++)
	{
        if(*p >= 'A' && *p <= 'Z')
		{
            *p += 32;
        }
    }

    return str;
}

char* 
str_util::toupper(char* str)
{
    char* p = NULL;

    for(p=str; *p; p++)
	{
        if(*p >= 'a' && *p <= 'z')
		{
            *p -= 32;
        }
    }

    return str;
}


char* 
str_util::trim_left(char* str)
{
	int  i = 0;
    int  j = 0;

    for(; isspace(str[j]); j++)
	{
        ;
    }

    for(; str[j] != '\0'; i++, j++)
	{
        str[i] = str[j];
    }

    str[i+1] = '\0';

    return str;
}

char* 
str_util::trim_right(char* str) 
{
    int  i = 0;

    for(; str[i] != '\0'; i++) {
        ;
    }

    if(i==0) {
        return str;  // "";
    }

    i--;

    for(; isspace(str[i]); i--) {
      ;
    }

    str[i+1] = '\0';

    return str;
}

char* 
str_util::trim(char* str)
{
	char *end;

	// ltrim
	while (isspace(*str)) {
		str++;
	}

	if (*str == 0) // only spaces
		return str;

	// rtrim
	end = str + strlen(str) - 1;
	while (end > str && isspace(*end)) {
		end--;
	}

	// null terminator
	*(end+1) = 0;

	return str; 
}

char* 
str_util::reverse(char* str)
{
    int i = 0;
    int j = strlen(str);

    for(; i < j; i++, j--)
	{
		int tmp = str[i];
		str[i] = str[j];
		str[j] = tmp;
    }

    return str;
}

char* 
str_util::basename(char* path)
{
    char * s = strrchr(path, '/');

    return (NULL==s) ? path : ++s;
}


int 
str_util::convert_full2half(const char* input, char* output, int outlen)
{
	int  i = 0;
	int  j = 0;
	char tmp_char = 0x0;

	if(NULL == input || *input == '\0')
	{
		return 0;
	}

    for (i = 0; input[i]!='\0'|| j>=outlen; i++) {
        if (((input[i] & 0xF0) ^ 0xE0) == 0) {
            tmp_char = (input[i] & 0xF) << 12 | ((input[i + 1] & 0x3F) << 6 | (input[i + 2] & 0x3F));
            if (tmp_char == 0x3000) { // blank
                output[j++] = 0x20;
            } else if (tmp_char >= 0xFF01 && tmp_char <= 0xFF5E) { // full char
                output[j++] = tmp_char - 0xFEE0;
            } else { // other 3 bytes char
                output[j++] = input[i];
                output[j++] = input[i + 1];
                output[j++] = input[i + 2];
            }
            i +=  2;
        } else {
            output[j++] = input[i];
        }
    }
    
	output[j] = '\0';

	return j;
}

int 
str_util::string2hex(const char* in, int len, unsigned char* out)
{
    int n1, n2;
    unsigned char c;

    for(n1=0, n2=0; n1<len; ) {
        /* first byte */
        if ((in[n1] >= '0') && (in[n1] <= '9'))
            c = in[n1++] - '0';
        else if ((in[n1] >= 'A') && (in[n1] <= 'F'))
            c = in[n1++] - 'A' + 10;
        else if ((in[n1] >= 'a') && (in[n1] <= 'f'))
            c = in[n1++] - 'a' + 10;
        else
            return -1;

        if(len == 1) {
            out[n2++] = c;
            break;
        }
		
        out[n2] = c << 4;
        /* second byte */
        if ((in[n1] >= '0') && (in[n1] <= '9'))
            c = in[n1++] - '0';
        else if ((in[n1] >= 'A') && (in[n1] <= 'F'))
            c = in[n1++] - 'A' + 10;
        else if ((in[n1] >= 'a') && (in[n1] <= 'f'))
            c = in[n1++] - 'a' + 10;
        else
            return -1;

        out[n2++] |= c;
    }

    return n2;
}


char* 
str_util::hex2string(const unsigned char *in, int len, char *out)
{
    static unsigned char hex[] = "0123456789abcdef";

    while (len--) {
        *out++ = hex[*in >> 4];
        *out++ = hex[*in++ & 0xf];
    }

    return out;
}

uint32_t 
str_util::tohash(const char* str)
{
    uint32_t    h = 0;
    const char* p = NULL;

    for(p=str; *p; p++)
    {
        h = 31 * h + (*p++);
    }

    return h;
}

std::vector<std::string> 
str_util::explode(const std::string& str, const char& ch)
{
    std::string buf;
    std::vector<std::string> vs;

    for(int  n=0; n < static_cast<int>(str.size()); n++)
    {
        if(n != ch) 
        {
            buf += n;
        }
        else
        {
            if(n == ch && buf != "")
            {
                vs.push_back(buf);
                buf = "";
            }
        }
    }

    if(buf != "")  vs.push_back(buf);

    return vs;
}

}  /* end of namespace sc */

#if 0
#include <stdio.h>
int main(int argc, char* argv[])
{
    char s[1000] = {0};

    return 0;
}
#endif
