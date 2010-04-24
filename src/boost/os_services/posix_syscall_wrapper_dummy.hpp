#ifndef BOOST_OS_SERVICES_POSIX_SYSCALL_WRAPPER_DUMMY_HPP
#define BOOST_OS_SERVICES_POSIX_SYSCALL_WRAPPER_DUMMY_HPP

//#include <string>

#include <boost/filesystem/path.hpp>

namespace boost {
namespace os_services {
namespace posix_syscall_wrapper_dummy
{

int open_file( const boost::filesystem::path& path )
{
	int file_descriptor = 1;
	return file_descriptor;
}


void close_file( int file_descriptor, bool no_throw = false )
{
	if ( file_descriptor != 0 )
	{
		file_descriptor = 0;
	}
}

} // namespace posix_syscall_wrapper_dummy
} // namespace os_services
} // namespace boost



#endif // BOOST_OS_SERVICES_POSIX_SYSCALL_WRAPPER_DUMMY_HPP
