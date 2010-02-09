#ifndef BOOST_OS_SERVICES_EVENT_ARGS_HPP
#define BOOST_OS_SERVICES_EVENT_ARGS_HPP

namespace boost {
namespace os_services {

//TODO: no me gusta el nombre
//TODO: completar

//TODO: extraido de .Net
//public enum WatcherChangeTypes
//{
//	Created = 1,
//	Deleted = 2,
//	Changed = 4,
//	Renamed = 8,
//	All = 15,
//}

struct filesystem_event_args
{

	filesystem_event_args(int _change_type, const std::string& _directory, const std::string& _name )
		: change_type(_change_type), name(_name)
	{
		full_path = _directory + '\\' + _name;
	}

	std::string name;
	std::string full_path;
	int change_type; //TODO: completar, enum WatcherChangeTypes de .Net
};


struct renamed_event_args : public filesystem_event_args
{

	renamed_event_args(int _change_type, const std::string& _directory, const std::string& _name, const std::string& _old_name)
		: filesystem_event_args( _change_type, _directory, _name), old_name(_old_name)
	{
		//TODO: ver el tema de los paths, revisar que ya el path no tenga incluido un '\'

		//full_path = _directory + '\\' + _name;
		old_full_path = _directory + '\\' + _old_name;
	}

	std::string old_full_path;
	std::string old_name;
};

} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_EVENT_ARGS_HPP
