#ifndef __sc_crc_h
#define __sc_crc_h
#include <stdint.h>

namespace sc {

class crc
{
public:

    /** 
     * @brief 计算16位 CRC 
     *
     * @param incrc16 16位初始值 
     * @param buf     被计算的数据
     * @param len     被计算的数据长度
     *
     * @return uint16_t  返回16位crc值
     */
    static uint16_t crc16(uint16_t incrc16, const uint8_t* buf, int len);  
    
    /** 
     * @brief 计算32位 CRC 
     *
     * @param incrc32 32位初始值 
     * @param buf     被计算的数据
     * @param len     被计算的数据长度
     *
     * @return uint32_t  返回32位crc值
     */
    static uint32_t crc32(uint32_t incrc32, const uint8_t* buf, int len);

    /** 
     * @brief 计算64位 CRC 
     *
     * @param incrc64 64位初始值 
     * @param buf     被计算的数据
     * @param len     被计算的数据长度
     *
     * @return uint64_t  返回64位crc值
     */
    static uint64_t crc64(uint64_t incrc64, const uint8_t* buf, int len);
};

} // end of namespace sc 


#endif /* end of namespace __sc_crc_h */
