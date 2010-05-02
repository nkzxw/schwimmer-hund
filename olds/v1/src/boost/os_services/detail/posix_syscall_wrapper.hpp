#ifndef BOOST_OS_SERVICES_DETAIL_POSIX_SYSCALL_WRAPPER_HPP_INCLUDED
#define BOOST_OS_SERVICES_DETAIL_POSIX_SYSCALL_WRAPPER_HPP_INCLUDED

//#include <string>

// C-Std headers
#include <cerrno>	//TODO: probar si es necesario
#include <cstdio>	//TODO: probar si es necesario
#include <cstdlib>	//TODO: probar si es necesario
#include <cstring>	// for strerror

// POSIX headers
//#include <sys/event.h>
//#include <sys/time.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <unistd.h>

//TODO: volver a compilar en FreeBSD

//TODO: analizar bien donde deberia estar todo esto...

#include <boost/filesystem/path.hpp>


//O_EVTONLY solo existe en MacOSX, no en FreeBSD
#ifndef O_EVTONLY
#define O_EVTONLY O_RDONLY
#endif


namespace boost {
namespace os_services {
namespace detail {
namespace posix_syscall_wrapper
{

//TODO: hacer lo mismo con WIN32API Wrapper. Los Wrapper deben acercar a las API's a la filosofia C++ -> excepciones, templates, clases, etc...

int open_file( const boost::filesystem::path& path )
{
	int file_descriptor = ::open( path.native_file_string().c_str(), O_EVTONLY );

	if ( file_descriptor == -1 )
	{
		std::ostringstream oss;
		oss << "open failed - File: " << path.native_file_string() << " - Reason: " << std::strerror(errno);
		throw (std::runtime_error(oss.str()));
	}

	return file_descriptor;
}


void close( int& descriptor )
{
	if ( descriptor != 0 )
	{
		int ret_value = ::close( descriptor ); //close
		if ( ret_value == -1 )
		{
			std::ostringstream oss;
			oss << "close error - Descriptor: '" << descriptor << "' - Reason: " << std::strerror(errno);
			throw (std::runtime_error(oss.str()));					
		}
		descriptor = 0;
	}
}

} // namespace posix_syscall_wrapper
} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_POSIX_SYSCALL_WRAPPER_HPP_INCLUDED
