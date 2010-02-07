//TODO: hacer build que no dependa de la librería dinamica

#ifndef BOOST_OS_SERVICES_DETAIL_LINUX_IMPL_HPP
#define BOOST_OS_SERVICES_DETAIL_LINUX_IMPL_HPP

#include <string>

#include <cstdio>		//<stdio.h>
#include <cstdlib>		//<stdlib.h>
#include <cerrno>		//<errno.h>
//#include <pthread.h>	// Threads
#include <sys/types.h>
#include <sys/inotify.h>

#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>

#include <boost/os_services/details/base_impl.hpp>
#include <boost/os_services/notify_filters.hpp>


#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

namespace boost {
namespace os_services {
namespace detail {

//TODO: si es necesario para todas las implementaciones, pasar a base_impl
typedef boost::shared_ptr<boost::thread> HeapThread; //TODO: cambiar nombre

//TODO: analizar si vale la pena que base_impl tenga template
class linux_impl : public base_impl<linux_impl>
{
public:

	~linux_impl()
	{
		closing_ = true;

		if ( watchDescriptor_ != 0 )
		{
//			printf("removing watch...\n");
			int retRMWatch = inotify_rm_watch( fileDescriptor_, watchDescriptor_ );
//			printf("retRMWatch: %d\n", retRMWatch);
		}

		if ( fileDescriptor_ != 0 )
		{
			// TODO: parece que close(0) cierra el standard input (CIN)
//			printf("closing file descriptor...\n");
			int retClose =  close( fileDescriptor_ );
//			printf("retClose: %d\n", retClose);
		}

		if ( thread_ )
		{
			thread_->join();
		}
	
	}

	void start(const std::string& path)
	{
		fileDescriptor_ = inotify_init();

		if ( fileDescriptor_ < 0 ) 
		{
			perror( "inotify_init" );
		}

		
		//watchDescriptor_ = inotify_add_watch( fileDescriptor_, "/home/fernando/temp1", IN_MODIFY | IN_CREATE | IN_DELETE );
		watchDescriptor_ = inotify_add_watch( fileDescriptor_, path.c_str(), IN_MODIFY | IN_CREATE | IN_DELETE ); //TODO: IN_MOVED ??


		thread_.reset( new boost::thread( boost::bind(&linux_impl::HandleDirectoryChange, this) ) );
	}

public: //private:  //TODO:

	void HandleDirectoryChange() //TODO: rename
	{
		int i = 0;
		char buffer[BUF_LEN];


		//TODO: while
		//TODO: boost asio buffer

		while (! closing_ )
		{
	//		printf("reading in file descriptor\n");
			int length = read( fileDescriptor_, buffer, BUF_LEN );
	//		printf("end reading on file descriptor\n");

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

					struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ]; //TODO:
					inotify_event = reinterpret_cast<struct inotify_event*> (buffer_ + bytes_processed);


	//				printf("event: %d\n", (void*)event);
	//				printf("event->len: %d\n", event->len);

					if ( event->len )
					{
						if ( event->mask & IN_CREATE )
						{
							if ( this->created_callback_ )
							{
	//							std::cout << "ESTOOOOYYY ACAA" << std::endl;
								//std::string fileName( event->name );
								filesystem_event_args temp;
								temp.Name = event->name; //fileName;

								//threadObject->This->Created(temp);
								this->created_callback_(temp);
							}

							//if ( event->mask & IN_ISDIR )
							//{
							//	printf( "The directory '%s' was created.\n", event->name );
							//}
							//else
							//{
							//	printf( "The file '%s' was created.\n", event->name );
							//}
						}
						else if ( event->mask & IN_DELETE )
						{
							if (this->deleted_callback_)
							{
								//std::string fileName( event->name );
								filesystem_event_args temp;
								temp.Name = event->name; //fileName;

								this->deleted_callback_(temp);
							}

	//						if ( event->mask & IN_ISDIR )
	//						{
	//							printf( "The directory '%s' was deleted.\n", event->name );
	//						}
	//						else
	//						{
	//							printf( "The file '%s' was deleted.\n", event->name );
	//						}
						}
						else if ( event->mask & IN_MODIFY )
						{

							if (this->changed_callback_)
							{
								//std::string fileName( event->name );
								filesystem_event_args temp;
								temp.Name = event->name; //fileName;

								this->changed_callback_(temp);
							}




	//						if ( event->mask & IN_ISDIR )
	//						{
	//							printf( "The directory '%s' was modified.\n", event->name );
	//						}
	//						else
	//						{
	//							printf( "The file '%s' was modified.\n", event->name );
	//						}
						}

						//TODO: renaming
					}

					i += EVENT_SIZE + event->len;

	//				printf("i: %d\n", i);
				}
			}
		}

		////( void ) inotify_rm_watch( fileDescriptor_, wd );
		////( void ) close( fileDescriptor_ );
		//int retRMWatch = inotify_rm_watch( fileDescriptor_, watchDescriptor_ );
		//printf("retRMWatch: %d\n", retRMWatch);
		//int retClose =  close( fileDescriptor_ );
		//printf("retClose: %d\n", retClose);
		//pthread_join( thread1, NULL);
		////printf("Thread 1 returns: %d\n", iret1);
		//exit(0);
	}

	HeapThread thread_;

	int fileDescriptor_; // file descriptor
	int watchDescriptor_;
	bool closing_;


//	int notify_filters_;									//TODO: deber�a ser un enum
//	std::string filter_;
//	bool include_subdirectories_;

};


} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_LINUX_IMPL_HPP
