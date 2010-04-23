#ifndef BOOST_OS_SERVICES_DETAIL_KQUEUE_WRAPPER_HPP
#define BOOST_OS_SERVICES_DETAIL_KQUEUE_WRAPPER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif

//TODO: revisar los headers

// C-Std Headers
#include <cerrno>	//TODO: probar si es necesario
#include <cstdio>	//TODO: probar si es necesario
#include <cstdlib>	//TODO: probar si es necesario
#include <cstring>	// for strerror

#include <sys/event.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/filesystem/path.hpp>
//#include <boost/foreach.hpp>
#include <boost/function.hpp>
//#include <boost/integer.hpp>
//#include <boost/ptr_container/ptr_vector.hpp>
//#include <boost/smart_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>


#include <boost/os_services/change_types.hpp>
#include <boost/os_services/details/base_impl.hpp>
#include <boost/os_services/details/file_inode_info.hpp>
#include <boost/os_services/details/filesystem_item.hpp>
#include <boost/os_services/details/user_entry.hpp>
#include <boost/os_services/notify_filters.hpp>



//TODO: sacar
/* kqueue(4) in MacOS/X does not support NOTE_TRUNCATE */
#ifndef NOTE_TRUNCATE
# define NOTE_TRUNCATE 0
#endif



namespace boost {
namespace os_services {
namespace detail {




class kqueue_wrapper //TODO: heredar de una clase abstracta
{
public:


	void initialize()
	{
		if ( ! is_initialized_ )
		{
			kqueue_file_descriptor_ = kqueue(); //::kqueue();

			if ( kqueue_file_descriptor_ == -1 )   //< 0)
			{
				std::ostringstream oss;
				oss << "Failed to initialize kqueue - Reason: " << std::strerror(errno);
				throw (std::runtime_error(oss.str()));
			}
			is_initialized_ = true;
		}
	}

	void close ()
	{
		if ( kqueue_file_descriptor_ != 0 )
		{
			int ret_value = ::close( kqueue_file_descriptor_ );
			if ( ret_value == -1 )
			{
				//Destructor -> no-throw
				std::cerr << "Failed to close kqueue file descriptor - File Descriptor: '" << kqueue_file_descriptor_ << "' - Reason: " << std::strerror(errno) << std::endl; 
			}
			kqueue_file_descriptor_ = 0;
		}
	}



	void add_watch( filesystem_item::pointer_type watch, bool launch_events = false )
	{
		struct kevent event;

		//int mask = watch->mask_;
		watch->open(); //TODO: catch errors

		if ( watch->is_directory() )
		{
			scan_directory( watch, launch_events );
		}

		//TODO: traducir de watch->mask_ a fflags
		//TODO: ver estos flags, deberia monitoriarse solo lo que el usuairo quiera monitorear...
		unsigned int fflags = NOTE_DELETE |  NOTE_WRITE | NOTE_EXTEND | NOTE_ATTRIB | NOTE_LINK | NOTE_REVOKE | NOTE_RENAME;

		//EV_SET( &event, watch->file_descriptor_, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_CLEAR, fflags, 0, watch ); // EV_ADD | EV_ENABLE | EV_ONESHOT | EV_CLEAR
		EV_SET( &event, watch->file_descriptor_, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_CLEAR, fflags, 0, watch.get() ); // EV_ADD | EV_ENABLE | EV_ONESHOT | EV_CLEAR

		//TODO: ver si Windows y Linux saltan cuando se mofica el nombre del directorio raiz monitoreado.
		// sino saltan, evisar que se use NOTE_RENAME con cualquier directorio raiz

		// -> Windows no salta...
		// -> Linux tampoco...
		// -> FreeBSD: ????????????

		//		kev->fflags |= NOTE_RENAME; //Eliminado porque no sirve monitorear el dir raiz.
		//		kev->fflags |= NOTE_TRUNCATE; //TODO: ver
		//		kev->fflags |= NOTE_WRITE; //TODO: ver
		//		kev->fflags |= NOTE_EXTEND; //TODO: ver
		//		kev->fflags |= NOTE_ATTRIB; //TODO: ver
		//		kev->fflags |= NOTE_DELETE; //TODO: ver
		//		kev->fflags |= NOTE_RENAME; //TODO: ver
		//		kev->fflags |= NOTE_REVOKE; //TODO: ver
		//		kev->fflags |= NOTE_LINK; //TODO: ver

		//TODO: no está incluido en PN_ALL_EVENTS por eso lo agrego de una
		//TODO: ver como mapear las constantes de BSD con las de la interfaz que vamos a exponer al usuario...
		//TODO: vamos a tener que hacer algo similar a lo que estamos haciendo a continuacion...

		//if (mask & PN_ACCESS || mask & PN_MODIFY)
		//{
		//	event->fflags |= NOTE_ATTRIB;
		//}

		//if (mask & PN_CREATE)
		//{
		//	event->fflags |= NOTE_WRITE;
		//}

		//if (mask & PN_DELETE)
		//{
		//	event->fflags |= NOTE_DELETE | NOTE_WRITE;
		//}

		//if (mask & PN_MODIFY)
		//{
		//	event->fflags |= NOTE_WRITE | NOTE_TRUNCATE | NOTE_EXTEND;
		//}

		//if (mask & PN_ONESHOT)
		//{
		//	event->flags |= EV_ONESHOT;
		//}

		//std::cout << "kev->flags: " << kev->flags << std::endl;

		int return_code = kevent( kqueue_file_descriptor_, &event, 1, NULL, 0, NULL );
		if ( return_code == -1 ) //< 0)
		{
			std::ostringstream oss;
			oss << "kevent error: - Reason: " << std::strerror(errno);
			throw (std::runtime_error(oss.str()));
		}
	}

	void handle_directory_changes()
	{
		struct kevent event;

		struct timespec timeout;
		timeout.tv_sec = 0;
		timeout.tv_nsec = 300000; //300 milliseconds //TODO: sacar el hardcode, hacer configurable...

		//TODO: pasar toda esta logica a un metodo o clase...
		int return_code = kevent ( kqueue_file_descriptor_, NULL, 0, &event, 1, &timeout );

		if ( return_code == -1 || event.flags & EV_ERROR) //< 0
		{
			//TODO: evaluar si este throw está relacionado con el destructor de fsm ya que está ejecutado en otro thread, no deberia... pero...
			std::ostringstream oss;
			oss << "kevent error - Reason: " << std::strerror(errno);
			throw (std::runtime_error(oss.str()));
		}


		if ( return_code == 0 ) //timeout
		{
			if ( queued_write_watch ) // != 0
			{
				handle_write( queued_write_watch );
				queued_write_watch.reset(); // = 0;
			}
		}


		if ( event.fflags & NOTE_DELETE )
		{
			handle_remove( watch );
			queued_write_watch = 0;
		}

		if ( event.fflags & NOTE_RENAME )
		{
			handle_rename( watch );
			queued_write_watch = 0;
		}

		if ( event.fflags & NOTE_WRITE )
		{
			if ( queued_write_watch ) //!= 0
			{
				handle_write( queued_write_watch );
				queued_write_watch.reset(); // = 0;
			}

			//Encolamos un solo evento WRITE ya que siempre viene WRITE+RENAME... hacemos que primero se procese el evento rename y luego el write
			queued_write_watch = watch;
		}


		//if (event.fflags & NOTE_TRUNCATE)
		//{
		//	std::cout << "NOTE_TRUNCATE -> PN_MODIFY" << std::endl;
		//}
		//if (event.fflags & NOTE_EXTEND)
		//{
		//	std::cout << "NOTE_EXTEND -> PN_MODIFY" << std::endl;
		//}
		//if (event.fflags & NOTE_ATTRIB)
		//{
		//	std::cout << "NOTE_ATTRIB -> PN_ATTRIB" << std::endl;
		//}
		//if (event.fflags & NOTE_REVOKE)
		//{
		//	std::cout << "NOTE_REVOKE -> XXXXXXXXX" << std::endl;
		//}
		//if (event.fflags & NOTE_LINK)
		//{
		//	std::cout << "NOTE_LINK -> XXXXXXXXX" << std::endl;
		//}
	}

	bool is_initialized_;
	int kqueue_file_descriptor_; // file descriptor

};


} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_KQUEUE_WRAPPER_HPP
