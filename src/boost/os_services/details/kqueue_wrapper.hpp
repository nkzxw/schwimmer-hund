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

// POSIX headers
#include <sys/event.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <boost/noncopyable.hpp>

//#include <boost/os_services/details/kqueue_watch_item.hpp> //TODO: ver si lo saco


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

//class kevent_error : public std::runtime_error 
//{
//public:
//	kevent_error ( const std::string& err ) : std::runtime_error ( err ) 
//	{}
//};


class kevent_timeout : public std::runtime_error 
{
public:
	kevent_timeout ( const std::string& err ) : std::runtime_error ( err ) 
	{}
};


struct null_deleter
{
	void operator()(void const* pepe) const
	{
		//std::cout << "void null_deleter::operator()(void const *) const" << std::endl;
		//std::cout << "pepe: " << pepe << std::endl;
		//const filesystem_item* temp = reinterpret_cast<const filesystem_item*>(pepe);
		//std::cout << "temp: " << temp << std::endl;
		//std::cout << "temp->parent().get(): " << temp->parent().get() << std::endl;

		//std::cout << "END void null_deleter::operator()(void const *) const" << std::endl;
	}
};


//TODO: debe ser singleton, ver como implementarlo...
//TODO: heredar de una clase abstracta, para poder hacer un dummy object con las mismas funcionalidades, salvo que se templarice
class kqueue_wrapper : private boost::noncopyable
{
public:

	kqueue_wrapper()
		: is_initialized_( false ), file_descriptor_( 0 )
	{
		std::cout << "kqueue_wrapper()" << std::endl;
	}

	~kqueue_wrapper()
	{
		std::cout << "~kqueue_wrapper()" << std::endl;
		close( true );
		std::cout << "END ~kqueue_wrapper()" << std::endl;
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
		std::cout << "void kqueue_wrapper::close( bool no_throw = false )" << std::endl;

		if ( file_descriptor_ != 0 )
		{

			std::cout << "closing kqueue file_descriptor_: " << file_descriptor_ << std::endl;


			//TODO: se puede usar el close_file de posix_wapper
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

					
					std::cout << "THROW - void kqueue_wrapper::close( bool no_throw = false )" << std::endl;

					throw (std::runtime_error(oss.str()));					
				}
			}
			file_descriptor_ = 0;
		}
		is_initialized_ = false;

		std::cout << "void END kqueue_wrapper::close( bool no_throw = false )" << std::endl;
	}

	template <typename T>
	void add_watch( const boost::shared_ptr<T>& watch )
	{
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

			std::cout << "THROW - void kqueue_wrapper::add_watch( const boost::shared_ptr<T>& watch )" << std::endl;

			throw (std::runtime_error(oss.str()));
		}
	}

	template <typename T>
	boost::shared_ptr<T> get( int& event_type )
	{
		boost::shared_ptr<T> watch;
		event_type = kqueue_event_types::none;

		struct kevent event;
		struct timespec timeout;
		timeout.tv_sec = 0;
		timeout.tv_nsec = 300000; //300 milliseconds //TODO: sacar el hardcode, hacer configurable...

		int return_code = kevent ( file_descriptor_, NULL, 0, &event, 1, &timeout );

		if ( return_code == -1 )
		{
			std::ostringstream oss;
			oss << "kevent error - Reason: " << std::strerror(errno);
			//throw ( kevent_error( oss.str() ) );
			
			std::cout << "kevent error - Reason: " << std::strerror(errno) << std::endl;
			std::cout << "return_code: " << return_code << std::endl;
			std::cout << "event.flags: " << event.flags << std::endl;
			std::cout << "EV_ERROR: " << EV_ERROR << std::endl;
			std::cout << "file_descriptor_: " << file_descriptor_ << std::endl;
			std::cout << "event.udata: " << event.udata << std::endl;

			std::cout << "THROW - boost::shared_ptr<T> kqueue_wrapper::get( int& event_type )" << std::endl;
			throw (std::runtime_error(oss.str())); 
			//std::cerr << oss.str() << std::endl; //incluir para probar


		}
		else if ( return_code == 0 ) //timeout
		{
			std::ostringstream oss;
			oss << "kevent timeout - Reason: " << std::strerror(errno);
			//throw (std::runtime_error(oss.str()));

			std::cout << "THROW - boost::shared_ptr<T> kqueue_wrapper::get( int& event_type ) - 2" << std::endl;

			throw ( kevent_timeout( oss.str() ) );
		}
		else
		{

			if ( event.flags & EV_ERROR )
			{
				std::ostringstream oss;
				oss << "kevent flags error (EV_ERROR) - Flags: " <<  event.flags;
				//throw ( kevent_error( oss.str() ) );

				std::cout << "kevent error - Reason: " << std::strerror(errno) << std::endl;
				std::cout << "return_code: " << return_code << std::endl;
				std::cout << "event.flags: " << event.flags << std::endl;
				std::cout << "EV_ERROR: " << EV_ERROR << std::endl;
				std::cout << "file_descriptor_: " << file_descriptor_ << std::endl;
				std::cout << "event.udata: " << event.udata << std::endl;

				std::cout << "THROW - boost::shared_ptr<T> kqueue_wrapper::get( int& event_type )" << std::endl;

				throw (std::runtime_error(oss.str()));
				//std::cerr << oss.str() << std::endl; //incluir para probar

			}
			else
			{
				watch = create_watch_item<T>( event.udata ); //null deleter shared_ptr

				std::cout << "event.udata: " << event.udata << std::endl;
				std::cout << "watch->path().native_file_string(): " << watch->path().native_file_string() << std::endl;
				std::cout << "watch.use_count() ----- 2: " << watch.use_count() << std::endl;

				if ( event.fflags & NOTE_DELETE )
				{
					std::cout << "NOTE_DELETE" << std::endl;
					event_type = kqueue_event_types::remove;
				}
				else if ( event.fflags & NOTE_RENAME )
				{
					std::cout << "NOTE_RENAME" << std::endl;
					event_type = kqueue_event_types::rename;
				}
				else if ( event.fflags & NOTE_WRITE )
				{
					std::cout << "NOTE_WRITE" << std::endl;
					event_type = kqueue_event_types::write;
				}

				//NOTE_TRUNCATE
				//NOTE_EXTEND
				//NOTE_ATTRIB
				//NOTE_REVOKE
				//NOTE_LINK
			}
		}

		return watch;
	}

protected:

	template <typename T>
	static boost::shared_ptr<T> create_watch_item ( void* raw_pointer )
	{
		boost::shared_ptr<T> px( reinterpret_cast<T*>( raw_pointer ), null_deleter() );
		std::cout << "px.use_count() ----- 1: " << px.use_count() << std::endl;
		return px;
	}

	bool is_initialized_;
	int file_descriptor_;

};


} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_KQUEUE_WRAPPER_HPP
