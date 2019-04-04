#pragma once
#include <string>
#include <memory>
#include "boost/log/core.hpp"
#include "boost/log/trivial.hpp"
#include "boost/log/expressions.hpp"
#include "boost/log/attributes/mutable_constant.hpp"
#include "boost/log/sinks/text_file_backend.hpp"
#include "boost/log/utility/setup/file.hpp"
#include "boost/log/utility/setup/common_attributes.hpp"
#include "boost/log/sources/severity_logger.hpp"
#include "boost/log/support/date_time.hpp"
#include "boost/log/sources/record_ostream.hpp"

namespace platon {
    extern std::shared_ptr<boost::log::sources::severity_logger< boost::log::trivial::severity_level>> lg;

    template<typename ValueType>
    ValueType set_get_attrib(const char* name, ValueType value) {
        auto attr = boost::log::attribute_cast<boost::log::attributes::mutable_constant<ValueType>>(boost::log::core::get()->get_global_attributes()[name]);
        attr.set(value);
        return attr.get();
    }

#define LOGLEVEL(level) \
   BOOST_LOG_STREAM_WITH_PARAMS( \
      (*lg.get()), \
         (set_get_attrib("File", string(__FILE__))) \
         (set_get_attrib("Line", __LINE__)) \
         (boost::log::keywords::severity = (boost::log::trivial::level)) \
   )

#define LOGTRACE LOGLEVEL(trace)
#define LOGDEBUG LOGLEVEL(debug)
#define LOGINFO LOGLEVEL(info)
#define LOGWARN LOGLEVEL(warning)
#define LOGERROR LOGLEVEL(error)
#define LOGFATAL LOGLEVEL(fatal)



    void initLog(const std::string &logPath, const std::string &logLevel);
}
