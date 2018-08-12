#include "hexdump.h"
#include <string.h>

namespace sc 
{

//#define LINE_WIDTH 16

const char hexdump::empty_str[] = "Null";

hexdump::hexdump(const uint8_t *buf, int buf_len) : membuf_(NULL)
{
	register int i;		// ptr into source buffer
	register int j;		// pair elements counter
	register int k;		// pairs counter [0;16[
	
	const uint8_t *p;		// ptr into source buffer
	char *hex;			// hex ptr into destination buffer
	char *ascii;		// ascii ptr into destination buffer
	
	long final_len;

	/*--- Precondition ---	*/
	if (buf_len <= 0 || buf == NULL) 
	{
		return;
	}
	j = k = 1;

	/*---
	  Each row holds 16 bytes of data. It requres 74 characters maximum.
	  Here's some examples:

0         1         2         3         4         5         6         7
0123456789012345678901234567890123456789012345678901234567890123456789012
-------------------------------------------------------------------------
3132 3037 3039 3039 3031 3130 3839 3033  1207090901108903
3038 3132 3030 3331 3030 0d0a 3839 3033  0812003100\r\n8903
0d0a 0d0a 0d0a 0d0a 0d0a 0d0a 0d0a 0d0a  \r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n

      If all 16 bytes are control characters, the ASCII representation
	  will extend line to 72 characters plus cartrige return and line 
	  feed at the end of the line.

	  If buf_len is not multiple of 16, we add one more row and another row
	  just to be on a safe side.
	  ---*/
	
	final_len = (int (buf_len/16) + 1 + (buf_len % 16 ? 1:0)) * 74;
	
	membuf_ = new char[final_len];
	memset (membuf_, ' ', final_len);
	
	p     = buf;				// ptr to original image
	hex   = membuf_;				// current ptr to hex image
	ascii = membuf_ + 41;		// current ptr to ascii image
	
	for (i = 0; i < buf_len; i++) 
	{
		sprintf(hex,"%01x%01x", p[i] >> 4 & 0x0f, p[i] & 0x0f); 
		hex+=2;

		if      (p[i] == '\n') { sprintf(ascii,"\\n"); ascii+=2; }
		else if (p[i] == '\t') { sprintf(ascii,"\\t"); ascii+=2; }
		else if (p[i] == '\v') { sprintf(ascii,"\\v"); ascii+=2; }
		else if (p[i] == '\b') { sprintf(ascii,"\\b"); ascii+=2; }
		else if (p[i] == '\r') { sprintf(ascii,"\\r"); ascii+=2; }
		else if (p[i] == '\f') { sprintf(ascii,"\\f"); ascii+=2; }
		else if (p[i] == '\a') { sprintf(ascii,"\\a"); ascii+=2; }
		else if (p[i] == '\0') { sprintf(ascii,"\\0"); ascii+=2; }
		else {
			sprintf (ascii++,"%c", ((p[i] < ' ' || p [i] > '~') ? '.' : p [i]));
		}
		
		if (!(j++ % 2)) {
			sprintf (hex++," ");
		}

		k %= 16;

		if (!(k++)) {
			*hex = ' ';
			sprintf (ascii++,"\n");
			hex = ascii;
			ascii +=  41;
		}
	}
	*hex = ' ';
	membuf_[final_len-1] = '\0';

}

void 
hexdump::dump_to_file(FILE* fp, const uint8_t* buf, int buf_len)
{
	if(NULL == fp || buf_len <= 0 || NULL == buf)
	    return;

	hexdump  tmp(buf, buf_len);

    fprintf(fp,"%s",  tmp.get_membuf());
}

void 
hexdump::print(const uint8_t* buf, int buf_len)
{
    dump_to_file(stderr, buf, buf_len);
    fprintf(stderr, "\n");
}

/*
static void put_line(FILE *fd, const unsigned char *ptr, buf_len_t buf_len) 
{
  fprintf(fd, "%p: ", ptr);

  for (buf_len_t i = 0; i < buf_len; i++) {
    fprintf(fd, "%02x ", ptr[i]);
  }

  for (buf_len_t i = 0; i < LINE_WIDTH - buf_len; i++) {
    fprintf(fd, "   ");
  }

  for (buf_len_t i = 0; i < buf_len; i++) {
    char c = ptr[i];

    if (c < ' ' || c > '~')
      c = '.';

    fprintf(fd, "%c", c);
  }

  fprintf(fd, "\n");
}

void hexdump(FILE *fd, const unsigned char *ptr, int buf_len) 
{
  int lines = buf_len / LINE_WIDTH;

  for (int i = 0; i < lines; i++) {
    put_line(fd, ptr + i * LINE_WIDTH, LINE_WIDTH);
  }

  if (lines * LINE_WIDTH < buf_len)
    put_line(fd, ptr + lines * LINE_WIDTH, buf_len - lines * LINE_WIDTH);
}
*/

}

#if 1
int main()
{
    char aaa[]="13kjdsfjakdfajj;l!#!#^%%*$($)(!)-dsfadfasffffffffffa;fdsfda\n\n\n\n\r\r\r\v\t\t\t"; 

    sc::hexdump::dump_to_file(stderr, (uint8_t*)aaa, strlen(aaa));

    return 0;
}
#endif
