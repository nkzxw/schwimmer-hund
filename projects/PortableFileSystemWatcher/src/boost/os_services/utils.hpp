#ifndef BOOST_OS_SERVICES_UTILS_HPP
#define BOOST_OS_SERVICES_UTILS_HPP

#include <string>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace boost {
namespace os_services {
namespace utils {

bool directory_exists( const std::string& path )
{
	boost::filesystem::path fullPath( path, boost::filesystem::native );

	if ( !boost::filesystem::exists( fullPath ) )
	{
		return false;
	}

	if ( !boost::filesystem::is_directory( fullPath ) )
	{
		return false;
	}

	return true;
}

} // namespace utils
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_UTILS_HPP
