#ifndef __sc_time_util_h
#define __sc_time_util_h

namespace sc {

class time_util
{
public:
    static time_t  mktime_bj(const struct tm * timeptr);

    static time_t  now(void);

    static int64_t now_msec(void);

    static int64_t now_usec(void);

    static struct  timeval conv2timeval(int64_t time_us);
    
    static long    timediff_msec(struct timeval* tv1, struct timeval* tv2);

    static double  timediff_usec(struct timeval* tv1, struct timeval* tv2);

    static struct tm* curdatetime(struct tm* ptm);

    static char*   curdatetimestr(char* time_str, int len);
    
    static char*   datetime2str(time_t t, char* time_str, int len);

    static char*   datetime2str_short(time_t t, char* time_str, int len);

    static char*   curdatetimestr_short(char* time_str, int len);

    static char*   curdatestr(char* time_str, int len);

    static char*   curdatestr_short(char* time_str, int len);
    
    static char*   curtimestr(char* time_str, int len);

    static char*   curtimestr_short(char* time_str, int len);

    static time_t  str2datetime(const char* strdatetime);

    static void    normalize_time(struct timeval* ptv);

    static int     daysofmonth(int iyear, int imonth);

    static bool    is_leapyear(int year);

}; 


} // end of namespace sc 


#endif  /* end of __SC_TIME_UTIL_H */
