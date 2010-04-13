#ifndef BOOST_OS_SERVICES_DETAIL_BASE_IMPL_HPP
#define BOOST_OS_SERVICES_DETAIL_BASE_IMPL_HPP

#include <string>

#include <boost/filesystem/path.hpp>

#include <boost/os_services/event_args.hpp>
#include <boost/os_services/event_handlers.hpp>
#include <boost/os_services/utils.hpp> //TODO: deberia estar detro del directorio details

namespace boost {
namespace os_services {
namespace detail {


template <typename Type>
class base_impl // : private boost::noncopyable -> No tiene sentido ya que es "detail" 
{
public:

	// Event Handlers
	filesystem_event_handler changed_handler_;
	filesystem_event_handler created_handler_;
	filesystem_event_handler deleted_handler_;
	renamed_event_handler renamed_handler_;

	void add_directory (const std::string& dir_name) //throw (std::invalid_argument, std::runtime_error)
	{ 
		if ( !utils::directory_exists(dir_name) )
		{
			//throw std::runtime_error("InvalidDirName");
			////throw new ArgumentException(SR.GetString("InvalidDirName", new object[] { path }));
			throw (std::invalid_argument("'" + dir_name + "' is not a valid directory."));
		}

		static_cast<Type*>(this)->add_directory_impl(dir_name);
	}

	void add_directory (boost::filesystem::path directory) //throw (std::invalid_argument, std::runtime_error)
	{ 
		if ( !utils::directory_exists(directory) )
		{
			throw (std::invalid_argument("'" + directory.native_file_string() + "' is not a valid directory."));
		}
			
		static_cast<Type*>(this)->add_directory_impl( directory.native_file_string() );
	}

	//void start() {}
	

public: // private: //TODO:
	int notify_filters_;			//TODO: deber�a ser un enum
	std::string filter_;
	bool include_subdirectories_;

protected:

	template <typename Callback, typename Args>
	inline void do_callback(Callback callback, Args args )
	{
		if ( callback )
		{
			callback( args );
		}
	}
};

} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_BASE_IMPL_HPP
