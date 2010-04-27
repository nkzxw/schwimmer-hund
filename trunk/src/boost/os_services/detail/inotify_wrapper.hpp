#ifndef BOOST_OS_SERVICES_DETAIL_INOTIFY_WRAPPER_HPP
#define BOOST_OS_SERVICES_DETAIL_INOTIFY_WRAPPER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif

//TODO: revisar los headers

#include <stdexcept>

// C-Std Headers
#include <cerrno>	//TODO: probar si es necesario
#include <cstdio>	//TODO: probar si es necesario
#include <cstdlib>	//TODO: probar si es necesario
#include <cstring>	// for strerror

// POSIX headers
//#include <sys/event.h>
//#include <sys/fcntl.h>
//#include <sys/time.h>
//#include <sys/types.h>
//#include <unistd.h>
#include <sys/inotify.h>
#include <sys/types.h>

#include <boost/noncopyable.hpp>

#include <boost/os_services/detail/posix_syscall_wrapper.hpp>


//TODO: problema de multiples CHANGE consecutivos. Vamos a probar con un buffer considerablemente más grande.
#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 4096 * ( EVENT_SIZE + 16 ) )	


namespace boost {
namespace os_services {
namespace detail {


class inotify_wrapper : private boost::noncopyable
{
public:

	inotify_wrapper()
		: is_initialized_( false ), file_descriptor_( 0 ), buffer_current_index_( 0 ), last_length_( 0 )
	{
		memset( buffer_, 0, BUF_LEN );
	}

	~inotify_wrapper()
	{
		close( true );
	}

	void initialize()
	{
		if ( ! is_initialized_ )
		{
			file_descriptor_ = ::inotify_init();

			if ( file_descriptor_ == -1 )
			{
				std::ostringstream oss;
				oss << "inotify_init error - Reason: " << std::strerror(errno);
				throw ( std::runtime_error(oss.str()) );
			}
			is_initialized_ = true;
		}
	}

	boost::uint32_t add_watch( const std::string& file_name )
	{
		boost::uint32_t watch_descriptor = ::inotify_add_watch( file_descriptor_, file_name.c_str(), IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO );
		
		if ( watch_descriptor == -1 )
		{
			std::ostringstream oss;
			oss << "inotify_add_watch error - File: " << file_name << " - Reason: " << std::strerror(errno);
			throw ( std::invalid_argument(oss.str()) );
		}
	}

	void remove_watch( boost::uint32_t& watch_descriptor ) const
	{
		int ret_value = ::inotify_rm_watch( file_descriptor_, watch_descriptor );

		if ( ret_value == -1 ) //TODO: constante POSIX_ERROR o algo asi... IDEM FreeBSD
		{
			std::ostringstream oss;
			oss << "inotify_rm_watch error - Reason: " << std::strerror(errno);
			throw ( std::runtime_error(oss.str()) );
		}
	}

	void close( bool no_throw = false )
	{
		if ( file_descriptor_ != 0 )
		{
			//TODO: se puede usar el close_file de posix_wapper
			int ret_value = ::close( file_descriptor_ );
			if ( ret_value == -1 )
			{
				if ( no_throw )
				{
					//Destructor -> no-throw
					std::cerr << "Failed to close inotify file descriptor - File Descriptor: '" << file_descriptor_ << "' - Reason: " << std::strerror(errno) << std::endl; 
				}
				else
				{
					std::ostringstream oss;
					oss << "Failed to close inotify file descriptor - File Descriptor: '" << file_descriptor_ << "' - Reason: " << std::strerror(errno);
					throw ( std::runtime_error(oss.str()) );					
				}
			}
			file_descriptor_ = 0;
		}
		is_initialized_ = false;
	}


	//boost::shared_ptr<T> get( int& event_type )

	template <typename T>
	T* get()
	{
		if ( buffer_current_index_ >= last_length_ )
		{
			buffer_current_index_ = 0;
			last_length_ = ::read( file_descriptor_, buffer_, BUF_LEN );

			if ( last_length_ == -1 )
			{
				std::ostringstream oss;
				oss << "read error - File Descriptor: '" << file_descriptor_ << "' - Reason: " << std::strerror(errno);
				throw ( std::runtime_error(oss.str()) );	
			}
		}

		
		//event = reinterpret_cast<struct inotify_event*> (buffer_ + bytes_processed);
		//struct inotify_event* event = ( struct inotify_event * ) &buffer_[ buffer_current_index_ ]; //TODO:

		T* event = (T*) &buffer_[ buffer_current_index_ ]; //TODO:
		buffer_current_index_ += EVENT_SIZE + event->len;

		return event;
	}

protected:

	bool is_initialized_;
	int file_descriptor_;

	char buffer_[BUF_LEN];
	int buffer_current_index_;
	int last_length_;
};


} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_INOTIFY_WRAPPER_HPP
