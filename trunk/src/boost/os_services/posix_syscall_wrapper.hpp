#ifndef BOOST_OS_SERVICES_POSIX_SYSCALL_WRAPPER_HPP
#define BOOST_OS_SERVICES_POSIX_SYSCALL_WRAPPER_HPP

//#include <string>

//// C-Std headers
//#include <cerrno>	//TODO: probar si es necesario
//#include <cstdio>	//TODO: probar si es necesario
//#include <cstdlib>	//TODO: probar si es necesario
//#include <cstring>	// for strerror
//
//// POSIX headers
//#include <sys/event.h>
//#include <sys/fcntl.h>
//#include <sys/time.h>
//#include <sys/types.h>
//#include <unistd.h>

//TODO: analizar bien donde deberia estar todo esto...

#include <boost/filesystem/path.hpp>


//O_EVTONLY solo existe en MacOSX, no en FreeBSD
#ifndef O_EVTONLY
#define O_EVTONLY O_RDONLY
#endif


namespace boost {
namespace os_services {
namespace posix_syscall_wrapper
{

int open_file( const boost::filesystem::path& path )
{
	int file_descriptor = ::open( path_.native_file_string().c_str(), O_EVTONLY );

	//std::cout << "file_descriptor: " << file_descriptor << std::endl;
	//std::cout << "path.native_file_string(): " << path.native_file_string() << std::endl;

	if ( file_descriptor == -1 )
	{
		std::ostringstream oss;
		oss << "open failed - File: " << path.native_file_string() << " - Reason: " << std::strerror(errno);
		throw (std::runtime_error(oss.str()));
		//throw (std::invalid_argument(oss.str()));
	}


	return file_descriptor;

}


void close_file( int file_descriptor, bool no_throw = false )
{
	//std::cout << "void close( bool no_throw = false, bool close_subitems = true )" << std::endl;

	if ( file_descriptor != 0 )
	{
		int ret_value = ::close( file_descriptor ); //close
		if ( ret_value == -1 )
		{
			if ( no_throw )
			{
				//Destructor -> no-throw
				std::cerr << "Failed to close file descriptor - File descriptor: '" << file_descriptor << "' - File path: '" << this->path_.native_file_string() << "' - Reason: " << std::strerror(errno) << std::endl;
			}
			else
			{
				std::ostringstream oss;
				oss << "Failed to close file descriptor - File descriptor: '" << file_descriptor << "' - File path: '" << this->path_.native_file_string() << "' - Reason: " << std::strerror(errno);
				throw (std::runtime_error(oss.str()));					
			}
		}
		file_descriptor = 0;
	}
}

} // namespace posix_syscall_wrapper
} // namespace os_services
} // namespace boost



#endif // BOOST_OS_SERVICES_POSIX_SYSCALL_WRAPPER_HPP
