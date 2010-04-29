#ifndef BOOST_OS_SERVICES_DETAIL_IMPL_SELECTOR_HPP_INCLUDED
#define BOOST_OS_SERVICES_DETAIL_IMPL_SELECTOR_HPP_INCLUDED

//#include <boost/config.hpp>

//#elif defined(BOOST_ASIO_HAS_KQUEUE)



/*

//TODO: chequear...

#include <boost/config.hpp>

#if defined(BOOST_MSVC)


#if (BOOST_MSVC >= 1310)
#pragma warning(disable: 4675)  // resolved overload found with Koenig lookup
#endif

#endif

*/


//TODO: Aparentemente MAXOSX, NETBSD, OPENBSD, etc todos son iguales...
//TODO: Agregar OpenSolaris
//TODO: ver OpenBSD y NetBSD
//TODO: ver que hacemos con IBM AIX, HP-UX y ver que otra opcion puede haber
//TODO: SunOS

// #ifdef BOOST_WINDOWS
#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__) 
#include <boost/os_services/detail/linux_impl.hpp>
#elif defined(__FreeBSD__) // || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
#include <boost/os_services/detail/freebsd_impl.hpp>
#elif defined(__CYGWIN__)
//TODO: ver si se puede implementar usando las api de windows. No creo que esté inotify disponible para cygwin
//#include <boost/os_services/detail/unsupported_impl.hpp>
#  error Platform not supported
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
//#  error Platform not supported
#include <boost/os_services/detail/windows_impl.hpp> //TODO: dummy
//#include <boost/os_services/detail/freebsd_impl.hpp>
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#include <boost/os_services/detail/macosx_impl.hpp>
#else
#  error Platform not supported
//#include <boost/os_services/detail/unsupported_impl.hpp>
#endif


namespace boost {
namespace os_services {
namespace detail {

// #ifdef BOOST_WINDOWS
#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__) 
	typedef linux_impl fsm_implementation_type;
#elif defined(__FreeBSD__) // || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
	typedef freebsd_impl fsm_implementation_type;
#elif defined(__CYGWIN__)
	typedef unsupported_impl fsm_implementation_type;
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
	typedef windows_impl fsm_implementation_type; //TODO: dummy
	//typedef freebsd_impl fsm_implementation_type;
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
	typedef macosx_impl fsm_implementation_type;
#else
	typedef unsupported_impl fsm_implementation_type;
#endif

	
} // namespace detail
} // namespace os_services
} // namespace boost



//namespace boost
//{
//namespace detail
//{
//	//typedef FileSystemMonitorImpl<PlatformImpl> ImplementationType;
//	typedef PlatformImpl fsm_implementation_type;
//} // namespace detail
//} // namespace boost



//}


#endif // BOOST_OS_SERVICES_DETAIL_IMPL_SELECTOR_HPP_INCLUDED
