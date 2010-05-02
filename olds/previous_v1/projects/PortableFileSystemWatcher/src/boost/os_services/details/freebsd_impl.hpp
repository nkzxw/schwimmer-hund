#ifndef BOOST_OS_SERVICES_DETAIL_FREEBSD_IMPL_HPP
#define BOOST_OS_SERVICES_DETAIL_FREEBSD_IMPL_HPP

#include <boost/os_services/details/base_impl.hpp>

namespace boost {
namespace os_services {
namespace detail {

struct freebsd_impl : public base_impl<freebsd_impl>
{
	void start()
	{
		std::cout << "freebsd_impl::start()" << std::endl;
	}
};


} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_FREEBSD_IMPL_HPP
