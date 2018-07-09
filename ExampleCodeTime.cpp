#include <boost/date_time/posix_time/posix_time.hpp>



// 字符串转换为北京时间
boost::posix_time::ptime local_to_utc(const boost::posix_time::ptime & local_tm)
{
    // 字符串转换成ptime 时，需要考虑时区问题。
    boost::posix_time::ptime tm(boost::posix_time::time_from_string("2018-07-06 18:00:00"));

    using CH_ZONE = boost::date_time::local_adjustor<
        boost::posix_time::ptime, +8, boost::posix_time::no_dst>;
    
    return CH_ZONE::local_to_utc(local_tm);
}


