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
#include <vector>

// C-Std Headers
#include <cerrno>		//<errno.h>
#include <cstdio>		//<stdio.h>
#include <cstdlib>		//<stdlib.h>
#include <cstring>		//<string.h>		// for strerror

#include <fcntl.h>
#include <sys/event.h>
#include <sys/types.h>

#include <boost/bind.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/foreach.hpp>
#include <boost/integer.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>

#include <boost/os_services/change_types.hpp>
#include <boost/os_services/details/base_impl.hpp>
#include <boost/os_services/notify_filters.hpp>

//TODO: ver como arreglamos esto...
//#define EVENT_SIZE  ( sizeof (struct inotify_event) )
//#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )


//TODO: sacar
static const int WATCH_MAX = 20000;


//TODO: sacar
enum {

	/** The atime of a file has been modified */
	PN_ACCESS 		= 0x1 << 0,

	/** A file was created in a watched directory */
	PN_CREATE		= 0x1 << 1,

	/** A file was deleted from a watched directory */
	PN_DELETE		= 0x1 << 2,

	/** The modification time of a file has changed */
	PN_MODIFY		= 0x1 << 3,

	/** Automatically delete the watch after a matching event occurs */
	PN_ONESHOT		= 0x1 << 4,

	/** An error condition in the underlying kernel event queue */
	PN_ERROR		= 0x1 << 5,

} __PN_BITMASK;

#define PN_ALL_EVENTS	(PN_ACCESS | PN_CREATE | PN_DELETE | PN_MODIFY)

//TODO: sacar
/* kqueue(4) in MacOS/X does not support NOTE_TRUNCATE */
#ifndef NOTE_TRUNCATE
# define NOTE_TRUNCATE 0
#endif


namespace boost {
namespace os_services {
namespace detail {


struct watch_data
{
	int               fd;	/**< kqueue(4) watched file descriptor */
	int               wd;	/**< Unique 'watch descriptor' */
	//char            path[255 + 1];	//path[PATH_MAX + 1];	/**< Path associated with fd */
	struct kevent     kev;
	boost::uint32_t   mask;	/**< Mask of monitored events */
};


struct fsentry
{
	fs::path full_path;
	int fd;					/**< kqueue(4) watched file descriptor */
	//int wd;	/**< Unique 'watch descriptor' */
	boost::uint32_t        mask;	/**< Mask of monitored events */


	std::vector<fsentry> subitems;
};


//struct directory
//{
//
//	boost::filesystem::path path;
////	char    *path;
////	size_t   path_len;
////
////	/** A directory handle returned by opendir(3) */
////	DIR     *dirp;
//
//	/* All entries in the directory (struct dirent) */
//	LIST_HEAD(, dentry) all;
//};


void fill_file_system(fsentry& head_dir)
{
	// A esta algura ya se que el path existe y es un directorio
//	if ( !fs::exists( head_dir.full_path ) )
//	{
//		std::cout << "\nNot found: " << head_dir.full_path.file_string() << std::endl;
//		return;
//	}
//	if ( ! fs::is_directory( head_dir.full_path ) )
//	{
//		std::cout << "\nFound: " << head_dir.full_path.file_string() << "\n";
//		return;
//	}

	fs::directory_iterator end_iter;
	for ( fs::directory_iterator dir_itr( head_dir.full_path ); dir_itr != end_iter; ++dir_itr )
	{
		try
		{
			directory child;
			child.full_path = dir_itr->path();

			if ( fs::is_directory( dir_itr->status() ) )
			{
				fill_file_system(child);
			}
			head_dir.subitems.push_back(child);
		}
		catch ( const std::exception & ex )
		{
			std::cout << dir_itr->path().filename() << " " << ex.what() << std::endl;
		}
	}

}



//TODO: si es necesario para todas las implementaciones, pasar a base_impl
typedef boost::shared_ptr<boost::thread> HeapThread; //TODO: cambiar nombre

//TODO: ver que pasa con NetBSD, OpenBSD, etc...

class freebsd_impl : public base_impl<freebsd_impl>
{
public:

	freebsd_impl()
		: is_initialized_(false), closing_(false), file_descriptor_(0), next_watch_(0)
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
					//int ret_value = ::inotify_rm_watch( file_descriptor_, p.second );
					int ret_value = 0;

					if ( ret_value < 0 )
					{
						//TODO: analizar si esta es la forma optima de manejar errores.
						std::ostringstream oss;
						oss << "Failed to remove watch - Reason: "; //TODO: ver que usar en Linux/BSD << GetLastError();
						throw (std::runtime_error(oss.str()));
					}
				}
			}

			// TODO: parece que close(0) cierra el standard input (CIN)
			//int ret_value = ::close( file_descriptor_ );
			int ret_value = 0;

			if ( ret_value < 0 )
			{
				std::ostringstream oss;
				oss << "Failed to close file descriptor - Reason: "; //TODO: ver que usar en Linux/BSD << GetLastError();
				throw (std::runtime_error(oss.str()));
			}
		}
	}

	void add_directory_impl ( const std::string& dir_name ) //throw (std::invalid_argument, std::runtime_error)
	{
		watch_descriptors_.push_back(std::make_pair(dir_name, 0));
	}

	//void remove_directory_impl(const std::string& dir_name) // throw (std::invalid_argument);


	//TODO: hacer lo mismo para linux.
	void initialize() //private
	{
		std::cout << "void initialize()" << std::endl;

		if (!is_initialized_)
		{
			std::cout << "file_descriptor_ = kqueue();" << std::endl;

			file_descriptor_ = kqueue(); //::kqueue();
			if (file_descriptor_ < 0)
			{
				std::ostringstream oss;
				oss << "Failed to initialize monitor - Reason: " << std::strerror(errno);
				throw (std::runtime_error(oss.str()));
			}
			is_initialized_ = true;
		}
	}


	//TODO: hacer lo mismo para Linux
	void create_watchs() //TODO: watches? //TODO: private
	{
		for (watch_descriptors_type::iterator it =  watch_descriptors_.begin(); it != watch_descriptors_.end(); ++it )
		{
			std::cout << "inside create_watchs LOOP" << std::endl;

			//watch_data watch;
			watch_data *watch = new watch_data;

			//TODO: ver esto...
			watch->mask = PN_ALL_EVENTS;

			//TODO: en el constructor
			watch->wd = 0;
			struct kevent *kev = &watch->kev;
			int mask = watch->mask;

			if (watch->wd < 0)
			{
				std::ostringstream oss;
				oss << "Failed to monitor directory - Directory: " << it->first << " - Reason: " << std::strerror(errno);
				throw (std::invalid_argument(oss.str()));
			}
			it->second = watch->wd;


			// watch->wd ES IGUAL A watch_descriptor


			std::cout << "FILE: " << it->first << std::endl;

			// TENEMOS UN FILE DESCRIPTOR POR WATCH
			//if ((watch->fd = open(watch->path, O_RDONLY)) < 0) 
			if ( (watch->fd = open( it->first.c_str(), O_RDONLY )) < 0)
			{
				//warn("opening path `%s' failed", watch->path);
				//return -1;

				std::ostringstream oss;
				//TODO:
				oss << "opening path failed: - Reason: " << std::strerror(errno);
				throw (std::invalid_argument(oss.str()));

			}


			//TODO: esto se hace con boost::filesystem
			///* Test if the file is a directory */
			//if ( fstat(watch->fd, &st) < 0) 
			//{
			//	warn("fstat(2) failed");
			//	return -1;
			//}
			//watch->is_dir = S_ISDIR(st.st_mode);
			///* Initialize the directory structure, if needed */
			//if (watch->is_dir) 
			//{
			//	directory_open(ctl, watch);
			//}

			//TODO: ver que hacer y probar que pasa si no lo incluimos...
			// directory_open(ctl, watch);

			/* Generate a new watch ID */
			/* FIXME - this never decreases and might fail */
			//if ((watch->wd = ++ctl->next_wd) > WATCH_MAX) 
			
			if ( (watch->wd = ++next_watch_) > WATCH_MAX )
			{
				//warn("watch_max exceeded");
				//return -1;
				std::ostringstream oss;
				//TODO:
				oss << "watch_max exceeded";
				throw (std::invalid_argument(oss.str()));
			}

			/* Create and populate a kevent structure */
			//EV_SET(kev, watch->fd, EVFILT_VNODE, EV_ADD | EV_CLEAR, 0, 0, watch);
			EV_SET(kev, watch->fd, EVFILT_VNODE, EV_ADD | EV_CLEAR, 0, 0, watch);

			//TODO: ver esto...
			if (mask & PN_ACCESS || mask & PN_MODIFY)
			{
				kev->fflags |= NOTE_ATTRIB;
			}

			if (mask & PN_CREATE)
			{
				kev->fflags |= NOTE_WRITE;
			}

			if (mask & PN_DELETE)
			{
				kev->fflags |= NOTE_DELETE | NOTE_WRITE;
			}

			if (mask & PN_MODIFY)
			{
				kev->fflags |= NOTE_WRITE | NOTE_TRUNCATE | NOTE_EXTEND;
			}

			if (mask & PN_ONESHOT)
			{
				kev->flags |= EV_ONESHOT;
			}

			std::cout << "kev->flags: " << kev->flags << std::endl;

			/* Add the kevent to the kernel event queue */
			//if (kevent(ctl->fd, kev, 1, NULL, 0, NULL) < 0) 
			if (kevent(file_descriptor_, kev, 1, NULL, 0, NULL) < 0) 
			{
				//perror("kevent(2)");
				//return -1;


				std::ostringstream oss;
				//TODO:
				oss << "kevent(2): - Reason: " << std::strerror(errno);
				throw (std::invalid_argument(oss.str()));
			}
		}
	}

	void start()
	{
		std::cout << "void start()" << std::endl;
		initialize();
		create_watchs();

		thread_.reset( new boost::thread( boost::bind(&freebsd_impl::handle_directory_changes, this) ) );
	}

public: //private:  //TODO:

	void handle_directory_changes()
	{
		while ( !closing_ )
		{

			watch_data *watch; //struct pnotify_watch *watch;
//			struct pnotify_event *evp;
			struct kevent   kev;
			int rc;

			/* Wait for an event notification from the kernel */
			std::cout << "waiting for kernel event.." << std::endl;

			//printf("-- antes del kevent (read) --\n");

			//TODO: ver timeout
			//rc = kevent(ctl->fd, NULL, 0, &kev, 1, NULL);
			rc = kevent(file_descriptor_, NULL, 0, &kev, 1, NULL);
			if ((rc < 0) || (kev.flags & EV_ERROR))
			{
				//TODO:
				//warn("kevent(2) failed");
				std::cout << "ERRRRRRRRRRRRRORRR ON kevent Wait" << std::endl;
				return;
			}

			std::cout << "kev.ident: " << kev.ident << std::endl;
			std::cout << "kev.filter: " << kev.filter << std::endl;
			std::cout << "kev.flags: " << kev.flags << std::endl;
			std::cout << "kev.fflags: " << kev.fflags << std::endl;
			std::cout << "kev.data: " << kev.data << std::endl;
			std::cout << "kev.udata: " << kev.udata << std::endl;

//		     struct kevent {
//		             uintptr_t       ident;          /* identifier for this event */
//		             int16_t         filter;         /* filter for event */
//		             uint16_t        flags;          /* general flags */
//		             uint32_t        fflags;         /* filter-specific flags */
//		             intptr_t        data;           /* filter-specific data */
//		             void            *udata;         /* opaque user data identifier */
//		     };



			std::cout << "Luego del IF" << std::endl;

			//printf("length: %d\n", length);
			//print_buffer(buffer, length);

			if (! closing_)
			{
				//watch = (struct pnotify_watch *) kev.udata;
				watch = (watch_data*) kev.udata;

				std::cout << "watch->fd: " << watch->fd << std::endl;
				std::cout << "watch->wd: " << watch->wd << std::endl;
				std::cout << "watch->mask: " << watch->mask << std::endl;
				//std::cout << "watch.fd: " << watch.fd << std::endl;

				//TODO: ver este tema, porque se puede estar disparando contra el directorui
//				if (watch->parent_wd && kev.fflags & NOTE_DELETE)
//				{
//					dprintf("ignoring NOTE_DELETE on a watched file\n");
//					goto retry;
//				}


				if (kev.fflags & NOTE_WRITE)
				{
					//evt->mask |= PN_MODIFY;
					std::cout << "NOTE_WRITE -> PN_MODIFY" << std::endl;
				}
				if (kev.fflags & NOTE_TRUNCATE)
				{
					//evt->mask |= PN_MODIFY;
					std::cout << "NOTE_TRUNCATE -> PN_MODIFY" << std::endl;
				}
				if (kev.fflags & NOTE_EXTEND)
				{
					//evt->mask |= PN_MODIFY;
					std::cout << "NOTE_EXTEND -> PN_MODIFY" << std::endl;
				}
		#if TODO
				// not implemented yet
				if (kev.fflags & NOTE_ATTRIB)
				{
					//evt->mask |= PN_ATTRIB;
					std::cout << "NOTE_ATTRIB -> PN_ATTRIB" << std::endl;
				}
		#endif
				if (kev.fflags & NOTE_DELETE)
				{
					//evt->mask |= PN_DELETE;
					std::cout << "NOTE_DELETE -> PN_DELETE" << std::endl;
				}

				if (kev.fflags & NOTE_RENAME)
				{
					//evt->mask |= PN_DELETE;
					std::cout << "NOTE_RENAME -> XXXXXXXXX" << std::endl;
				}

				if (kev.fflags & NOTE_REVOKE)
				{
					//evt->mask |= PN_DELETE;
					std::cout << "NOTE_REVOKE -> XXXXXXXXX" << std::endl;
				}

				if (kev.fflags & NOTE_LINK)
				{
					//evt->mask |= PN_DELETE;
					std::cout << "NOTE_LINK -> XXXXXXXXX" << std::endl;
				}




//				/* Construct a pnotify_event structure */
//				if ((evp = calloc(1, sizeof(*evp))) == NULL)
//				{
//					//TODO:
//					//warn("malloc failed");
//					return;
//				}


//				/* If the event happened within a watched directory, add the filename and the parent watch descriptor.
//				*/
//				if (watch->parent_wd)
//				{
//
//					/* KLUDGE: remove the leading basename */
//					char *fn = strrchr(watch->path, '/') ;
//					if (!fn)
//					{
//						fn = watch->path;
//					}
//					else
//					{
//						fn++;
//					}
//
//					evt->wd = watch->parent_wd;
//					/* FIXME: more error handling */
//					(void) strncpy(evt->name, fn, strlen(fn));
//				}
//
//				/* Add the event to the list of pending events */
//				memcpy(evp, evt, sizeof(*evp));
//				STAILQ_INSERT_TAIL(&ctl->event, evp, entries);
//
//				dprint_event(evt);

			}
		}
	}

protected:

	//TODO: las tres funciones siguientes están duplicadas en windows_impl y freebsd_impl -> RESOLVER
	// inline void notify_file_system_event_args( int action, const std::string& directory, const std::string& name )


	HeapThread thread_;

	bool is_initialized_;
	int file_descriptor_; // file descriptor
	int next_watch_;
	bool closing_;



//	typedef std::pair<std::string, boost::uint32_t> pair_type;
	typedef std::pair<std::string, fsentry*> pair_type;
	typedef std::vector<pair_type> watch_descriptors_type;
	
	watch_descriptors_type watch_descriptors_;
	
};


} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_FREEBSD_IMPL_HPP
