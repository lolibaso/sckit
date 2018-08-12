#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "time_util.h"

namespace sc {

/*
 * 线程安全， mktime() 不是线程安全
 * 更快，没有调用 tzset
 * 固定了北京的时区
 */

time_t time_util::mktime_bj(const struct tm* timeptr)
{
	static const int mon_days[] =
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	//判断闰年，tm_year是从1900开始，1970年不是闰年，1968年是闰年
	long leap_years = (timeptr->tm_year - 68) / 4 - (timeptr->tm_year / 100) + (timeptr->tm_year + 300) / 400;
	if (timeptr->tm_mon < 2) {
		// 如果是1、2月，且今年是闰年，则前面leap_years多算了一天
		long year = timeptr->tm_year + 1900;
		if ((year % 100 != 0 && year % 4 == 0) || (year % 400 == 0)) --leap_years;
	}

	//tm_year是从1900开始，但时间戳从1970年开始算起
	long days = (timeptr->tm_year - 70) * 365L + leap_years + timeptr->tm_mday - 1;
	for (int i = 0; i < timeptr->tm_mon; ++i) {
		days += mon_days[i];
	}

    //这里已知是北京时间，所以写死28800
	return days * 86400 + timeptr->tm_hour * 3600 + timeptr->tm_min * 60 + timeptr->tm_sec - (8*3600);
}

struct tm* time_util::curdatetime(struct tm* ptm)
{
    time_t now = time(NULL);
    localtime_r(&now, ptm);
    return ptm;
}

time_t time_util::now()
{
    return time(NULL);
}

int64_t time_util::now_msec(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (int64_t)((int64_t)tv.tv_sec*1000 + (int64_t)tv.tv_usec/1000);
}

int64_t time_util::now_usec(void)
{
    struct timeval tv;
	gettimeofday(&tv, NULL);
	return (int64_t)((int64_t)tv.tv_sec*1000000 + (int64_t)tv.tv_usec);
}

struct timeval conv2timeval(int64_t time_us)
{
    struct timeval tv;
    tv.tv_sec = (long)time_us / 1000000;
    tv.tv_usec = (long)time_us % 1000000;

    return tv;
}

long time_util::timediff_msec(struct timeval* tv1, struct timeval* tv2)
{
	long diff_time = 0;

	diff_time = (tv1->tv_sec - tv2->tv_sec) * 1000 + (tv1->tv_usec - tv2->tv_usec) / 1000 ;

	return diff_time;
}

double time_util::timediff_usec(struct timeval* tv1, struct timeval* tv2)
{
	double diff_time = 0;

	diff_time = (tv1->tv_sec - tv2->tv_sec) * 1000000 + (tv1->tv_usec - tv2->tv_usec) ;

	return diff_time;
}


char* time_util::datetime2str(time_t t, char* time_str, int len)
{
	struct tm* ptm = localtime(&t);

	if(ptm->tm_year > 50)
    {
		snprintf(time_str, len-1, "%04d-%02d-%02d %02d:%02d:%02d",
					ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
					ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	}
	else
    {
		snprintf(time_str, len-1, "%04d-%02d-%02d %02d:%02d:%02d",
					ptm->tm_year+2000, ptm->tm_mon+1, ptm->tm_mday,
					ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	}

	return time_str;
}

char* time_util::datetime2str_short(time_t t, char* time_str, int len)
{
	struct tm* ptm = localtime(&t);

	if(ptm->tm_year > 50) {
		snprintf(time_str, len-1, "%04d%02d%02d%02d%02d%02d",
					ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
					ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	}
	else {
		snprintf(time_str, len-1, "%04d%02d%02d%02d%02d%02d",
					ptm->tm_year+2000, ptm->tm_mon+1, ptm->tm_mday,
					ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	}

	return time_str;
}

char* time_util::curdatetimestr(char* time_str, int len)
{
    struct tm res;

    curdatetime(&res);

    if(res.tm_year > 50) {
        snprintf(time_str, len-1, "%04d-%02d-%02d %02d:%02d:%02d",
                    res.tm_year+1900, res.tm_mon+1, res.tm_mday,
                    res.tm_hour, res.tm_min, res.tm_sec);
    }
    else {
        snprintf(time_str, len-1, "%04d-%02d-%02d %02d:%02d:%02d",
                    res.tm_year+2000, res.tm_mon+1, res.tm_mday,
                    res.tm_hour, res.tm_min, res.tm_sec);
    }

    return time_str;
}

char* time_util::curdatetimestr_short(char* time_str, int len)
{
    struct tm res;

    curdatetime(&res);

    if(res.tm_year > 50) {
        snprintf(time_str, len-1, "%04d%02d%02d%02d%02d%02d",
                    res.tm_year+1900, res.tm_mon+1, res.tm_mday,
                    res.tm_hour, res.tm_min, res.tm_sec);
    }
    else {
        snprintf(time_str, len-1, "%04d%02d%02d%02d%02d%02d",
                    res.tm_year+2000, res.tm_mon+1, res.tm_mday,
                    res.tm_hour, res.tm_min, res.tm_sec);
    }

    return time_str;
}


char* time_util::curdatestr(char* time_str, int len)
{
    struct tm res;

    curdatetime(&res);

    if(res.tm_year > 50) {
        snprintf(time_str, len-1, "%04d-%02d-%02d",
                    res.tm_year+1900, res.tm_mon+1, res.tm_mday);
    }
    else {
        snprintf(time_str, len-1, "%04d-%02d-%02d",
                    res.tm_year+2000, res.tm_mon+1, res.tm_mday);
    }

    return time_str;
}

char* time_util::curdatestr_short(char* time_str, int len)
{
    struct tm res;

    curdatetime(&res);

    if(res.tm_year > 50) {
        snprintf(time_str, len-1, "%04d%02d%02d",
                    res.tm_year+1900, res.tm_mon+1, res.tm_mday);
    }
    else {
        snprintf(time_str, len-1, "%04d%02d%02d",
                    res.tm_year+2000, res.tm_mon+1, res.tm_mday);
    }

    return time_str;
}

char* time_util::curtimestr(char* time_str, int len)
{
    struct tm res;

    curdatetime(&res);

    snprintf(time_str, len-1, "%02d:%02d:%02d", res.tm_hour, res.tm_min, res.tm_sec);

    return time_str;
}

char* time_util::curtimestr_short(char* time_str, int len)
{
    struct tm res;

    curdatetime(&res);

    snprintf(time_str, len-1, "%02d%02d%02d", res.tm_hour, res.tm_min, res.tm_sec);

    return time_str;
}


/*  sscanf is a little slower
 *  see the faster' method below
 *
time_t time_util::Str2Datetime(const char* strDateTime)
{
	struct tm 	_tm;

	const  char* ptr = strDateTime;

	// skip space
	// while(isspace(*ptr)) {
	//	 ++ptr;
	// }

	if(6 != sscanf(ptr, "%d-%d-%d %d:%d:%d",
			&_tm.tm_year, &_tm.tm_mon, &_tm.tm_mday,
			&_tm.tm_hour, &_tm.tm_min, &_tm.tm_sec) )
    {
		return 0;
	}

	_tm.tm_mon--;
	_tm.tm_year -= 1900;
	_tm.tm_isdst = -1;

	return mktime(&_tm);
}
*/

time_t time_util::str2datetime(const char* datetime)
{
    struct tm tt = { 0,  0,  0,
                     0,  0,  0,
                     0,  0,  0,
                     0,  0
                   };

    int *ptr[] = {&tt.tm_year, &tt.tm_mon, &tt.tm_mday, &tt.tm_hour, &tt.tm_min, &tt.tm_sec};
    char dlm[] = {'-','-',' ',':',':'};
    unsigned pos = 0;
    unsigned n = 0;

    for(; datetime[n] != '\0'; ++n) 
    {
        if(datetime[n] >= '0' && datetime[n] <= '9') {
        //  补0    2017-1-1 1:1:1 == 2017-01-01 01:01:01
            *(ptr[pos]) = *(ptr[pos]) * 10 + datetime[n] - '0';
        }
        else {
            if(pos >= sizeof(dlm) || dlm[pos] != datetime[n])
            {
                break;
            }
            ++pos;
        }
    }

    tt.tm_year -= 1900;
    tt.tm_mon -= 1;
	tt.tm_isdst = -1;

    return  mktime(&tt);
}


void time_util::normalize_time(struct timeval* ptv)
{
	if (ptv->tv_usec >= 1000000) {
		do {
			ptv->tv_sec++;
			ptv->tv_usec -= 1000000;
		}while(ptv->tv_usec >= 1000000);
	}
	else if(ptv->tv_usec <= -1000000) {
		do {
			ptv->tv_sec--;
			ptv->tv_usec += 1000000;
		}while(ptv->tv_usec <= -1000000);
	}

	if(ptv->tv_sec >= 1 && ptv->tv_usec < 0) {
		ptv->tv_sec--;
		ptv->tv_usec += 1000000;
	}

	return;
}


bool time_util::is_leapyear(int year)
{
    return ( (year%400==0) || ((year%4==0)&&(year%100!=0)) );
}

int time_util::daysofmonth(int year, int month)
{

	switch (month)
    {
	case 2:
		{
            if(sc::time_util::is_leapyear(year))
                return 29;
            else
                return 28;
            break;
		}
	case 4:
	case 6:
	case 9:
	case 11:
		return 30;
	default:  // 1 3 5 7 8 10 12
		return 31;
	}

	return 31;
}

} /* end of namespace sc */

#if 0

int main(int argc, char* argv[])
{
	struct timeval  tv1, tv2;
	gettimeofday(&tv1, NULL);
	time_t t = time(NULL);
	int i = 0;
	//int x = atoi(argv[1]);

	while(i < 1000*1000) {
        sc::Str2Datetime(argv[1]);
		i++;
	}
	gettimeofday(&tv2, NULL);

    printf("- %lu\n", sc::Str2Datetime(argv[1]));

    /*
	printf("%ld ms\n",  sc::Timediff_Msec(&tv2, &tv1));
	printf("month %d has %d days \n",  x, sc::DaysOfMonth(2016, x));
    */


	return 0;
}
#endif
