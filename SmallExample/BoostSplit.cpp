#include <boost/algorithm/string.hpp>

void boostSplit()
{
    std::string strSrc = "1,3,5,  66cdd,2,3,4 5 ,";
    
    // boost::token_compress_on: 将连续多个分隔符当成一个分隔符处理。
    // 即：连续多个分隔符之间不会生成一个"" 字符 串。如",,, " 只会有一个""
    boost::split(vecResult, strSrc, boost::is_any_of(", "),
        boost::token_compress_on);
    
}

