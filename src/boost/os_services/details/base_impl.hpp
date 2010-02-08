#ifndef BOOST_OS_SERVICES_DETAIL_BASE_IMPL_HPP
#define BOOST_OS_SERVICES_DETAIL_BASE_IMPL_HPP

#include <string>

#include <boost/os_services/event_args.hpp>
#include <boost/os_services/event_handlers.hpp>

namespace boost {
namespace os_services {
namespace detail {

template <typename Type>
class base_impl
{
public:
	// Event Handlers
	filesystem_event_handler changed_callback_;
	filesystem_event_handler created_callback_;
	filesystem_event_handler deleted_callback_;
	renamed_event_handler renamed_callback_;

public: // private: //TODO:
	int notify_filters_;									//TODO: debería ser un enum
	std::string filter_;
	bool include_subdirectories_;

protected:
	template <typename Callback, typename Args>
	inline void do_callback(Callback callback, Args args )
	{
		if ( callback )
		{
			callback( args );
		}
		
	}
};


} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_BASE_IMPL_HPP
