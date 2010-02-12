// http://en.wikipedia.org/wiki/Kqueue
// http://mark.heily.com/pnotify/
/*
pnotify has been ported to the following platforms:

* Linux 2.6
* OpenBSD 4.0
* NetBSD 3.1
* FreeBSD 6.2
* MacOS/X 10.4 

There are platforms that are not supported due to lack of developer resources. Interested parties with access to these platforms are encouraged to port the library to their platform. For example:

* Sun OpenSolaris will soon get a similar API called FEM that could be supported.
* Microsoft Windows has a filesystem event notification mechanism 
*/


#ifndef BOOST_OS_SERVICES_DETAIL_FREEBSD_IMPL_HPP
#define BOOST_OS_SERVICES_DETAIL_FREEBSD_IMPL_HPP

#include <string>

// C-Std Headers
#include <cerrno>		//<errno.h>
#include <cstdio>		//<stdio.h>
#include <cstdlib>		//<stdlib.h>
#include <cstring>		//<string.h>		// for strerror

//#include <sys/types.h>
//#include <sys/inotify.h>
#include <sys/event.h>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/integer.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>

#include <boost/os_services/change_types.hpp>
#include <boost/os_services/details/base_impl.hpp>
#include <boost/os_services/notify_filters.hpp>

//TODO: ver como arreglamos esto...
#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )


namespace boost {
namespace os_services {
namespace detail {

//TODO: si es necesario para todas las implementaciones, pasar a base_impl
typedef boost::shared_ptr<boost::thread> HeapThread; //TODO: cambiar nombre

//TODO: ver que pasa con NetBSD, OpenBSD, etc...

class freebsd_impl : public base_impl<freebsd_impl>
{
public:

	freebsd_impl()
		: is_initialized_(false), closing_(false), file_descriptor_(0)
	{}

	~freebsd_impl()
	{
		closing_ = true;

		if ( thread_ )
		{
			thread_->join();
		}

		if ( file_descriptor_ != 0 )
		{
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
		watch_descriptors_.push_back(std::make_pair(dir_name, 0));
	}

	//void remove_directory_impl(const std::string& dir_name) // throw (std::invalid_argument);

	void start()
	{
		if (!is_initialized_)
		{
			file_descriptor_ = ::kqueue();
			if (file_descriptor_ < 0)
			{
				std::ostringstream oss;
				oss << "Failed to initialize monitor - Reason: " << std::strerror(errno);
				throw (std::runtime_error(oss.str()));
			}
			is_initialized_ = true;
			return 0;

		}

		for (watch_descriptors_type::iterator it =  watch_descriptors_.begin(); it != watch_descriptors_.end(); ++it )
		{
			boost::uint32_t watch_descriptor = 0
			if (watch_descriptor < 0)
			{
				std::ostringstream oss;
				oss << "Failed to monitor directory - Directory: " << it->first << " - Reason: " << std::strerror(errno);
				throw (std::invalid_argument(oss.str()));
			}
			it->second = watch_descriptor;


			// watch->wd ES IGUAL A watch_descriptor

			// TENEMOS UN FILE DESCRIPTOR POR WATCH
			/* Open the file */
			if ((watch->fd = open(watch->path, O_RDONLY)) < 0) 
			{
				warn("opening path `%s' failed", watch->path);
				return -1;
			}

			//TODO: esto se hace con boost::filesystem
			/* Test if the file is a directory */
			if (fstat(watch->fd, &st) < 0) 
			{
				warn("fstat(2) failed");
				return -1;
			}
			watch->is_dir = S_ISDIR(st.st_mode);

			/* Initialize the directory structure, if needed */
			if (watch->is_dir) 
			{
				directory_open(ctl, watch);
			}

			/* Generate a new watch ID */
			/* FIXME - this never decreases and might fail */
			if ((watch->wd = ++ctl->next_wd) > WATCH_MAX) 
			{
				warn("watch_max exceeded");
				return -1;
			}

			/* Create and populate a kevent structure */
			EV_SET(kev, watch->fd, EVFILT_VNODE, EV_ADD | EV_CLEAR, 0, 0, watch);
			if (mask & PN_ACCESS || mask & PN_MODIFY)
				kev->fflags |= NOTE_ATTRIB;
			if (mask & PN_CREATE)
				kev->fflags |= NOTE_WRITE;
			if (mask & PN_DELETE)
				kev->fflags |= NOTE_DELETE | NOTE_WRITE;
			if (mask & PN_MODIFY)
				kev->fflags |= NOTE_WRITE | NOTE_TRUNCATE | NOTE_EXTEND;
			if (mask & PN_ONESHOT)
				kev->flags |= EV_ONESHOT;

			/* Add the kevent to the kernel event queue */
			if (kevent(ctl->fd, kev, 1, NULL, 0, NULL) < 0) 
			{
				perror("kevent(2)");
				return -1;
			}


		}

		thread_.reset( new boost::thread( boost::bind(&freebsd_impl::handle_directory_changes, this) ) );
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

		while ( !closing_ )
		{
			//printf("-- antes del read --\n");
			char buffer[BUF_LEN];
			int i = 0;
			int length = ::read( file_descriptor_, buffer, BUF_LEN );

			//printf("length: %d\n", length);
			//print_buffer(buffer, length);

			if (! closing_)
			{
				if ( length < 0 )
				{
					//TODO:
					perror( "read" );
				}

				boost::optional<std::string> old_name;
				std::string directory_name;


				//printf("i: %d\n", i);
				while ( i < length )
				{
					//printf("dentro de ... while ( i < length ) \n");


					struct kevent kev;

					struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ]; //TODO:
					//event = reinterpret_cast<struct inotify_event*> (buffer_ + bytes_processed);




					if ( event->len ) //TODO: que espera hacer acá, mala práctica
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
								//TODO: en este caso puede ser que se haya movido a otra carpeta no monitoreada, entonces sería un DELETE?
								//notify_rename_event_args(change_types::renamed, directory_name, "", *old_name);
								notify_file_system_event_args( change_types::deleted, directory_name, *old_name);
								old_name.reset();
							}

							notify_file_system_event_args( event->mask, directory_name, file_name);
						}
					}

					i += EVENT_SIZE + event->len;
					//printf("--- while end -- i: %d\n", i);
				}

				if (old_name)
				{
					//TODO: en este caso puede ser que se haya movido a otra carpeta no monitoreada
					//notify_rename_event_args(change_types::renamed, directory_name, "", *old_name);
					notify_file_system_event_args( change_types::deleted, directory_name, *old_name);
					old_name.reset();
				}
			}
		}
	}

protected:


	//TODO: las tres funciones siguientes están duplicadas en windows_impl y freebsd_impl -> RESOLVER
	inline void notify_file_system_event_args( int action, const std::string& directory, const std::string& name )
	{
		//TODO: ver en .Net
		//if (!MatchPattern(name))
		//{
		//	return;
		//}

		//std::cout << "-------------------------------------------- action: " << action << std::endl;


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

	typedef std::pair<std::string, boost::uint32_t> pair_type;
	typedef std::vector<pair_type> watch_descriptors_type;
	
	watch_descriptors_type watch_descriptors_;
	
};


} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_FREEBSD_IMPL_HPP
