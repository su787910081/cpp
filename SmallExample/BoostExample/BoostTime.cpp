#include <boost/date_time/posix_time/posix_time.hpp>


extern char* cur_time_04(char strDateTime[64])
{
    boost::posix_time::ptime tm_msloc = boost::posix_time::microsec_clock::local_time();
    
    auto&& d_msloc = tm_msloc.date();    // 日期部分
    boost::posix_time::time_duration td_msloc = tm_msloc.time_of_day(); // 获取时间部分
    
    sprintf(strDateTime, "%4d-%02d-%02d %02d:%02d:%02d.%03d",
            d_msloc.year(), d_msloc.month().as_number(), d_msloc.day(),     // 这里的月份不用额外 +1 
            td_msloc.hours(), td_msloc.minutes(), td_msloc.seconds(), 
            td_msloc.fractional_seconds / 1000);
    
    return strDateTime;
}


// 字符串转换为北京时间
boost::posix_time::ptime local_to_utc()
{
    // 字符串转换成ptime 时，需要考虑时区问题。这里的字符 串时间，boost 内部按第0 时区处理的。
    // 所以需要转换成北京在这个时间的UTC 时刻值(ptime 结构)。
    boost::posix_time::ptime tm(boost::posix_time::time_from_string("2018-07-06 18:00:00"));

    using CH_ZONE = boost::date_time::local_adjustor<
        boost::posix_time::ptime, +8, boost::posix_time::no_dst>;
    
    return CH_ZONE::local_to_utc(tm);
}


// ptime 结构
void ptime_code()
{
    boost::posix_time::ptime tm_sloc = boost::posix_time::second_clock::local_time();            // 本地时间
    boost::posix_time::ptime tm_sutc = boost::posix_time::second_clock::universal_time();        // UTC 时间
    boost::posix_time::ptime tm_msloc = boost::posix_time::microsec_clock::local_time();
	boost::posix_time::ptime tm_msutc = boost::posix_time::microsec_clock::universal_time();
    
    auto&& d_msloc = tm_msloc.date();    // 日期部分
    boost::posix_time::time_duration td_msloc = tm_msloc.time_of_day(); // 获取时间部分
    uint64_t l = td_msloc.fractional_seconds();   // 获取秒的小数部分
}
