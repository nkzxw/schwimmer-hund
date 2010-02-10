#ifndef BOOST_OS_SERVICES_NOTIFY_FILTERS_HPP
#define BOOST_OS_SERVICES_NOTIFY_FILTERS_HPP

//enum NotifyFilters
//TODO: por ahora muy atado a Win32

namespace boost {
namespace os_services {
namespace notify_filters 
{
	static const int file_name		= 0x00000001;  //1;
	static const int directory_name	= 0x00000002;  //2;
	static const int attributes		= 0x00000004;  //4;
	static const int size			= 0x00000008;  //8;
	static const int last_write		= 0x00000010;  //16;
	static const int last_access	= 0x00000020;  //32;
	static const int creation_time	= 0x00000040;  //64;
	static const int security		= 0x00000100;  //256;

} // namespace notify_filters
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_NOTIFY_FILTERS_HPP
