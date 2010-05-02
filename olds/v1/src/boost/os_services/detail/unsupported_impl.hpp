#ifndef BOOST_OS_SERVICES_DETAIL_UNSUPPORTED_IMPL_HPP_INCLUDED
#define BOOST_OS_SERVICES_DETAIL_UNSUPPORTED_IMPL_HPP_INCLUDED

namespace boost {
namespace os_services {
namespace detail {

struct unsupported_impl
{
	void start()
	{
		std::cout << "unsupported_impl::start()" << std::endl;
	}
};

//typedef UnsupportedImpl PlatformImpl;

} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_UNSUPPORTED_IMPL_HPP_INCLUDED
