#ifndef BOOST_OS_SERVICES_DETAIL_UTILS_HPP_INCLUDED
#define BOOST_OS_SERVICES_DETAIL_UTILS_HPP_INCLUDED

#include <string>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace boost {
namespace os_services {
namespace detail {
namespace utils {

bool directory_exists( const boost::filesystem::path& full_path )
{
	if ( ! boost::filesystem::exists( full_path ) )
	{
		return false;
	}

	if ( ! boost::filesystem::is_directory( full_path ) )
	{
		return false;
	}

	return true;
}

bool directory_exists( const std::string& str_path )
{
	//TODO: trim de path
	if ( str_path.size() == 0 )
	{
		return false;
	}

	boost::filesystem::path full_path( str_path, boost::filesystem::native );
	return directory_exists(full_path);
}

} // namespace utils
} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_UTILS_HPP_INCLUDED
