#ifndef BOOST_OS_SERVICES_EVENT_ARGS_HPP_INCLUDED
#define BOOST_OS_SERVICES_EVENT_ARGS_HPP_INCLUDED

#include <boost/filesystem/path.hpp>

namespace boost {
namespace os_services {

struct filesystem_event_args
{
	filesystem_event_args( int _change_type, const std::string& _directory, const std::string& _name )
		: change_type(_change_type), name(_name), full_path("")
	{
		if (_name.size() > 0)
		{
			boost::filesystem::path directory_path( _directory );
			full_path = (directory_path / _name).native_file_string();
		}
	}

	filesystem_event_args( int _change_type, const boost::filesystem::path& _path )
		: change_type(_change_type), name(""), full_path("")
	{
		if (_path.native_file_string().size() > 0)
		{
			this->full_path = _path.native_file_string();
			this->name = _path.filename();
		}
	}

	std::string name;
	std::string full_path;
	int change_type; //TODO: completar, enum WatcherChangeTypes de .Net
};


struct renamed_event_args : public filesystem_event_args
{

	renamed_event_args( int _change_type, const std::string& _directory, const std::string& _name, const std::string& _old_name )
		: filesystem_event_args( _change_type, _directory, _name), old_name(_old_name), old_full_path("")
	{
		if ( _old_name.size() > 0 )
		{
			boost::filesystem::path directory_path(_directory);
			old_full_path = (directory_path / _old_name).native_file_string();
		}
	}

	renamed_event_args( int _change_type, const boost::filesystem::path& _path, const boost::filesystem::path& _old_path )
		: filesystem_event_args( _change_type, _path), old_name(""), old_full_path("")
	{
		if ( _old_path.native_file_string().size() > 0 )
		{
			this->old_full_path = _old_path.native_file_string();
			this->old_name = _old_path.filename();
		}
	}

	std::string old_full_path; //TODO: analizar si usar boost::filesystem::path
	std::string old_name;
};

} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_EVENT_ARGS_HPP_INCLUDED
