#pragma once

#include <exception>
#include <boost/exception/all.hpp>
namespace platon {
    typedef boost::error_info<struct tag_err_no, int> ErrNum;
    typedef boost::error_info<struct tag_err_str,std::string> ErrStr;
    class Exception
            : public std::exception,
            public  boost::exception {

    };
}