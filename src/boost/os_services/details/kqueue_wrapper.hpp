#ifndef BOOST_OS_SERVICES_DETAIL_KQUEUE_WRAPPER_HPP
#define BOOST_OS_SERVICES_DETAIL_KQUEUE_WRAPPER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif

//TODO: revisar los headers

// C-Std Headers
//#include <cerrno>	//TODO: probar si es necesario
//#include <cstdio>	//TODO: probar si es necesario
//#include <cstdlib>	//TODO: probar si es necesario
//#include <cstring>	// for strerror
//
//#include <sys/event.h>
//#include <sys/fcntl.h>
//#include <sys/time.h>
//#include <sys/types.h>
//#include <unistd.h>

//#include <boost/bind.hpp>
//#include <boost/enable_shared_from_this.hpp>
//#include <boost/filesystem/path.hpp>
//#include <boost/function.hpp>
//#include <boost/shared_ptr.hpp>
//#include <boost/thread.hpp>

#include <boost/os_services/change_types.hpp>
#include <boost/os_services/details/kqueue_watch_item.hpp>



//TODO: sacar
/* kqueue(4) in MacOS/X does not support NOTE_TRUNCATE */
#ifndef NOTE_TRUNCATE
# define NOTE_TRUNCATE 0
#endif



namespace boost {
namespace os_services {
namespace detail {


//TODO: debe ser singleton, ver como implementarlo...
class kqueue_wrapper //TODO: heredar de una clase abstracta
{
public:

	kqueue_wrapper()
		: is_initialized_( false ), file_descriptor_( 0 )
	{}

	~kqueue_wrapper()
	{
		close( true );
	}

	void initialize()
	{
		if ( ! is_initialized_ )
		{
			file_descriptor_ = kqueue(); //::kqueue();

			if ( file_descriptor_ == -1 )   //< 0)
			{
				std::ostringstream oss;
				oss << "Failed to initialize kqueue - Reason: " << std::strerror(errno);
				throw (std::runtime_error(oss.str()));
			}
			is_initialized_ = true;
		}
	}

	void close( bool no_throw = false )
	{
		if ( file_descriptor_ != 0 )
		{
			int ret_value = ::close( file_descriptor_ );
			if ( ret_value == -1 )
			{
				if ( no_throw )
				{
					//Destructor -> no-throw
					std::cerr << "Failed to close kqueue file descriptor - File Descriptor: '" << file_descriptor_ << "' - Reason: " << std::strerror(errno) << std::endl; 
				}
				else
				{
					std::ostringstream oss;
					oss << "Failed to close kqueue file descriptor - File Descriptor: '" << file_descriptor_ << "' - Reason: " << std::strerror(errno);
					throw (std::runtime_error(oss.str()));					
				}
			}
			file_descriptor_ = 0;
		}
		is_initialized_ = false;
	}



	void add_watch( filesystem_item::pointer_type watch, bool launch_events = false )
	{

		//Necesito:
		//			mask
		//			un file_descriptor del archivo a monitorear -> el archivo debe estar abierto
		//			puntero a la data que quiero recuperar
		//

		struct kevent event;

		//int mask = watch->mask_;

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

		//TODO: no est� incluido en PN_ALL_EVENTS por eso lo agrego de una
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

		int return_code = kevent( file_descriptor_, &event, 1, NULL, 0, NULL );
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
		int return_code = kevent ( file_descriptor_, NULL, 0, &event, 1, &timeout );

		if ( return_code == -1 || event.flags & EV_ERROR) //< 0
		{
			//TODO: evaluar si este throw est� relacionado con el destructor de fsm ya que est� ejecutado en otro thread, no deberia... pero...
			std::ostringstream oss;
			oss << "kevent error - Reason: " << std::strerror(errno);
			throw (std::runtime_error(oss.str()));
		}


		if ( return_code == 0 ) //timeout
		{
		}


		if ( event.fflags & NOTE_DELETE )
		{
		}

		if ( event.fflags & NOTE_RENAME )
		{
		}

		if ( event.fflags & NOTE_WRITE )
		{
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

protected:
	bool is_initialized_;
	int file_descriptor_;

};


} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_KQUEUE_WRAPPER_HPP
