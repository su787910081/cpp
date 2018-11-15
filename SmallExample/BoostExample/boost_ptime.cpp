

void f1() 
{
    boost::date_time::special_values MIN(boost::date_time::special_values::min_date_time);
    // 通过一个时刻得到表示该时刻的整数值
    boost::posix_time::ptime pt_uni = boost::posix_time::microsec_clock::universal_time();
    boost::posix_time::ptime pt_min(MIN);
    boost::posix_time::time_duration tdCur = pt_uni - pt_min;
    int64_t milli_seconds = tdCur.total_milliseconds();
    
    // 得到一个时间点: msTotal --> ptim
    // 通过一个表示某一时刻的毫秒整数值，来得到该时刻的ptime 结构
    boost::gregorian::date dt(MIN);
    boost::posix_time::ptime pt(dt, boost::posix_time::milliseconds(milli_seconds));
}
