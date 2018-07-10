#include <chrono>

void current_time_01()
{
    // 当前时间点
    std::chrono::system_clock::time_point t_now =
        std::chrono::high_resolution_clock::now();
    // 转换精度到微秒级别
    std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds> t_n = 
        std::chrono::time_point_cast<std::chrono::microseconds>(t_now);

    time_t nt = std::chrono::system_clock::to_time_t(t_now);
    struct tm btm;
#ifdef _WIN32
    localtime_s(&btm, &nt);
#else
    localtime_r(&nt, &btm);
#endif

    char mbstr[256] = { 0 };
    strftime(mbstr, sizeof(mbstr), "%Y-%m-%d %H:%M:%S", &btm);
    printf("%s.%06" PRIu64 "\n", mbstr, t_n.time_since_epoch() % 1000000);
}

#include <sys/timeb.h>

char* cur_time_02(char strDateTime[32])
{
    struct timeb tp_cur;
    
    ftime(&tp_cur);
    
    struct tm btm;
    
#ifdef _WIN32
    localtime_s(&btm, &tp_cur.time);
#else
    localtime_r(&tp_cur.time, &btm);
#endif
    
    sprintf(strDateTime, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
            btm.tm_year + 1900, btm.tm_mon + 1, btm.tm_mday,
            btm.tm_hour, btm.tm_min, btm.tm_sec, 
            tp_cur.millitm);
    
    return strDateTime;
}

extern char* cur_time_03(char strDateTime[32])
{
    static const std::chrono::hours EIGHT_HOURS(8);
    
    // +8 小时转换为北京时间
    auto nowLocalTimeCount = std::chrono::system_clock::now().time_since_epock() + EIGHT_HOURS;
    
    std::chrono::hours now_h = std::chrono::duration_cast<
        std::chrono::hours>(nowLocalTimeCount);
    std::chrono::minutes now_m = std::chrono::duration_cast<
        std::chrono::minutes>(nowLocalTimeCount);
    std::chrono::seconds now_s = std::chrono::duration_cast<
        std::chrono::seconds>(nowLocalTimeCount);
    std::chrono::milliseconds now_ms = std::chrono::duration_cast<
        std::chrono::milliseconds>(nowLocalTimeCount);
    
    sprintf(strDateTime, "%02d:%02d:%02d.%03d",
            now_h % 24, now_m % 60, now_s % 60, now_ms % 1000);
    
    return strDatetime;
}

extern char* cur_time_04(char strDateTime[64])
{
    boost::posix_time::ptime tm_msloc = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration td_msloc = tm_msloc.time_of_day(); // 获取时间部分
    
    sprintf(strDateTime, "%02d:%02d:%02d.%03d",
            td_msloc.hours(), td_msloc.minutes(), td_msloc.seconds(), 
            td_msloc.fractional_seconds / 1000);
    
    return strDateTime;
}







