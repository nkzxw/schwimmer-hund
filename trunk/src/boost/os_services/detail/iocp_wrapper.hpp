#ifndef BOOST_OS_SERVICES_DETAIL_IOCP_WRAPPER_HPP_INCLUDED
#define BOOST_OS_SERVICES_DETAIL_IOCP_WRAPPER_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif

//TODO: revisar los headers


#include <boost/noncopyable.hpp>


namespace boost {
namespace os_services {
namespace detail {


class iocp_wrapper : private boost::noncopyable
{
public:

	iocp_wrapper()
		: is_initialized_( false ), handle_( 0 )
	{
	}

	~iocp_wrapper()
	{
		close( true ); //Destructor -> no throw
	}

	void initialize()
	{
		//TODO:
		if ( ! is_initialized_ )
		{
			is_initialized_ = true;
		}
	}

	boost::uint32_t add_watch( const std::string& file_name )
	{
		boost::uint32_t watch_descriptor = ::inotify_add_watch( handle_, file_name.c_str(), IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO );
		
		if ( watch_descriptor == -1 )
		{
			std::ostringstream oss;
			oss << "inotify_add_watch error - File: " << file_name << " - Reason: " << std::strerror(errno);
			throw ( std::invalid_argument(oss.str()) );
		}
	}

	//TODO:
	//	void* add_watch( const std::string& path ) //throw (std::invalid_argument, std::runtime_error)
	template <typename T>
	void add_watch( const boost::shared_ptr<T>& watch )
	{
		//TODO: datos necesarios para crear el watch
		//      const std::string& path					// OK -> Dato
		//		this->include_subdirectories_			// OK -> Dato
		//		this->notify_filters_,					// OK -> Dato

		//		completion_port_handle_					// pertenece a la clase
		//		directory_info->directory_handle,		// Se crea acá mismo
		//		directory_info
		//		&(*it)->overlapped
		//		(*it)->buffer,							// Un buffer por directorio???????????? Probar usando uno independiente...
		//		MAX_BUFFER, 
		//		&(*it)->buffer_length, 



		LPDIRECTORY_INFO directory_info = (LPDIRECTORY_INFO) malloc(sizeof(DIRECTORY_INFO));
		memset(directory_info, 0, sizeof(DIRECTORY_INFO));

		directory_info->directory_handle = win32api_wrapper::create_file( path, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL );

		//TODO: esto no me gusta
		lstrcpy( directory_info->directory_name, path.c_str() );

		//unsigned long addr = (unsigned long) &directory_info;

		completion_port_handle_ = win32api_wrapper::create_io_completion_port( directory_info->directory_handle, completion_port_handle_, (DWORD) directory_info, 0 );

		win32api_wrapper::read_directory_changes( (*it)->directory_handle, (*it)->buffer, MAX_BUFFER, this->include_subdirectories_ ? 1 : 0, this->notify_filters_, &(*it)->buffer_length, &(*it)->overlapped, NULL );

	}


	//void remove_watch( boost::uint32_t& watch_descriptor ) const
	//{
	//	//TODO:
	//}

	void close( bool no_throw = false )
	{
		if ( handle_ != 0 )
		{
			if ( no_throw )
			{
				try
				{
					win32api_wrapper::post_queued_completion_status( handle_, 0, 0, NULL );
					win32api_wrapper::close_handle( handle_ );
				}
				catch ( const std::runtime_error& e )
				{
					//Destructor -> NO_THROW
					std::cerr << e.what() << std::endl;
				}
			}
			else
			{
				win32api_wrapper::post_queued_completion_status( handle_, 0, 0, NULL );
				win32api_wrapper::close_handle( handle_ );
			}
			handle_ = 0;
		}
		is_initialized_ = false;
	}


	//boost::shared_ptr<T> get( int& event_type )
	template <typename T>
	T* get()
	{
	}

protected:

	bool is_initialized_;
	int handle_;
};


} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_IOCP_WRAPPER_HPP_INCLUDED
