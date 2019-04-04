//
// Created by zhou.yang on 2018/10/11.
//
#include "boost/algorithm/string.hpp"
#include "boost/log/attributes/mutable_constant.hpp"

#include "Log.h"
using namespace std;
namespace logging = boost::log;

namespace keywords = boost::log::keywords;
namespace attrs    = boost::log::attributes;
namespace expr     = boost::log::expressions;
namespace platon{

    shared_ptr<logging::sources::severity_logger< logging::trivial::severity_level> > lg;

    const static std::string logName = "platonabi.log";

    logging::trivial::severity_level getLevel(const string &logLevel) {
        string lowerLevel = boost::algorithm::to_lower_copy(logLevel);

        if (lowerLevel == "trace") {
            return logging::trivial::trace;
        } else if (lowerLevel == "debug") {
            return logging::trivial::debug;
        } else if (lowerLevel == "info") {
            return logging::trivial::info;
        } else if (lowerLevel == "warning") {
            return logging::trivial::warning;
        } else if (lowerLevel == "error") {
            return logging::trivial::error;
        } else if (lowerLevel == "fatal") {
            return logging::trivial::fatal;
        }
        return logging::trivial::info;
    }

    void initLog(const string &logPath, const string &logLevel) {
        string logfile = logPath + "/" + logName;
        logging::core::get()->add_global_attribute("File", attrs::mutable_constant<std::string>(""));
        logging::core::get()->add_global_attribute("Line", attrs::mutable_constant<int>(0));
        logging::core::get()->set_filter(logging::trivial::severity>=getLevel(logLevel));

        logging::add_file_log (
                keywords::file_name = logfile,
                keywords::format = (
                        expr::stream
                                << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d_%H:%M:%S.%f")
                                << ": <" << boost::log::trivial::severity << "> "
                                << '['   << expr::attr<std::string>("File")
                                << ':' << expr::attr<int>("Line") << "] "
                                << expr::smessage
                )
        );
        logging::add_common_attributes();
        lg = make_shared<logging::sources::severity_logger< logging::trivial::severity_level>>();
    }
}
