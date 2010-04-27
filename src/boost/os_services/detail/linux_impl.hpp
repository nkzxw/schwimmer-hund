//TODO: hacer build que no dependa de la librería dinamica
//TODO: comentarios...
//TODO: probar cuando se mueve un archivo a otra directorio monitoreada con otro Watch
//TODO: probar cuando se mueve un archivo a otra directorio NO monitoreada 

//TODO: Anteriormente a iNotify existía dNotify. Ver de usarlo cuando la version de Linux sea la indicada. http://en.wikipedia.org/wiki/Dnotify

#ifndef BOOST_OS_SERVICES_DETAIL_LINUX_IMPL_HPP
#define BOOST_OS_SERVICES_DETAIL_LINUX_IMPL_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif

#include <string>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/integer.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>

#include <boost/os_services/change_types.hpp>
#include <boost/os_services/detail/base_impl.hpp>
#include <boost/os_services/detail/inotify_wrapper.hpp>
#include <boost/os_services/notify_filters.hpp>


namespace boost {
namespace os_services {
namespace detail {

//TODO: si es necesario para todas las implementaciones, pasar a base_impl
typedef boost::shared_ptr<boost::thread> thread_type;

class linux_impl : public base_impl<linux_impl>
{
public:

	linux_impl()
		: is_initialized_(false), closing_(false) //, file_descriptor_(0)
	{}

	~linux_impl()
	{
		closing_ = true;

		//if ( file_descriptor_ != 0 )
		//{
		//	BOOST_FOREACH(pair_type p, watch_descriptors_)
		//	{
		//		if ( p.second != 0 )
		//		{
		//			//int ret_value = ::inotify_rm_watch( file_descriptor_, p.second );

		//			//if ( ret_value == -1 ) //TODO: constante POSIX_ERROR o algo asi... IDEM FreeBSD
		//			//{
		//			//	//Destructor -> NO_THROW
		//			//	std::cerr << "Failed to remove watch - Reason: " << std::strerror(errno) << std::endl;
		//			//}

		//			try
		//			{
		//				inotify.remove_watch( p.second );
		//			}
		//			catch ( const std::runtime_error& e )
		//			{
		//				//Destructor -> NO_THROW
		//				std::cerr << e.what() << std::endl;
		//			}
		//		}
		//	}

		BOOST_FOREACH(pair_type p, watch_descriptors_)
		{
			if ( p.second != 0 )
			{
				//int ret_value = ::inotify_rm_watch( file_descriptor_, p.second );

				//if ( ret_value == -1 ) //TODO: constante POSIX_ERROR o algo asi... IDEM FreeBSD
				//{
				//	//Destructor -> NO_THROW
				//	std::cerr << "Failed to remove watch - Reason: " << std::strerror(errno) << std::endl;
				//}

				try
				{
					inotify.remove_watch( p.second );
				}
				catch ( const std::runtime_error& e )
				{
					//Destructor -> NO_THROW
					std::cerr << e.what() << std::endl;
				}
			}
		}

		//// TODO: parece que close(0) cierra el standard input (CIN / stdin)
		//int ret_value = ::close( file_descriptor_ );

		//if ( ret_value < 0 )
		//{
		//	//Destructor -> NO_THROW
		//	std::cerr << "Failed to close inotify file descriptor - Reason: " << std::strerror(errno) << std::endl;
		//}

		try
		{
			inotify.close();
		}
		catch ( const std::runtime_error& e )
		{
			//Destructor -> NO_THROW
			std::cerr << e.what() << std::endl;
		}

		if ( thread_ )
		{
			thread_->join(); //TODO: darle un timeout al close, sino llegó a cerrarse hacer un kill...
		}
	}

	void add_watch_impl( const std::string& dir_name ) //throw (std::invalid_argument, std::runtime_error)
	{
		watch_descriptors_.push_back( std::make_pair( dir_name, 0 ) );
	}

	void add_watch_impl( const boost::filesystem::path& dir ) //throw (std::invalid_argument, std::runtime_error)
	{
		watch_descriptors_.push_back( std::make_pair( dir, 0 ) );
	}

	//TODO: agregar + la variante de boost::path
	//void remove_watch_impl( const std::string& dir_name ) // throw (std::invalid_argument);



	void start()
	{
		initialize();

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
			//TODO: ver si estos atributos como "IN_MODIFY" deben ir fijos o seteados desde afuera. Tienen que ser asignados por el usuario de acuerdo a lo que quiere monitorear... IDEM Windows... ver como se hace para espeficarle esto en Windows...
			//boost::uint32_t watch_descriptor = ::inotify_add_watch(file_descriptor_, it->first.c_str(), IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);
			//if ( watch_descriptor == -1 )
			//{
			//	std::ostringstream oss;
			//	oss << "Failed to monitor directory - Directory: " << it->first << " - Reason: " << std::strerror(errno);
			//	throw (std::invalid_argument(oss.str()));
			//}
			//it->second = watch_descriptor;

			it->second = inotify.add_watch( it->first.c_str() );

		}


//		BOOST_FOREACH(pair_type p, watch_descriptors_)
//		{
//			std::cout << "p.first: " << p.first << std::endl;
//			std::cout << "p.second: " << p.second << std::endl;
//		}


		thread_.reset( new boost::thread( boost::bind(&linux_impl::handle_directory_changes, this) ) );
	}

public: //private:  //TODO:

	//TODO: ver si hace falta hacer lo mismo para Windows
	void initialize() //private
	{
		if (!is_initialized_)
		{
			//file_descriptor_ = ::inotify_init();
			//if (file_descriptor_ < 0)
			//{
			//	std::ostringstream oss;
			//	oss << "Failed to initialize monitor (inotify_init) - Reason: " << std::strerror(errno);
			//	throw (std::runtime_error(oss.str()));
			//}

			inotify.initialize();

			is_initialized_ = true;
		}

	}

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
			struct inotify_event* event = inotify.get<struct inotify_event>();

			if ( ! closing_ )
			{

				//if ( event->len != 0) //TODO: que espera hacer acá?, mala práctica
				if ( event->len ) //TODO: que espera hacer acá?, mala práctica
				{
					std::string directory_name;
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
							//TODO: en este caso puede ser que se haya movido a otro directorio no monitoreada, entonces sería un DELETE?
							//notify_rename_event_args(change_types::renamed, directory_name, "", *old_name);
							notify_file_system_event_args( change_types::deleted, directory_name, *old_name);
							old_name.reset();
						}
						notify_file_system_event_args( event->mask, directory_name, file_name);
					}
				}
			}
		}
	}


	//void handle_directory_changes()
	//{
	//	boost::optional<std::string> old_name;

	//	while ( !closing_ )
	//	{
	//		char buffer[BUF_LEN];
	//		int i = 0;

	//		int length = ::read( file_descriptor_, buffer, BUF_LEN );

	//		if ( ! closing_ )
	//		{
	//			if ( length < 0 )
	//			{
	//				//TODO:
	//				perror( "read" );
	//			}

	//			std::string directory_name;

	//			while ( i < length )
	//			{

	//				struct inotify_event* event = ( struct inotify_event * ) &buffer[ i ]; //TODO:
	//				//event = reinterpret_cast<struct inotify_event*> (buffer_ + bytes_processed);

	//				//if ( event->len != 0) //TODO: que espera hacer acá?, mala práctica
	//				if ( event->len ) //TODO: que espera hacer acá?, mala práctica
	//				{

	//					std::string file_name( event->name );

	//					watch_descriptors_type::const_iterator it = std::find_if( watch_descriptors_.begin(), watch_descriptors_.end(), boost::bind( &pair_type::second, _1 ) == event->wd );

	//					if ( it != watch_descriptors_.end() )
	//					{
	//						directory_name = it->first;
	//					}
	//					else
	//					{
	//						//TODO: que pasa si no lo encontramos en la lista... DEBERIA SER UN RUN-TIME ERROR
	//					}

	//					if ( event->mask & IN_MOVED_FROM )
	//					{
	//						old_name.reset( file_name );
	//					}
	//					else if ( event->mask & IN_MOVED_TO )
	//					{
	//						if ( old_name )
	//						{
	//							notify_rename_event_args(change_types::renamed, directory_name, file_name, *old_name);
	//							old_name.reset();
	//						}
	//						else
	//						{
	//							notify_rename_event_args(change_types::renamed, directory_name, file_name, "");
	//							old_name.reset();
	//						}
	//					}
	//					else
	//					{
	//						if ( old_name )
	//						{
	//							//std::cout << "------------- VER -------------" << std::endl;
	//							//std::cout << "file_name: '" << file_name << "'" << std::endl;
	//							//std::cout << "*old_name: '" << *old_name << "'" << std::endl;

	//							//TODO: en este caso puede ser que se haya movido a otro directorio no monitoreada, entonces sería un DELETE?
	//							//notify_rename_event_args(change_types::renamed, directory_name, "", *old_name);
	//							notify_file_system_event_args( change_types::deleted, directory_name, *old_name);
	//							old_name.reset();
	//						}

	//						//if ( event->mask & IN_MODIFY )
	//						//{
	//						//	std::cout << "------------- IN_MODIFY -----------------" << std::endl;
	//						//}

	//						notify_file_system_event_args( event->mask, directory_name, file_name);
	//					}
	//				}

	//				i += EVENT_SIZE + event->len;
	//				//printf("--- while end -- i: %d\n", i);
	//			}
	//		}
	//	}
	//}

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
	//int file_descriptor_; // file descriptor
	bool closing_;

	typedef std::pair<std::string, boost::uint32_t> pair_type;
	typedef std::vector<pair_type> watch_descriptors_type;
	
	watch_descriptors_type watch_descriptors_;


protected:

	inotify_wrapper inotify;
};


} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_LINUX_IMPL_HPP
