#ifndef BOOST_OS_SERVICES_CHANGE_TYPES_HPP
#define BOOST_OS_SERVICES_CHANGE_TYPES_HPP

namespace boost {
namespace os_services {
namespace change_types
{
	static const int created = 1;
	static const int deleted = 2;
	static const int changed = 4;
	static const int renamed = 8;
	static const int all = 15;

} // namespace change_types
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_CHANGE_TYPES_HPP
