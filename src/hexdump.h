#ifndef __sc_hexdump_h
#define __sc_hexdump_h
#include <stdio.h>
#include <stdint.h>

namespace sc {

class hexdump
{
public:
    hexdump(const uint8_t *buf, int buf_len);

    ~hexdump()
    {
        if(membuf_)
        {
            delete [] membuf_;
            membuf_ = NULL;
        }
    }

public:
    const char* get_membuf() const
    {
        return membuf_ ? (const char*) membuf_ : empty_str;
    }

    static void dump_to_file(FILE* fp, const uint8_t* buf, int buf_len);
    static void print(const uint8_t* buf, int buf_len);

private:
    char*   membuf_;
    static  const char empty_str[];
};

}

#endif
