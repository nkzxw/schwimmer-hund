#ifndef BOOST_OS_SERVICES_DETAIL_KQUEUE_WRAPPER_HPP
#define BOOST_OS_SERVICES_DETAIL_KQUEUE_WRAPPER_HPP

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

#include <sys/event.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

//#include <boost/bind.hpp>
//#include <boost/enable_shared_from_this.hpp>
//#include <boost/filesystem/path.hpp>
//#include <boost/function.hpp>
//#include <boost/shared_ptr.hpp>
//#include <boost/thread.hpp>

//#include <boost/os_services/change_types.hpp>

#include <boost/os_services/details/kqueue_watch_item.hpp>



//TODO: sacar
/* kqueue(4) in MacOS/X does not support NOTE_TRUNCATE */
#ifndef NOTE_TRUNCATE
# define NOTE_TRUNCATE 0
#endif



namespace boost {
namespace os_services {
namespace detail {

namespace kqueue_event_types
{
	static const int none = 0;
	static const int write = 1;
	static const int remove = 2;
	static const int rename = 3;
}

class kevent_error : public std::runtime_error 
{
public:
	kevent_error ( const std::string& err ) : std::runtime_error ( err ) 
	{}
};

class kevent_timeout : public std::runtime_error 
{
public:
	kevent_timeout ( const std::string& err ) : std::runtime_error ( err ) 
	{}
};



//TODO: comentado solamente para probar porque es que no se está llamando al destructor
//struct null_deleter
//{
//	void operator()(void const *) const
//	{
//	}
//};


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

	//TODO: ver si me conviene templetizar este metodo y hacer desaparecer el kqueue_watch_item
	//void add_watch( filesystem_item::pointer_type watch, bool launch_events = false )
	//void add_watch( kqueue_watch_item* watch ) //TODO: puntero, referencia, shared_ptr ????
	//void add_watch( typename T::pointer_type watch ) //TODO: puntero, referencia, shared_ptr ????

	template <typename T>
	void add_watch( const boost::shared_ptr<T>& watch ) //TODO: puntero, referencia, shared_ptr ????
	{
		//Necesito:
		//			mask
		//			un file_descriptor del archivo a monitorear -> el archivo debe estar abierto
		//			puntero a la data que quiero recuperar
		//

		//TODO: ver si Windows y Linux saltan cuando se mofica el nombre del directorio raiz monitoreado.
		// sino saltan, evisar que se use NOTE_RENAME con cualquier directorio raiz

		// -> Windows no salta...
		// -> Linux tampoco...
		// -> FreeBSD: ????????????

		struct kevent event;

		//TODO: traducir de watch->mask_ a fflags
		//TODO: ver estos flags, deberia monitoriarse solo lo que el usuairo quiera monitorear...
		
		//int mask = watch->mask_;
		int mask = watch->mask();

		unsigned int fflags = NOTE_DELETE |  NOTE_WRITE | NOTE_EXTEND | NOTE_ATTRIB | NOTE_LINK | NOTE_REVOKE | NOTE_RENAME;

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


		//EV_SET( &event, watch->file_descriptor_, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_CLEAR, fflags, 0, watch ); // EV_ADD | EV_ENABLE | EV_ONESHOT | EV_CLEAR
		EV_SET( &event, watch->file_descriptor_, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_CLEAR, fflags, 0, watch.get() ); // EV_ADD | EV_ENABLE | EV_ONESHOT | EV_CLEAR

		int return_code = kevent( file_descriptor_, &event, 1, NULL, 0, NULL );
		if ( return_code == -1 ) //< 0)
		{
			std::ostringstream oss;
			oss << "kevent error: - Reason: " << std::strerror(errno);
			throw (std::runtime_error(oss.str()));
		}
	}

	template <typename T>
	boost::shared_ptr<T> get( int& event_type )
	{
		event_type = kqueue_event_types::none;
		struct kevent event;

		struct timespec timeout;
		timeout.tv_sec = 0;
		timeout.tv_nsec = 300000; //300 milliseconds //TODO: sacar el hardcode, hacer configurable...

		int return_code = kevent ( file_descriptor_, NULL, 0, &event, 1, &timeout );

		if ( return_code == -1 || event.flags & EV_ERROR) //< 0
		{
			std::ostringstream oss;
			oss << "kevent error - Reason: " << std::strerror(errno);
			//throw (std::runtime_error(oss.str()));
			throw ( kevent_error( oss.str() ) );
		}
		else if ( return_code == 0 ) //timeout
		{
			std::ostringstream oss;
			oss << "kevent timeout - Reason: " << std::strerror(errno);
			//throw (std::runtime_error(oss.str()));
			throw ( kevent_timeout( oss.str() ) );
		}
		else
		{
			boost::shared_ptr<T> watch = create_watch_item<T>( event.udata );

			if ( event.fflags & NOTE_DELETE )
			{
				event_type = kqueue_event_types::remove;
			}
			else if ( event.fflags & NOTE_RENAME )
			{
				event_type = kqueue_event_types::rename;
			}
			else if ( event.fflags & NOTE_WRITE )
			{
				event_type = kqueue_event_types::write;
			}

			//if (event.fflags & NOTE_TRUNCATE)
			//{
			//}
			//if (event.fflags & NOTE_EXTEND)
			//{
			//}
			//if (event.fflags & NOTE_ATTRIB)
			//{
			//}
			//if (event.fflags & NOTE_REVOKE)
			//{
			//}
			//if (event.fflags & NOTE_LINK)
			//{
			//}
		}
	}

protected:

	template <typename T>
	boost::shared_ptr<T> create_watch_item ( void* raw_pointer )
	{
		//boost::shared_ptr<T> px( reinterpret_cast<T*>( raw_pointer ), null_deleter() );
		boost::shared_ptr<T> px( reinterpret_cast<T*>( raw_pointer ) ); //TODO: comentado solamente para probar porque es que no se está llamando al destructor
		return px;
	}

	bool is_initialized_;
	int file_descriptor_;

};


} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_KQUEUE_WRAPPER_HPP
