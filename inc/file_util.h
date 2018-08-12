#ifndef __sc_file_util_h
#define __sc_file_util_h

#include <stdint.h>
#include <vector>
#include <string>

namespace sc {

class file_util 
{
public:
    /**
     * @brief 获取文件大小
     *
     * @param filepath  文件路径
     *
     * @return uint64_t 文件大小
     */
    static uint64_t get_filesize(const char* filepath);

    /**
     * @brief 判断文件是否存在
     *
     * @param filepath 文件路径
     *
     *
     * @return bool true 存在，false 不存在
     */
    static bool     file_exist(const char* filepath);

    /**
     * @brief 从文件路径里取文件名
     *
     * @param filepath 文件路径
     *
     * @return 指向文件名的指针
     */
    static const char*    get_filename(const char* filepath);

    /**
     * @brief 从文件路径里取文件扩展名
     *
     * @param filepath 文件路径
     *
     * @return 指向文件扩展的指针
     */
    static const char*    get_fileext(const  char* filepath);

    /**
     * @brief 从文件路径里取文件目录
     *
     * @param filepath 文件路径
     *
     * @return 指向文件目录的指针
     */
    static const char*    get_filepath(const char* filepath);

    /**
     * @brief 从文件里读取内容到缓冲
     *
     * @param filepath  文件路径
     * @param buf       存储缓冲 
     * @param buf_len   存储缓冲大小
     *
     * @return int64_t  读取了多大的数据大小
     */
    static int64_t  get_filecontent(const char* filename, void* buf, int64_t buf_len);

    /**
     * @brief 从缓冲里的内容写到文件
     *
     * @param filepath  文件路径
     * @param buf       存储缓冲 
     * @param buf_len   缓冲大小
     *
     * @return int64_t  写入了多大的数据大小
     */
    static int64_t  write_filecontent(const char* filename, const void* buf, int64_t buf_len);

    /*
     * @brief 从指定目录读取符合匹配模式的所有文件名并存储
     */
    static int get_files_fromdir(const char* dirpath, 
                                 const char* pattern, 
                                 std::vector<std::string>& files_list);

	   /**
     * @brief  把 进程id写入文件，方便管理
     *
     * @param  pidfile   保存进程id的文件路径, 格式是： 12345\n  后面带一个换行符，方便提取
     *
     * @return int       成功返回0，如果失败,返回负数
     */

    static int writepid_tofile(const char* pidfile);

    /**
     * @brief 从文件里读出 进程id
     *
     * @param 进程id 文件路径
     *
     * @return int      返回进程id，如果失败，返回负数
     */

    static int getpid_fromfile(const char* pidfile);

    static int read_fromfd(int fd, void* buf, int buf_len);
    static int write_tofd(int fd, const void* buf, int buf_len);
}; 

} // end of namespace sc 

#endif 
