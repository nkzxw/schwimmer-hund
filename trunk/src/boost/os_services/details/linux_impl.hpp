//TODO: hacer build que no dependa de la librería dinamica
//TODO: comentarios...
//TODO: probar cuando se mueve un archivo a otra directorio monitoreada con otro Watch
//TODO: probar cuando se mueve un archivo a otra directorio NO monitoreada 


#ifndef BOOST_OS_SERVICES_DETAIL_LINUX_IMPL_HPP
#define BOOST_OS_SERVICES_DETAIL_LINUX_IMPL_HPP

#include <string>

// C-Std Headers
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>		// for strerror

#include <sys/inotify.h>
#include <sys/types.h>

//#include <boost/bimap.hpp>
//#include <boost/bimap/list_of.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/integer.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>

#include <boost/os_services/change_types.hpp>
#include <boost/os_services/details/base_impl.hpp>
#include <boost/os_services/notify_filters.hpp>

//TODO: ver como arreglamos esto...
//TODO: ver la posibilidad de que el buffer pueda ser seteado desde afuera...
#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

namespace boost {
namespace os_services {
namespace detail {

//TODO: si es necesario para todas las implementaciones, pasar a base_impl
typedef boost::shared_ptr<boost::thread> thread_type;

class linux_impl : public base_impl<linux_impl>
{
public:

	linux_impl()
		: is_initialized_(false), closing_(false), file_descriptor_(0)
	{}

	~linux_impl()
	{
		closing_ = true;

		if ( file_descriptor_ != 0 )
		{
			BOOST_FOREACH(pair_type p, watch_descriptors_)
			{
				if ( p.second != 0 )
				{
					int ret_value = ::inotify_rm_watch( file_descriptor_, p.second );

					if ( ret_value < 0 )
					{
						//TODO: analizar si esta es la forma optima de manejar errores.
						std::ostringstream oss;
						oss << "Failed to remove watch - Reason: "; //TODO: ver que usar en Linux/BSD << GetLastError();
						throw (std::runtime_error(oss.str()));
					}

				}
			}

			// TODO: parece que close(0) cierra el standard input (CIN / stdin)
			int ret_value = ::close( file_descriptor_ );

			if ( ret_value < 0 )
			{
				std::ostringstream oss;
				oss << "Failed to close file descriptor - Reason: "; //TODO: ver que usar en Linux/BSD << GetLastError();
				throw (std::runtime_error(oss.str()));
			}
		}

		if ( thread_ )
		{
			thread_->join(); //TODO: darle un timeout al close, sino llegó a cerrarse hacer un kill...
		}

	}

	void add_directory_impl ( const std::string& dir_name ) //throw (std::invalid_argument, std::runtime_error)
	{
		watch_descriptors_.push_back(std::make_pair(dir_name, 0));
	}

	//void remove_directory_impl(const std::string& dir_name) // throw (std::invalid_argument);

	void start()
	{
		if (!is_initialized_)
		{
			file_descriptor_ = ::inotify_init();
			//std::cout << "file_descriptor_: " << file_descriptor_ << std::endl;

			if (file_descriptor_ < 0)
			{
				std::ostringstream oss;
				oss << "Failed to initialize monitor - Reason: " << std::strerror(errno);
				throw (std::runtime_error(oss.str()));
			}
			is_initialized_ = true;
		}

//		BOOST_FOREACH(pair_type p, watch_descriptors_)
//		{
//			//TODO: ver si estos atributos como "IN_MODIFY" deben ir fijos o seteados desde afuera.
//			boost::uint32_t watch_descriptor = ::inotify_add_watch(file_descriptor_, p.first.c_str(), IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);
//			//if (watch_descriptor == -1)
//			if (watch_descriptor < 0)
//			{
//				std::ostringstream oss;
//				oss << "Failed to monitor directory - Directory: " << p.first << " - Reason: " << std::strerror(errno);
//				throw (std::invalid_argument(oss.str()));
//			}
//
//			std::cout << "watch_descriptor: " << watch_descriptor << std::endl;
//			p.second = watch_descriptor;
//		}

		for (watch_descriptors_type::iterator it =  watch_descriptors_.begin(); it != watch_descriptors_.end(); ++it )
		{
			//TODO: ver si estos atributos como "IN_MODIFY" deben ir fijos o seteados desde afuera.
			boost::uint32_t watch_descriptor = ::inotify_add_watch(file_descriptor_, it->first.c_str(), IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);
			//std::cout << "watch_descriptor: " << watch_descriptor << std::endl;

			//if (watch_descriptor == -1)
			if (watch_descriptor < 0)
			{
				std::ostringstream oss;
				oss << "Failed to monitor directory - Directory: " << it->first << " - Reason: " << std::strerror(errno);
				throw (std::invalid_argument(oss.str()));
			}

			//std::cout << "watch_descriptor: " << watch_descriptor << std::endl;
			it->second = watch_descriptor;
		}


//		BOOST_FOREACH(pair_type p, watch_descriptors_)
//		{
//			std::cout << "p.first: " << p.first << std::endl;
//			std::cout << "p.second: " << p.second << std::endl;
//		}


		thread_.reset( new boost::thread( boost::bind(&linux_impl::handle_directory_changes, this) ) );
	}

public: //private:  //TODO:

	//void print_buffer(char* buffer, unsigned long num_bytes) //, DWORD buffer_length)
	//{
	//	printf("%d bytes: \n", num_bytes);

	//	for (int i = 0; i<num_bytes; ++i)
	//	{
	//		printf("%u ", (unsigned int)buffer[i]);
	//	}

	//	printf("\n");
	//}

	void handle_directory_changes()
	{
		boost::optional<std::string> old_name;

		while ( !closing_ )
		{
			//printf("-- antes del read --\n");
			char buffer[BUF_LEN];
			int i = 0;

			//boost::system_time time = boost::get_system_time();
			//time += boost::posix_time::milliseconds(3000);
			//boost::thread::sleep(time);

			int length = ::read( file_descriptor_, buffer, BUF_LEN );

			//printf("length: %d\n", length);
			//print_buffer(buffer, length);

			if ( ! closing_ )
			{
				if ( length < 0 )
				{
					//TODO:
					perror( "read" );
				}

				std::string directory_name;


				//printf("i: %d\n", i);
				while ( i < length )
				{
					//printf("dentro de ... while ( i < length ) \n");

					struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ]; //TODO:
					//event = reinterpret_cast<struct inotify_event*> (buffer_ + bytes_processed);

					//printf("event: %d\n", (void*)event);
//					printf("event->wd: %d\n", event->wd);
//					printf("event->mask: %u\n", event->mask);
//					printf("event->cookie: %u\n", event->cookie);
//					printf("event->len: %u\n", event->len);
//					printf("event->name: %s\n", event->name);

//					if ( event->mask & IN_MOVED_FROM )
//					{
//						std::cout << "MOVED FROM" << std::endl;
//					}
//					else if ( event->mask & IN_MOVED_TO )
//					{
//						std::cout << "MOVED TO" << std::endl;
//					}
//					else if ( event->mask & IN_CREATE )
//					{
//						std::cout << "CREATE" << std::endl;
//					}
//					else if ( event->mask & IN_DELETE )
//					{
//						std::cout << "DELETE" << std::endl;
//					}
//					else if ( event->mask & IN_MODIFY )
//					{
//						std::cout << "MODIFY" << std::endl;
//					}
//					else
//					{
//						std::cout << "... NOTHING ..." << std::endl;
//					}

					//if ( event->len != 0) //TODO: que espera hacer acá?, mala práctica
					if ( event->len ) //TODO: que espera hacer acá?, mala práctica
					{

						std::string file_name( event->name );

						watch_descriptors_type::const_iterator it = std::find_if( watch_descriptors_.begin(), watch_descriptors_.end(), boost::bind( &pair_type::second, _1 ) == event->wd );

						if ( it != watch_descriptors_.end() )
						{
							directory_name = it->first;
						}
						else
						{
							//TODO: que pasa si no lo encontramos en la lista... DEBERIA SER UN RUN-TIME ERROR
						}

						if ( event->mask & IN_MOVED_FROM )
						{
							old_name.reset( file_name );
						}
						else if ( event->mask & IN_MOVED_TO )
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
							if ( old_name )
							{
								//std::cout << "------------- VER -------------" << std::endl;
								//std::cout << "file_name: '" << file_name << "'" << std::endl;
								//std::cout << "*old_name: '" << *old_name << "'" << std::endl;

								//TODO: en este caso puede ser que se haya movido a otro directorio no monitoreada, entonces sería un DELETE?
								//notify_rename_event_args(change_types::renamed, directory_name, "", *old_name);
								notify_file_system_event_args( change_types::deleted, directory_name, *old_name);
								old_name.reset();
							}

							//if ( event->mask & IN_MODIFY )
							//{
							//	std::cout << "------------- IN_MODIFY -----------------" << std::endl;
							//}

							notify_file_system_event_args( event->mask, directory_name, file_name);
						}
					}

					i += EVENT_SIZE + event->len;
					//printf("--- while end -- i: %d\n", i);
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

		//std::cout << "-------------------------------------------- action: " << action << std::endl;


		//if (action & IN_CREATE)
		//{
		//	do_callback(created_handler_, filesystem_event_args(change_types::created, directory, name));
		//}
		//else if ( action & IN_DELETE )
		//{
		//	do_callback(deleted_handler_, filesystem_event_args(change_types::deleted, directory, name));
		//}
		//else if ( action & IN_MODIFY )
		//{
		//	do_callback(changed_handler_, filesystem_event_args(change_types::changed, directory, name));
		//}
		//else
		//{
		//	//TODO:
		//	//Debug.Fail("Unknown FileSystemEvent action type!  Value: " + action);
		//}

		

		if ( action == change_types::created )
		{
			do_callback(created_handler_, filesystem_event_args(change_types::created, directory, name));
		}
		else if ( action == change_types::deleted )
		{
			do_callback(deleted_handler_, filesystem_event_args(change_types::deleted, directory, name));
		}
		else if ( action == change_types::changed )
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


	thread_type thread_;

	bool is_initialized_;
	int file_descriptor_; // file descriptor
	bool closing_;

	typedef std::pair<std::string, boost::uint32_t> pair_type;
	typedef std::vector<pair_type> watch_descriptors_type;
	
	watch_descriptors_type watch_descriptors_;
	
};


} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_LINUX_IMPL_HPP
