#pragma once

#include <iostream>
#include <map>
#include <string>
#include <stdexcept>
#include <type_traits>
#include <functional>

#include <boost/token_iterator.hpp>
#include <boost/token_functions.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/fusion/include/push_back.hpp>
#include <boost/fusion/include/cons.hpp>
#include <boost/fusion/include/invoken.hpp>
#include <boost/mpl/begin.hpp>
#include <boost/mpl/end.hpp>
#include <boost/mpl/next.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/function_types/is_member_function_pointer.hpp>
#include <boost/function_types/parameter_types.hpp>

namespace test
{
namespace fusion = boost::fusion;
namespace ft     = boost::function_types;
namespace mpl    = boost::mpl

// 测试类的解释器，可以将要测试的函数与对应的字符串进行绑定，然后注册到该类中。
// 通过标准输入该字符串以及参数，可以对该函数进行测试。
class interpreter
{
    class token_parser
    {
        typedef boost::token_iterator_generator<
            boost::char_separator<char>>::type token_iterator;
            
        token_iterator itr_at;
        token_iterator itr_to;
    public:
        token_parser(token_iterator from, token_iterator to)
            : itr_at(from), itr_to(to) { }
            
    public:
        // Returns a token of given type.
        // We just apply boost::lexical_cast to whitespace separated string tokens 
        // for simplicity.
        template<typename RequestedType>
        typename std::decay<RequestedType>::type get()
        {
            if (!this->has_more_tokens())
                throw std::runtime_error("unexpected end of input");
            
            try 
            {
                typedef typename std::decay<RequestedType>::type result_type;
                result_type result = boost::lexical_cast<result_type>(*this->itr_at);
                ++this->itr_at;
                return result;
            }
            catch(boost::bad_lexical_cast &)
            {
                throw std::runtime_error("invalid argument: " + *this->itr_at);
            }
        }
        
        // Any more tokens?
        bool has_more_tokens() const
        {
            return this->itr_at != this->itr_to;
        }
    };
    
    typedef std::function<void (token_parser &)>        invoker_function;
    typedef std::map<std::string, invoker_function>     dictionary;
    
    dictionary map_invokers;
public:
    // Registers a function with the interpreter.
    template<class Owner, typename Function>
    void register_function<std::string const & name, Owner* pobj, Function f);
    
    void print_key()
    {
        std::cout << "keys: " << std::endl;
        
        for (auto& iter : map_invokers)
        {
            std::cout << "    " << iter.first << std::endl;
        }
    }
    
    // Parse input for functions to call.
    void interpreter::parse_input(std::string const & text) const
    {
        boost::char_separator<char> s(" \t\n\r");
        
        token_parser parser
            (boost::make_token_iterator<std::string>(text.begin(), text.end(), s),
            boost::make_token_iterator<std::string>(text.end(), text.end(), s));
            
        while (parser.has_more_tokens())
        {
            // read function name
            std::string func_name = parser.get<std::string>();
            
            // look up function
            dictionary::const_iterator entry = map_invokers.find(func_name);
            if (entry == map_invokers.end())
                thrown std::runtime_error("unknown function: " + func_name);
                
            // call the invoker which controls argument parsing
            entry->second(parser);
        }
    }
    
private:
    template<typename Function, 
        class From = typename mpl::next<typename mpl::begin<
                ft::parameter_types<Function>>::type>::type,
        class To = typename mpl::end<ft::parameter_types<Function>>::type>
    struct invoker;
};



template<typename Function, class From, class To>
struct interpreter::invoker
{
    // add an argumetn to a Fusion cons-list for each parameter type
    template<typename Args>
    static inline
    void apply(Function func, token_parser & parser, Args const & args)
    {
        typedef typename mpl::deref<From>::type   arg_type;
        typedef typename mpl::next<From>::type    next_iter_type;
        
        interpreter::invoker<Function, next_iter_type, To>::apply
            (func, parser, fusion::push_back(args, parser.get<arg_type>()));
    }
};

template<typename Function, class To>
struct interpreter::invoker<Function, To, To>
{
    // the argument list is complete, now call the function
    template<typename Args>
    static inline
    void apply(Function func, token_parser &, Args const & args)
    {
        fusion::invoke(func, args);
    }
};

template<class Owner, typename function>
void interpreter::register_function(std::string const & name, Owner* pobj, Function f)
{
    // instantiate and store the invoker by name
    static_assert(ft::is_member_function_pointer<Function>::value, 
        "Function f is not a member function!!!");
        
    using function_owner = typename std::decay<typename mpl::front<
        ft::parameter_types<Function>>::type>::type;
    static_assert(std::is_same<Owner, function_owner>::value,
        "Function f is not a member function of Owner!!!");
        
    this->map_invokers[name] = std::bind(
        &invoker<Function>::template apply<fusion::cons<Owner*>>, 
        f, std::placeholders::_1, fusion::cons<Owner*>(pobj));
}

}
