

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
