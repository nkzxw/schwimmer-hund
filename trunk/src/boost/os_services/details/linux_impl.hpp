//TODO: hacer build que no dependa de la librería dinamica

#ifndef BOOST_OS_SERVICES_DETAIL_LINUX_IMPL_HPP
#define BOOST_OS_SERVICES_DETAIL_LINUX_IMPL_HPP

#include <string>

// C-Std Headers
#include <cerrno>		//<errno.h>
#include <cstdio>		//<stdio.h>
#include <cstdlib>		//<stdlib.h>
#include <cstring>		//<string.h>		// for strerror

//TODO: comentado solo para compilar bajo Windows
//#include <sys/types.h>
//#include <sys/inotify.h>

#include <boost/bimap.hpp>
#include <boost/bimap/list_of.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/integer.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>

#include <boost/os_services/details/base_impl.hpp>
#include <boost/os_services/notify_filters.hpp>

//#define EVENT_SIZE  ( sizeof (struct inotify_event) )
//#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

namespace boost {
namespace os_services {
namespace detail {

//TODO: si es necesario para todas las implementaciones, pasar a base_impl
typedef boost::shared_ptr<boost::thread> HeapThread; //TODO: cambiar nombre

//TODO: analizar si vale la pena que base_impl tenga template
class linux_impl : public base_impl<linux_impl>
{
public:

	linux_impl()
		: is_initialized_(false), closing_(false), file_descriptor_(0) //,watch_descriptor_(0)
	{}

	~linux_impl()
	{
		closing_ = true;

		if ( thread_ )
		{
			thread_->join();
		}

		if ( file_descriptor_ != 0 )
		{

			BOOST_FOREACH(watch_descriptors_type::left_reference p, watch_descriptors_.left)
			{
				if ( p.second != 0 )
				{
					//TODO: manejo de errores
					////printf("removing watch...\n");
					//int ret_value = ::inotify_rm_watch( file_descriptor_, p.second );
					////printf("retRMWatch: %d\n", retRMWatch);
				}
			}

			// TODO: parece que close(0) cierra el standard input (CIN)
			////printf("closing file descriptor...\n");
			//int ret_value =  ::close( file_descriptor_ );
			////printf("retClose: %d\n", retClose);
		}
	}

	void add_directory_impl ( const std::string& dir_name ) //throw (std::invalid_argument, std::runtime_error)
	{
		//watch_descriptors_.insert(watch_descriptors_type::relation(watch_descriptor, dir_name));
		//watch_descriptors_.insert(watch_descriptors_type::value_type(dir_name, watch_descriptor));
		watch_descriptors_.insert(watch_descriptors_type::relation(dir_name, 0));
	}

	//void remove_directory_impl(const std::string& dir_name) // throw (std::invalid_argument);

	void start() //(const std::string& path)
	{
		if (!is_initialized_)
		{
			//file_descriptor_ = ::inotify_init();
			if (file_descriptor_ == -1)
			{
				std::ostringstream oss;
				oss << "Failed to initialize monitor - Reason: " << std::strerror(errno);
				throw (std::runtime_error(oss.str()));
			}
			is_initialized_ = true;
			//std::cout << "CREATION fileDescriptor_: " << file_descriptor_ << std::endl;
		}

		BOOST_FOREACH(watch_descriptors_type::left_reference p, watch_descriptors_.left)
		{
			uint32_t watch_descriptor = 0;
			//uint32_t watch_descriptor = ::inotify_add_watch(fd_, p.first.c_str(), IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO);
			if (watch_descriptor == -1)
			{
				std::ostringstream oss;
				oss << "Failed to monitor directory - Directory: " << p.first << " - Reason: " << std::strerror(errno);
				throw (std::invalid_argument(oss.str()));
			}

			p.second = watch_descriptor;
		}

		thread_.reset( new boost::thread( boost::bind(&linux_impl::handle_directory_changes, this) ) );
	}

public: //private:  //TODO:

	void handle_directory_changes() //TODO: rename
	{
		int i = 0;
		//char buffer[BUF_LEN];

		//TODO: while
		//TODO: boost asio buffer

		while (! closing_ )
		{
	////		printf("reading in file descriptor\n");
	//		int length = read( file_descriptor_, buffer, BUF_LEN );
	////		printf("end reading on file descriptor\n");
			int length = 0;

			if (! closing_)
			{

	//			printf("length: %d\n", length);

				if ( length < 0 )
				{
					perror( "read" );
				}


				//int j;
				//for (j = 0; j<length; j++)
				//{
				//	printf("buffer[j]: %d\n", buffer[j]);
				//}

	//			printf("i: %d\n", i);
				while ( i < length )
				{
	//				printf("inside the 'while'\n");

					//struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ]; //TODO:
					//inotify_event = reinterpret_cast<struct inotify_event*> (buffer_ + bytes_processed);


	//				printf("event: %d\n", (void*)event);
	//				printf("event->len: %d\n", event->len);

					//TODO: cambiar todo esto, adaptar a como lo hace windows_impl
	//				if ( event->len )
	//				{
	//					if ( event->mask & IN_CREATE )
	//					{
	//						if ( this->created_callback_ )
	//						{
	////							std::cout << "ESTOOOOYYY ACAA" << std::endl;
	//							//std::string fileName( event->name );
	//							filesystem_event_args temp;
	//							temp.Name = event->name; //fileName;

	//							//threadObject->This->Created(temp);
	//							this->created_callback_(temp);
	//						}

	//						//if ( event->mask & IN_ISDIR )
	//						//{
	//						//	printf( "The directory '%s' was created.\n", event->name );
	//						//}
	//						//else
	//						//{
	//						//	printf( "The file '%s' was created.\n", event->name );
	//						//}
	//					}
	//					else if ( event->mask & IN_DELETE )
	//					{
	//						if (this->deleted_callback_)
	//						{
	//							//std::string fileName( event->name );
	//							filesystem_event_args temp;
	//							temp.name = event->name; //fileName;

	//							this->deleted_callback_(temp);
	//						}

	////						if ( event->mask & IN_ISDIR )
	////						{
	////							printf( "The directory '%s' was deleted.\n", event->name );
	////						}
	////						else
	////						{
	////							printf( "The file '%s' was deleted.\n", event->name );
	////						}
	//					}
	//					else if ( event->mask & IN_MODIFY )
	//					{

	//						if (this->changed_callback_)
	//						{
	//							//std::string fileName( event->name );
	//							filesystem_event_args temp;
	//							temp.Name = event->name; //fileName;

	//							this->changed_callback_(temp);
	//						}




	////						if ( event->mask & IN_ISDIR )
	////						{
	////							printf( "The directory '%s' was modified.\n", event->name );
	////						}
	////						else
	////						{
	////							printf( "The file '%s' was modified.\n", event->name );
	////						}
	//					}

	//					//TODO: renaming
	//				}

					//i += EVENT_SIZE + event->len;

	//				printf("i: %d\n", i);
				}
			}
		}
	}

	HeapThread thread_;

	bool is_initialized_;
	int file_descriptor_; // file descriptor
	//int watch_descriptor_;
	bool closing_;


	//typedef boost::bimap<boost::uint32_t, std::string> watch_descriptors_type;
	//typedef boost::bimap<std::string, boost::uint32_t> watch_descriptors_type;
	typedef boost::bimap< std::string, boost::bimaps::list_of<boost::uint32_t> > watch_descriptors_type;
	watch_descriptors_type watch_descriptors_;

};


} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_LINUX_IMPL_HPP