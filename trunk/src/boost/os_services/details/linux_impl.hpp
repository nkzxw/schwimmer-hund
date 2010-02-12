//TODO: hacer build que no dependa de la librería dinamica

#ifndef BOOST_OS_SERVICES_DETAIL_LINUX_IMPL_HPP
#define BOOST_OS_SERVICES_DETAIL_LINUX_IMPL_HPP

#include <string>

// C-Std Headers
#include <cerrno>		//<errno.h>
#include <cstdio>		//<stdio.h>
#include <cstdlib>		//<stdlib.h>
#include <cstring>		//<string.h>		// for strerror

#include <sys/types.h>
#include <sys/inotify.h>

#include <boost/bimap.hpp>
#include <boost/bimap/list_of.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/integer.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>

#include <boost/os_services/change_types.hpp>
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

			//BOOST_FOREACH(watch_descriptors_type::left_reference p, watch_descriptors_.left)
			//{
			//	if ( p.second != 0 )
			//	{
			//		//TODO: manejo de errores
			//		////printf("removing watch...\n");
			//		int ret_value = ::inotify_rm_watch( file_descriptor_, p.second );
			//		////printf("retRMWatch: %d\n", retRMWatch);
			//	}
			//}


			BOOST_FOREACH(pair_type p, watch_descriptors_)
			{
				if ( p.second != 0 )
				{
					//TODO: manejo de errores
					////printf("removing watch...\n");
					int ret_value = ::inotify_rm_watch( file_descriptor_, p.second );
					////printf("retRMWatch: %d\n", retRMWatch);
				}
			}

			// TODO: parece que close(0) cierra el standard input (CIN)
			////printf("closing file descriptor...\n");
			int ret_value =  ::close( file_descriptor_ );
			////printf("retClose: %d\n", retClose);
		}
	}

	void add_directory_impl ( const std::string& dir_name ) //throw (std::invalid_argument, std::runtime_error)
	{
		//watch_descriptors_.insert(watch_descriptors_type::relation(watch_descriptor, dir_name));
		//watch_descriptors_.insert(watch_descriptors_type::value_type(dir_name, watch_descriptor));
		//watch_descriptors_.insert(watch_descriptors_type::relation(dir_name, 0));

		watch_descriptors_.push_back(std::make_pair(dir_name, 0));

	}

	//void remove_directory_impl(const std::string& dir_name) // throw (std::invalid_argument);

	void start() //(const std::string& path)
	{
		if (!is_initialized_)
		{
			file_descriptor_ = ::inotify_init();
			if (file_descriptor_ == -1)
			{
				std::ostringstream oss;
				oss << "Failed to initialize monitor - Reason: " << std::strerror(errno);
				throw (std::runtime_error(oss.str()));
			}
			is_initialized_ = true;
			//std::cout << "CREATION fileDescriptor_: " << file_descriptor_ << std::endl;
		}

		//BOOST_FOREACH(watch_descriptors_type::left_reference p, watch_descriptors_.left)
		//{

		//	//TODO: ver si estos atributos como "IN_MODIFY" deben ir fijos o seteados desde afuera.
		//	uint32_t watch_descriptor = ::inotify_add_watch(file_descriptor_, p.first.c_str(), IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);
		//	if (watch_descriptor == -1)
		//	{
		//		std::ostringstream oss;
		//		oss << "Failed to monitor directory - Directory: " << p.first << " - Reason: " << std::strerror(errno);
		//		throw (std::invalid_argument(oss.str()));
		//	}

		//	p.second = watch_descriptor;
		//}

		BOOST_FOREACH(pair_type p, watch_descriptors_)
		{

			//TODO: ver si estos atributos como "IN_MODIFY" deben ir fijos o seteados desde afuera.
			boost::uint32_t watch_descriptor = ::inotify_add_watch(file_descriptor_, p.first.c_str(), IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);
			//if (watch_descriptor == -1)
			if (watch_descriptor < 0)
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

	void handle_directory_changes()
	{
		int i = 0;
		char buffer[BUF_LEN];

		//TODO: while
		//TODO: boost asio buffer

		while ( !closing_ )
		{
	//		printf("reading in file descriptor\n");
			int length = ::read( file_descriptor_, buffer, BUF_LEN );
	//		printf("end reading on file descriptor\n");

			if (! closing_)
			{
				printf("length: %d\n", length);

				if ( length < 0 )
				{
					//TODO:
					perror( "read" );
				}

				boost::optional<std::string> old_name;
				std::string directory_name;


				printf("i: %d\n", i);
				while ( i < length )
				{
					//printf("inside the 'while'\n");

					struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ]; //TODO:
					//event = reinterpret_cast<struct inotify_event*> (buffer_ + bytes_processed);

					//printf("event: %d\n", (void*)event);
					//printf("event->len: %d\n", event->len);


					//TODO: cambiar todo esto, adaptar a como lo hace windows_impl
					if ( event->len ) //TODO: que espera hacer acá, mala práctica
					{

						std::string file_name( event->name );


						//watch_descriptors_type::left_iterator it = watch_descriptors_.left.find( "" ); // event->wd );
						//if ( it != watch_descriptors_.left.end() )
						
						watch_descriptors_type::const_iterator it = std::find_if( watch_descriptors_.begin(), watch_descriptors_.end(), boost::bind( &pair_type::second, _1 ) == event->wd );
						if ( it != watch_descriptors_.end() )
						{
							//directory_name = it->second;
						}
						else
						{
							//TODO: que pasa si no lo encontramos en la lista... DEBERIA SER UN RUN-TIME ERROR
						}



						if ( event->mask & IN_MOVED_FROM )
						{
							old_name.reset( file_name );
						}
						else if ( event->mask & IN_MOVED_TO)
						{
							if ( old_name )
							{
								notify_rename_event_args(change_types::renamed, directory_name, file_name, *old_name);
								old_name.reset();
							}
							else
							{
								notify_rename_event_args(change_types::renamed, directory_name, file_name, "");
								old_name.reset();
							}
						}
						else
						{
							if (old_name)
							{
								notify_rename_event_args(change_types::renamed, directory_name, "", *old_name);
								old_name.reset();
							}

							notify_file_system_event_args( event->mask, directory_name, file_name);
						}

						//event->mask & IN_CREATE


//						if ( event->mask & IN_CREATE )
//						{
//							if ( this->created_callback_ )
//							{
//	//							std::cout << "ESTOOOOYYY ACAA" << std::endl;
//								//std::string fileName( event->name );
//								filesystem_event_args temp;
//								temp.Name = event->name; //fileName;
//
//								//threadObject->This->Created(temp);
//								this->created_callback_(temp);
//							}
//
//							//if ( event->mask & IN_ISDIR )
//							//{
//							//	printf( "The directory '%s' was created.\n", event->name );
//							//}
//							//else
//							//{
//							//	printf( "The file '%s' was created.\n", event->name );
//							//}
//						}
//						else if ( event->mask & IN_DELETE )
//						{
//							if (this->deleted_callback_)
//							{
//								//std::string fileName( event->name );
//								filesystem_event_args temp;
//								temp.name = event->name; //fileName;
//
//								this->deleted_callback_(temp);
//							}
//
//	//						if ( event->mask & IN_ISDIR )
//	//						{
//	//							printf( "The directory '%s' was deleted.\n", event->name );
//	//						}
//	//						else
//	//						{
//	//							printf( "The file '%s' was deleted.\n", event->name );
//	//						}
//						}
//						else if ( event->mask & IN_MODIFY )
//						{
//							if (this->changed_callback_)
//							{
//								//std::string fileName( event->name );
//								filesystem_event_args temp;
//								temp.Name = event->name; //fileName;
//
//								this->changed_callback_(temp);
//							}
//	//						if ( event->mask & IN_ISDIR )
//	//						{
//	//							printf( "The directory '%s' was modified.\n", event->name );
//	//						}
//	//						else
//	//						{
//	//							printf( "The file '%s' was modified.\n", event->name );
//	//						}
//						}

						//TODO: renaming
					}

					i += EVENT_SIZE + event->len;
					printf("i: %d\n", i);
				}


				if (old_name)
				{
					notify_rename_event_args(change_types::renamed, directory_name, "", *old_name);
					old_name.reset();
				}



			}
		}
	}

protected:


	//TODO: las tres funciones siguientes están duplicadas en windows_impl y linux_impl -> RESOLVER
	inline void notify_file_system_event_args( int action, const std::string& directory, const std::string& name )
	{
		//TODO: ver en .Net
		//if (!MatchPattern(name))
		//{
		//	return;
		//}

		std::cout << "-------------------------------------------- action: " << action << std::endl;

		if (action & IN_CREATE)
		{
			do_callback(created_handler_, filesystem_event_args(change_types::created, directory, name));
		}
		else if ( action & IN_DELETE )
		{
			do_callback(deleted_handler_, filesystem_event_args(change_types::deleted, directory, name));
		}
		else if ( action & IN_MODIFY )
		{
			do_callback(changed_handler_, filesystem_event_args(change_types::changed, directory, name));
		}
		else
		{
			//TODO:
			//Debug.Fail("Unknown FileSystemEvent action type!  Value: " + action);
		}
	}

	//TODO:
	//private void NotifyInternalBufferOverflowEvent()
	//{
	//	InternalBufferOverflowException ex = new InternalBufferOverflowException(SR.GetString(SR.FSW_BufferOverflow, directory));

	//	ErrorEventArgs errevent = new ErrorEventArgs(ex);

	//	OnError(errevent);
	//}


	inline void notify_rename_event_args(int action, const std::string& directory, const std::string& name, const std::string& old_name)
	{
		//filter if neither new name or old name are a match a specified pattern

		//TODO:
		//if (!MatchPattern(name) && !MatchPattern(oldName))
		//{
		//	return;
		//}

		do_callback(renamed_handler_, renamed_event_args(action, directory, name, old_name));
	}



	HeapThread thread_;

	bool is_initialized_;
	int file_descriptor_; // file descriptor
	bool closing_;


	//typedef boost::bimap<boost::uint32_t, std::string> watch_descriptors_type;
	//typedef boost::bimap<std::string, boost::uint32_t> watch_descriptors_type;
	//typedef boost::bimap< std::string, boost::bimaps::list_of<boost::uint32_t> > watch_descriptors_type;

	typedef std::pair<std::string, boost::uint32_t> pair_type;
	typedef std::vector<pair_type> watch_descriptors_type;
	
	watch_descriptors_type watch_descriptors_;
	
};


} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_LINUX_IMPL_HPP




//int j;
//for (j = 0; j<length; j++)
//{
//	printf("buffer[j]: %d\n", buffer[j]);
//}
