#ifndef __sc_macro_func_h
#define __sc_macro_func_h
#include <string.h> /* memset , strerror */

#define  BZERO(buf, size)  memset((buf), 0x0, (size));
#define  COUNT_OF(a)    (sizeof(a)/sizeof(a[0]))
#define  IS_ODD(a)       (((unsigned int)(a))& 1)
#define  IS_EVEN(a)      !(isodd(a)) 
#define  ROUNDUP(x, r)  (((x)%(r))?((x)+((r)-((x)%(r)))):(x))

#define  SAFE_FREE(x)         do { if(NULL != x) { free(x); (x) = NULL;} } while(0)
#define  SAFE_DELETE(x)       do { if(NULL != x) { delete(x); (x) = NULL;} } while(0)
#define  SAFE_DELETE_ARR(x)   do { if(NULL != x) { delete [] (x); (x) = NULL;} } while(0)

#define  HI_UINT8(a) (((a) >> 4) & 0x0F)
#define  LO_UINT8(a) ((a) & 0x0F)
#define  HI_UINT16(a) (((a) >> 8) & 0xFF)
#define  LO_UINT16(a) ((a) & 0xFF)

#define MIN(x,y)  (((x)<(y))?(x):(y))
#define MAX(x,y)  (((x)>(y))?(x):(y)) 
#define MIN3(x,y,z) ((x)<(y) ? MIN(x,z) : MIN(y,z))
#define MAX3(x,y,z) ((x)>(y) ? MAX(x,z) : MAX(y,z))

 
#define  BUILD_UINT8(hiByte, loByte)   ((uint8_t)(((loByte) & 0x0F) + (((hiByte) & 0x0F) << 4)))
#define  BUILD_UINT16(loByte, hiByte ) ((uint16_t)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF ) << 8)))
#define  BUILD_UINT32(Byte0, Byte1, Byte2, Byte3)  ((uint32_t)((uint32_t)((Byte0) & 0x00FF)         \
                                                    + ((uint32_t)((Byte1) & 0x00FF) << 8)           \
                                                    + ((uint32_t)((Byte2) & 0x00FF) << 16)          \
                                                    + ((uint32_t)((Byte3) & 0x00FF) << 24)))        

// Takes a byte out of a uint32_t : var - uint32_t,  ByteNum - byte to take out ( 0 - 3 ) 
#define  BREAK_UINT32(var, ByteNum)    (uint8_t)((uint32_t)(((var) >> ((ByteNum) * 8)) & 0x00FF))
 
// Write the 32bit value of 'val' in little endian format to the buffer pointed 
// // to by pBuf, and increment pBuf by 4
#define  UINT32_TO_BUF_LITTLE_ENDIAN(pBuf,val) \
	do { \
		*(pBuf)++ = (((val) >>  0) & 0xFF); \
		*(pBuf)++ = (((val) >>  8) & 0xFF); \
		*(pBuf)++ = (((val) >> 16) & 0xFF); \
		*(pBuf)++ = (((val) >> 24) & 0xFF); \
	} while (0)
 
// Return the 32bit little-endian formatted value pointed to by pBuf, and increment pBuf by 4
#define  BUF_TO_UINT32_LITTLE_ENDIAN(pBuf) (((pBuf) += 4), BUILD_UINT32((pBuf)[-4], (pBuf)[-3], (pBuf)[-2], (pBuf)[-1]))


 
#define  SET_BIT(arr,bit)   ((arr[bit >> 3] |= (unsigned char)(1 << (bit & 0x7))))
#define  CLEAR_BIT(arr,bit) ((arr[bit >> 3] &= (unsigned char)(~(1 << (bit & 0x7)))))
#define  CHECK_BIT(arr,bit) ((arr[bit >> 3] & (unsigned char)(1 << (bit & 0x7))) != 0)

#endif // end of __sc_macro_func_h
