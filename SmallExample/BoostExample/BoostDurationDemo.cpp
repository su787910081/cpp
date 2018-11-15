    //对象的定义
    boost::posix_time::time_duration td(1, 10, 30, 1000); //1小时10分钟30秒1毫秒
    boost::posix_time::time_duration  td1(1, 60, 60, 1000); //2小时1分钟1毫秒，超出的时间会自动进位
    boost::posix_time::time_duration td2 = boost::posix_time::duration_from_string("1:10:30:001");  //1小时10分钟30秒1毫秒

    //成员函数
    assert(td.hours() == 1 && td.minutes() == 10 && td.seconds() == 30);
    assert(td.total_seconds() == 1 * 3600 + 10 * 60 + 30);
    
    //获取字符串表示
    cout << boost::posix_time::to_simple_string(td) << endl; //输出为 01:10:30.001000
    cout << boost::posix_time::to_iso_string(td) << endl; //输出为 011030.001000

    //运算
    boost::posix_time::hours h(1);
    boost::posix_time::minutes m(10);
    boost::posix_time::seconds s(30);
    boost::posix_time::millisec ms(1);
    boost::posix_time::time_duration td3 = h + m + s + ms;
    assert(td2 == td3);
    
    
