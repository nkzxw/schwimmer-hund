#ifndef BOOST_OS_SERVICES_EVENT_HANDLERS_HPP
#define BOOST_OS_SERVICES_EVENT_HANDLERS_HPP

#include <boost/function.hpp>

#include <boost/os_services/event_handlers.hpp>

namespace boost {
namespace os_services {

//TODO: falta el object sender que podría ser implementado mediante boost::any
typedef boost::function<void (filesystem_event_args e)> filesystem_event_handler;
typedef boost::function<void (renamed_event_args e)> renamed_event_handler;

} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_EVENT_HANDLERS_HPP
