#include <boost/date_time/posix_time/posix_time.hpp>
#include <chrono>

// 字符串转换为北京时间
boost::posix_time::ptime local_to_utc(const boost::posix_time::ptime & local_tm)
{
    // 字符串转换成ptime 时，需要考虑时区问题。这里的字符 串时间，boost 内部按第0 时区处理的。
    // 所以需要转换成北京在这个时间的UTC 时刻值(ptime 结构)。
    boost::posix_time::ptime tm(boost::posix_time::time_from_string("2018-07-06 18:00:00"));

    using CH_ZONE = boost::date_time::local_adjustor<
        boost::posix_time::ptime, +8, boost::posix_time::no_dst>;
    
    return CH_ZONE::local_to_utc(local_tm);
}

// ptime 结构
void ptime_code()
{
    boost::posix_time::ptime tm_local = boost::posix_time::second_clock::local_time();            // 本地时间
    boost::posix_time::ptime tm_utc   = boost::posix_time::second_clock::universal_time();        // UTC 时间
}


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

